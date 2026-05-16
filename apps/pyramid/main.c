#include <stddef.h>
#include <math.h>
#include "vkbase.h"
#include "vkbnative.h"
#include "shaders/shaders.h"

#define BRICK_TEX_W 256
#define BRICK_TEX_H 256

typedef struct { float pos[3]; float uv[2]; } Vertex;

/* Egyptian square pyramid: 4 base corners + 1 apex = 5 unique positions.
   Vertices are split per-face so each triangular face gets its own UV space. */
static const Vertex s_vertices[16] = {
    /* front  (-Z) */  {{-0.5f, 0.0f, -0.5f}, {0.0f, 1.0f}},
                       {{ 0.5f, 0.0f, -0.5f}, {1.0f, 1.0f}},
                       {{ 0.0f, 0.8f,  0.0f}, {0.5f, 0.0f}},
    /* right  (+X) */  {{ 0.5f, 0.0f, -0.5f}, {0.0f, 1.0f}},
                       {{ 0.5f, 0.0f,  0.5f}, {1.0f, 1.0f}},
                       {{ 0.0f, 0.8f,  0.0f}, {0.5f, 0.0f}},
    /* back   (+Z) */  {{ 0.5f, 0.0f,  0.5f}, {0.0f, 1.0f}},
                       {{-0.5f, 0.0f,  0.5f}, {1.0f, 1.0f}},
                       {{ 0.0f, 0.8f,  0.0f}, {0.5f, 0.0f}},
    /* left   (-X) */  {{-0.5f, 0.0f,  0.5f}, {0.0f, 1.0f}},
                       {{-0.5f, 0.0f, -0.5f}, {1.0f, 1.0f}},
                       {{ 0.0f, 0.8f,  0.0f}, {0.5f, 0.0f}},
    /* base (square) */{{-0.5f, 0.0f, -0.5f}, {0.0f, 0.0f}},
                       {{ 0.5f, 0.0f, -0.5f}, {1.0f, 0.0f}},
                       {{ 0.5f, 0.0f,  0.5f}, {1.0f, 1.0f}},
                       {{-0.5f, 0.0f,  0.5f}, {0.0f, 1.0f}},
};

static const uint16_t s_indices[18] = {
     0,  1,  2,   /* front  */
     3,  4,  5,   /* right  */
     6,  7,  8,   /* back   */
     9, 10, 11,   /* left   */
    12, 13, 14,   /* base-a */
    12, 14, 15,   /* base-b */
};

/* Vertex input descriptors for pipeline creation */
static const VkVertexInputBindingDescription s_vtxBinding = {
    0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX
};
static const VkVertexInputAttributeDescription s_vtxAttribs[2] = {
    {0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0},
    {1, 0, VK_FORMAT_R32G32_SFLOAT,    offsetof(Vertex, uv)},
};

/* --- Brick texture -------------------------------------------------- */

static void generate_brick_texture(uint8_t *pixels) {
    const int ROW_H = 28, COL_W = 56, MORT_H = 4, MORT_W = 4;
    for (int y = 0; y < BRICK_TEX_H; y++) {
        int row   = y / ROW_H;
        int row_y = y % ROW_H;
        for (int x = 0; x < BRICK_TEX_W; x++) {
            int offset = (row & 1) ? COL_W / 2 : 0;
            int col_x  = (x + offset) % COL_W;
            uint8_t r, g, b;
            if (row_y >= ROW_H - MORT_H || col_x >= COL_W - MORT_W) {
                r = 215; g = 200; b = 175;
            } else {
                int brick = (x + offset) / COL_W;
                int seed  = (row * 97 + brick * 31) & 0xff;
                int var   = ((seed * 7) & 0x1f) - 12;
                r = (uint8_t)(175 + var);
                g = (uint8_t)(140 + (var >> 1));
                b = (uint8_t)( 88 + (var / 3));
            }
            uint8_t *p = pixels + (y * BRICK_TEX_W + x) * 4;
            p[0] = r; p[1] = g; p[2] = b; p[3] = 255;
        }
    }
}

/* --- Column-major mat4 math (matches GLSL layout) ------------------- */

typedef struct { float m[16]; } Mat4;

static Mat4 mat4_identity(void) {
    Mat4 r = {{0}};
    r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.0f;
    return r;
}

/* result = a * b, both column-major */
static Mat4 mat4_mul(Mat4 a, Mat4 b) {
    Mat4 r = {{0}};
    for (int c = 0; c < 4; c++)
        for (int row = 0; row < 4; row++)
            for (int k = 0; k < 4; k++)
                r.m[c*4 + row] += a.m[k*4 + row] * b.m[c*4 + k];
    return r;
}

/* Perspective projection — Vulkan clip space (Y down, depth [0,1], RH) */
static Mat4 mat4_perspective(float fovY, float aspect, float zNear, float zFar) {
    float f = 1.0f / tanf(fovY * 0.5f);
    Mat4 r = {{0}};
    r.m[0]  =  f / aspect;
    r.m[5]  = -f;                                    /* flip Y for Vulkan NDC */
    r.m[10] = zFar / (zNear - zFar);
    r.m[11] = -1.0f;
    r.m[14] = (zNear * zFar) / (zNear - zFar);
    return r;
}

/* View matrix — camera at (ex,ey,ez) looking at (tx,ty,tz), up=(0,1,0) */
static Mat4 mat4_lookat(float ex, float ey, float ez,
                         float tx, float ty, float tz) {
    float fx = tx-ex, fy = ty-ey, fz = tz-ez;
    float fl = sqrtf(fx*fx + fy*fy + fz*fz);
    fx /= fl; fy /= fl; fz /= fl;
    /* right = cross(forward, world_up) */
    float rx = -fz, ry = 0.0f, rz = fx;
    float rl = sqrtf(rx*rx + ry*ry + rz*rz);
    rx /= rl; ry /= rl; rz /= rl;
    /* recalculated up = cross(right, forward) */
    float ux = ry*fz - rz*fy, uy = rz*fx - rx*fz, uz = rx*fy - ry*fx;
    Mat4 r = {{0}};
    r.m[0] = rx;  r.m[4] = ry;  r.m[8]  = rz;  r.m[12] = -(rx*ex + ry*ey + rz*ez);
    r.m[1] = ux;  r.m[5] = uy;  r.m[9]  = uz;  r.m[13] = -(ux*ex + uy*ey + uz*ez);
    r.m[2] = -fx; r.m[6] = -fy; r.m[10] = -fz; r.m[14] =   fx*ex + fy*ey + fz*ez;
    r.m[15] = 1.0f;
    return r;
}

/* Rotation around Y axis */
static Mat4 mat4_rotY(float a) {
    float c = cosf(a), s = sinf(a);
    Mat4 r = mat4_identity();
    r.m[0] = c;  r.m[8]  =  s;
    r.m[2] = -s; r.m[10] =  c;
    return r;
}

/* --- One-shot command buffer helpers -------------------------------- */

static VkResult begin_one_shot(VkbBase *pb, VkCommandPool pool, VkCommandBuffer *cmd) {
    VkCommandBufferAllocateInfo ai = *vkb_emptyCommandBuffer();
    ai.commandPool        = pool;
    ai.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    ai.commandBufferCount = 1;
    VkResult r = vkAllocateCommandBuffers(pb->dev, &ai, cmd);
    if (r != VK_SUCCESS) return r;
    VkCommandBufferBeginInfo bi = *vkb_emptyCommandBufferBegin();
    bi.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    r = vkBeginCommandBuffer(*cmd, &bi);
    if (r != VK_SUCCESS) { vkFreeCommandBuffers(pb->dev, pool, 1, cmd); }
    return r;
}

static VkResult end_one_shot(VkbBase *pb, VkCommandPool pool, VkCommandBuffer cmd) {
    VkResult r = vkEndCommandBuffer(cmd);
    if (r == VK_SUCCESS) {
        VkSubmitInfo si      = *vkb_emptySubmit();
        si.commandBufferCount = 1;
        si.pCommandBuffers    = &cmd;
        r = vkQueueSubmit(pb->gq, 1, &si, VK_NULL_HANDLE);
        if (r == VK_SUCCESS) r = vkQueueWaitIdle(pb->gq);
    }
    vkFreeCommandBuffers(pb->dev, pool, 1, &cmd);
    return r;
}

/* --- Upload helpers (staging → device-local) ----------------------- */

static VkResult upload_buffer(VkbBase *pb, VkCommandPool pool,
                               VkbBuffer *dst, const void *data, VkDeviceSize size) {
    VkBufferCreateInfo si = *vkb_emptyBuffer();
    si.size = size; si.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    si.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    VkbBuffer stg = {0};
    VkResult r = vkb_createBuffer(pb->pdev, pb->dev, &si,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stg);
    if (r != VK_SUCCESS) return r;
    void *m;
    vkMapMemory(pb->dev, stg.memory, 0, size, 0, &m);
    memcpy(m, data, (size_t)size);
    vkUnmapMemory(pb->dev, stg.memory);
    VkCommandBuffer cmd;
    r = begin_one_shot(pb, pool, &cmd);
    if (r == VK_SUCCESS) {
        VkBufferCopy cp = {0, 0, size};
        vkCmdCopyBuffer(cmd, stg.buffer, dst->buffer, 1, &cp);
        r = end_one_shot(pb, pool, cmd);
    }
    vkb_destroyBuffer(pb->dev, stg);
    return r;
}

static VkResult upload_image(VkbBase *pb, VkCommandPool pool,
                              VkbShaderImage *dst, const void *pixels,
                              uint32_t w, uint32_t h) {
    VkDeviceSize size = (VkDeviceSize)w * h * 4;
    VkBufferCreateInfo si = *vkb_emptyBuffer();
    si.size = size; si.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    si.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    VkbBuffer stg = {0};
    VkResult r = vkb_createBuffer(pb->pdev, pb->dev, &si,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stg);
    if (r != VK_SUCCESS) return r;
    void *m;
    vkMapMemory(pb->dev, stg.memory, 0, size, 0, &m);
    memcpy(m, pixels, (size_t)size);
    vkUnmapMemory(pb->dev, stg.memory);
    VkCommandBuffer cmd;
    r = begin_one_shot(pb, pool, &cmd);
    if (r == VK_SUCCESS) {
        vkb_cmdTransitionImageLayout(cmd, dst->image.image, VK_FORMAT_R8G8B8A8_UNORM,
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        VkBufferImageCopy cp = {0};
        cp.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        cp.imageSubresource.layerCount = 1;
        cp.imageExtent = (VkExtent3D){w, h, 1};
        vkCmdCopyBufferToImage(cmd, stg.buffer, dst->image.image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &cp);
        vkb_cmdTransitionImageLayout(cmd, dst->image.image, VK_FORMAT_R8G8B8A8_UNORM,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        r = end_one_shot(pb, pool, cmd);
    }
    vkb_destroyBuffer(pb->dev, stg);
    return r;
}

/* ================================================================== */
/* App state — static so teardown() can always reach them             */
/* ================================================================== */

static VkInstance       instance  = VK_NULL_HANDLE;
static VkbNativeSurface ns;
static VkbBase          base;
static VkbSwapchain     swapchain;
static VkbFrameContext  ctx;
static VkbBuffer        vtxBuffer;
static VkbBuffer        idxBuffer;
static VkbTexture       brickTex;

/* pipeline state */
static VkbShaderImage   depthImage;
static VkRenderPass     renderPass  = VK_NULL_HANDLE;
static VkFramebuffer    framebuffers[VKB_MAX_SWAPCHAIN_IMAGES];
static VkDescriptorSetLayout dsLayout   = VK_NULL_HANDLE;
static VkPipelineLayout pipeLayout  = VK_NULL_HANDLE;
static VkDescriptorPool descPool    = VK_NULL_HANDLE;
static VkDescriptorSet  descSet     = VK_NULL_HANDLE;
static VkPipeline       pipeline    = VK_NULL_HANDLE;

static void teardown(void) {
    if (base.dev) {
        vkDeviceWaitIdle(base.dev);
        if (pipeline)    vkDestroyPipeline(base.dev, pipeline, NULL);
        if (pipeLayout)  vkDestroyPipelineLayout(base.dev, pipeLayout, NULL);
        if (descPool)    vkDestroyDescriptorPool(base.dev, descPool, NULL);
        if (dsLayout)    vkDestroyDescriptorSetLayout(base.dev, dsLayout, NULL);
        for (uint32_t i = 0; i < VKB_MAX_SWAPCHAIN_IMAGES; i++)
            if (framebuffers[i]) vkDestroyFramebuffer(base.dev, framebuffers[i], NULL);
        if (renderPass)  vkDestroyRenderPass(base.dev, renderPass, NULL);
        if (depthImage.view) vkb_destroyShaderImage(base.dev, depthImage);
        if (brickTex.image.view) vkb_destroyTexture(base.dev, brickTex);
        if (idxBuffer.buffer)    vkb_destroyBuffer(base.dev, idxBuffer);
        if (vtxBuffer.buffer)    vkb_destroyBuffer(base.dev, vtxBuffer);
        if (ctx.cmdPool)         vkb_destroyFrameContext(&base, &ctx);
        if (swapchain.chain)     vkb_destroySwapchain(&base, &swapchain);
        vkb_destroyBase(&base);
    }
    if (ns.surface) vkb_destroyNativeSurface(instance, &ns);
    if (instance)   vkDestroyInstance(instance, NULL);
}

/* ================================================================== */

int main(void) {
    /* Instance */
    VkApplicationInfo appInfo = vkb_simpleApplication("pyramid", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0);
    VkInstanceCreateInfo instInfo = vkb_simpleInstance(&appInfo, VK_FALSE);
    if (vkCreateInstance(&instInfo, NULL, &instance) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create VkInstance\n");
        return 1;
    }

    /* Native surface — created before physical device selection so we can
       query present support against the real surface. */
    if (vkb_createNativeSurface(instance, "pyramid", 800, 600, &ns) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create native surface\n");
        teardown(); return 1;
    }

    /* Physical device + logical device + queues */
    if (vkb_createBase(instance, ns.surface, NULL, &base) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create base\n");
        teardown(); return 1;
    }
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(base.pdev, &props);
    printf("Physical device: %s\n", props.deviceName);

    /* Swapchain */
    if (vkb_createSwapchain(&base, &ns, NULL, &swapchain) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create swapchain\n");
        teardown(); return 1;
    }
    printf("Swapchain created (%u images, %ux%u)\n",
           swapchain.imageCount, swapchain.extent.width, swapchain.extent.height);

    /* Frame context — command pool, command buffers, and sync objects per frame */
    if (vkb_createFrameContext(&base, VKB_MAX_FRAMES_IN_FLIGHT, &ctx) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create frame context\n");
        teardown(); return 1;
    }
    printf("Frame context created (%u frames in flight)\n", ctx.frameCount);

    /* ----------------------------------------------------------------
       Vertex buffer — device-local, uploaded via staging
       ---------------------------------------------------------------- */
    VkBufferCreateInfo vtxInfo = *vkb_emptyBuffer();
    vtxInfo.size        = sizeof(s_vertices);
    vtxInfo.usage       = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    vtxInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (vkb_createBuffer(base.pdev, base.dev, &vtxInfo,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vtxBuffer) != VK_SUCCESS
     || upload_buffer(&base, ctx.cmdPool, &vtxBuffer, s_vertices, sizeof(s_vertices)) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create/upload vertex buffer\n");
        teardown(); return 1;
    }
    printf("Vertex buffer: %u bytes, %u vertices (vec3 pos + vec2 uv)\n",
           (uint32_t)sizeof(s_vertices),
           (uint32_t)(sizeof(s_vertices) / sizeof(s_vertices[0])));

    /* ----------------------------------------------------------------
       Index buffer — device-local, uploaded via staging
       ---------------------------------------------------------------- */
    VkBufferCreateInfo idxInfo = *vkb_emptyBuffer();
    idxInfo.size        = sizeof(s_indices);
    idxInfo.usage       = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    idxInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (vkb_createBuffer(base.pdev, base.dev, &idxInfo,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &idxBuffer) != VK_SUCCESS
     || upload_buffer(&base, ctx.cmdPool, &idxBuffer, s_indices, sizeof(s_indices)) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create/upload index buffer\n");
        teardown(); return 1;
    }
    printf("Index buffer: %u bytes, %u indices (uint16)\n",
           (uint32_t)sizeof(s_indices),
           (uint32_t)(sizeof(s_indices) / sizeof(s_indices[0])));

    /* ----------------------------------------------------------------
       Brick texture — procedurally generated sandstone pattern,
       uploaded as R8G8B8A8_UNORM SHADER_READ_ONLY_OPTIMAL
       ---------------------------------------------------------------- */
    static uint8_t brickPixels[BRICK_TEX_H * BRICK_TEX_W * 4];
    generate_brick_texture(brickPixels);

    VkImageCreateInfo imgInfo = vkb_simpleImage2D(VK_FORMAT_R8G8B8A8_UNORM,
        (VkExtent2D){BRICK_TEX_W, BRICK_TEX_H},
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    VkSamplerCreateInfo sampInfo = vkb_simpleSampler(VK_FILTER_LINEAR,
        VK_SAMPLER_ADDRESS_MODE_REPEAT);
    if (vkb_createTexture(base.pdev, base.dev, &imgInfo,
                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                          VK_IMAGE_ASPECT_COLOR_BIT, &sampInfo, &brickTex) != VK_SUCCESS
     || upload_image(&base, ctx.cmdPool, &brickTex.image,
                     brickPixels, BRICK_TEX_W, BRICK_TEX_H) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create/upload brick texture\n");
        teardown(); return 1;
    }
    printf("Brick texture: %ux%u RGBA UNORM, SHADER_READ_ONLY_OPTIMAL\n",
           BRICK_TEX_W, BRICK_TEX_H);

    /* ----------------------------------------------------------------
       Depth image — D32_SFLOAT, same extent as swapchain
       ---------------------------------------------------------------- */
    VkImageCreateInfo depthImgInfo = vkb_simpleImage2D(VK_FORMAT_D32_SFLOAT,
        swapchain.extent,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    if (vkb_createShaderImage(base.pdev, base.dev, &depthImgInfo,
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                              VK_IMAGE_ASPECT_DEPTH_BIT, &depthImage) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create depth image\n");
        teardown(); return 1;
    }
    printf("Depth image: %ux%u D32_SFLOAT\n",
           swapchain.extent.width, swapchain.extent.height);

    /* ----------------------------------------------------------------
       Render pass — color (PRESENT_SRC_KHR) + depth (D32_SFLOAT)
       ---------------------------------------------------------------- */
    VkAttachmentDescription attachDescs[2] = {
        vkb_simpleColorAttachment(swapchain.format, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR),
        vkb_simpleDepthAttachment(VK_FORMAT_D32_SFLOAT),
    };
    VkAttachmentReference colorRef = vkb_colorAttachmentRef(0);
    VkAttachmentReference depthRef = vkb_depthAttachmentRef(1);
    VkSubpassDescription  subpass  = vkb_simpleGraphicsSubpass(&colorRef, &depthRef);
    VkSubpassDependency   dep      = vkb_simpleExternalDependency();
    VkRenderPassCreateInfo rpInfo  = vkb_simpleRenderPass(
        2, attachDescs, 1, &subpass, 1, &dep);
    if (vkCreateRenderPass(base.dev, &rpInfo, NULL, &renderPass) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create render pass\n");
        teardown(); return 1;
    }

    /* ----------------------------------------------------------------
       Framebuffers — one per swapchain image, color + depth views
       ---------------------------------------------------------------- */
    for (uint32_t i = 0; i < swapchain.imageCount; i++) {
        VkImageView fbViews[2] = {swapchain.views[i], depthImage.view};
        VkFramebufferCreateInfo fbInfo = vkb_simpleFramebuffer(
            renderPass, 2, fbViews,
            swapchain.extent.width, swapchain.extent.height, 1);
        if (vkCreateFramebuffer(base.dev, &fbInfo, NULL, &framebuffers[i]) != VK_SUCCESS) {
            fprintf(stderr, "Failed to create framebuffer %u\n", i);
            teardown(); return 1;
        }
    }
    printf("Framebuffers: %u created\n", swapchain.imageCount);

    /* ----------------------------------------------------------------
       Descriptor set layout — binding 0: combined image sampler (frag)
       ---------------------------------------------------------------- */
    VkDescriptorSetLayoutBinding samplerBinding = {
        .binding         = 0,
        .descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1,
        .stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT,
    };
    VkDescriptorSetLayoutCreateInfo dsLayoutInfo = *vkb_emptyDescriptorSetLayout();
    dsLayoutInfo.bindingCount = 1;
    dsLayoutInfo.pBindings    = &samplerBinding;
    if (vkCreateDescriptorSetLayout(base.dev, &dsLayoutInfo, NULL, &dsLayout) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create descriptor set layout\n");
        teardown(); return 1;
    }

    /* ----------------------------------------------------------------
       Pipeline layout — descriptor set + push constant (mat4 MVP)
       ---------------------------------------------------------------- */
    VkPushConstantRange pcRange = {
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .offset     = 0,
        .size       = sizeof(Mat4),
    };
    VkPipelineLayoutCreateInfo plInfo = *vkb_emptyPipelineLayout();
    plInfo.setLayoutCount         = 1;
    plInfo.pSetLayouts            = &dsLayout;
    plInfo.pushConstantRangeCount = 1;
    plInfo.pPushConstantRanges    = &pcRange;
    if (vkCreatePipelineLayout(base.dev, &plInfo, NULL, &pipeLayout) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create pipeline layout\n");
        teardown(); return 1;
    }

    /* ----------------------------------------------------------------
       Descriptor pool + set — one combined-image-sampler for the texture
       ---------------------------------------------------------------- */
    VkDescriptorPoolSize poolSize = {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1};
    VkDescriptorPoolCreateInfo dpInfo = *vkb_emptyDescriptorPool();
    dpInfo.maxSets       = 1;
    dpInfo.poolSizeCount = 1;
    dpInfo.pPoolSizes    = &poolSize;
    if (vkCreateDescriptorPool(base.dev, &dpInfo, NULL, &descPool) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create descriptor pool\n");
        teardown(); return 1;
    }
    VkDescriptorSetAllocateInfo dsAllocInfo = *vkb_emptyDescriptorSet();
    dsAllocInfo.descriptorPool     = descPool;
    dsAllocInfo.descriptorSetCount = 1;
    dsAllocInfo.pSetLayouts        = &dsLayout;
    if (vkAllocateDescriptorSets(base.dev, &dsAllocInfo, &descSet) != VK_SUCCESS) {
        fprintf(stderr, "Failed to allocate descriptor set\n");
        teardown(); return 1;
    }
    VkDescriptorImageInfo texInfo = {
        .sampler     = brickTex.sampler,
        .imageView   = brickTex.image.view,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };
    VkWriteDescriptorSet dsWrite = *vkb_emptyWriteDescriptorSet();
    dsWrite.dstSet          = descSet;
    dsWrite.dstBinding      = 0;
    dsWrite.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    dsWrite.descriptorCount = 1;
    dsWrite.pImageInfo      = &texInfo;
    vkUpdateDescriptorSets(base.dev, 1, &dsWrite, 0, NULL);

    /* ----------------------------------------------------------------
       Graphics pipeline
       ---------------------------------------------------------------- */
    VkShaderModuleCreateInfo vertModInfo = vkb_simpleShaderModule(
        sizeof(pyramid_vert_spv), pyramid_vert_spv);
    VkShaderModuleCreateInfo fragModInfo = vkb_simpleShaderModule(
        sizeof(pyramid_frag_spv), pyramid_frag_spv);
    VkShaderModule vertModule = VK_NULL_HANDLE, fragModule = VK_NULL_HANDLE;
    if (vkCreateShaderModule(base.dev, &vertModInfo, NULL, &vertModule) != VK_SUCCESS ||
        vkCreateShaderModule(base.dev, &fragModInfo, NULL, &fragModule) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create shader modules\n");
        if (vertModule) vkDestroyShaderModule(base.dev, vertModule, NULL);
        teardown(); return 1;
    }

    VkPipelineShaderStageCreateInfo stages[2] = {
        vkb_simpleShaderStage(VK_SHADER_STAGE_VERTEX_BIT,   vertModule),
        vkb_simpleShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fragModule),
    };

    VkPipelineVertexInputStateCreateInfo vtxInputState =
        vkb_simpleVertexInputState(1, &s_vtxBinding, 2, s_vtxAttribs);

    VkPipelineInputAssemblyStateCreateInfo inputAssembly =
        vkb_simpleInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE);

    /* Viewport and scissor are set dynamically each frame */
    static const VkDynamicState dynStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR,
    };
    VkPipelineDynamicStateCreateInfo dynState =
        vkb_simpleDynamicState(2, dynStates);
    VkPipelineViewportStateCreateInfo vpState =
        vkb_simpleViewportState(NULL, NULL);

    VkPipelineRasterizationStateCreateInfo raster =
        vkb_simpleRasterizationState(VK_POLYGON_MODE_FILL,
                                     VK_CULL_MODE_NONE,
                                     VK_FRONT_FACE_COUNTER_CLOCKWISE);

    VkPipelineMultisampleStateCreateInfo ms =
        vkb_simpleMultisampleState(VK_SAMPLE_COUNT_1_BIT);

    VkPipelineDepthStencilStateCreateInfo ds =
        vkb_simpleDepthStencilState(VK_TRUE, VK_COMPARE_OP_LESS);

    VkPipelineColorBlendAttachmentState blendAttach = {
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
                        | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };
    float blendConst[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    VkPipelineColorBlendStateCreateInfo blend =
        vkb_simpleColorBlendState(&blendAttach, blendConst);
    blend.attachmentCount = 1;

    VkGraphicsPipelineCreateInfo pipeInfo = *vkb_emptyGraphicsPipeline();
    pipeInfo.stageCount          = 2;
    pipeInfo.pStages             = stages;
    pipeInfo.pVertexInputState   = &vtxInputState;
    pipeInfo.pInputAssemblyState = &inputAssembly;
    pipeInfo.pViewportState      = &vpState;
    pipeInfo.pRasterizationState = &raster;
    pipeInfo.pMultisampleState   = &ms;
    pipeInfo.pDepthStencilState  = &ds;
    pipeInfo.pColorBlendState    = &blend;
    pipeInfo.pDynamicState       = &dynState;
    pipeInfo.layout              = pipeLayout;
    pipeInfo.renderPass          = renderPass;
    pipeInfo.subpass             = 0;

    VkResult pipeResult = vkCreateGraphicsPipelines(
        base.dev, VK_NULL_HANDLE, 1, &pipeInfo, NULL, &pipeline);

    vkDestroyShaderModule(base.dev, fragModule, NULL);
    vkDestroyShaderModule(base.dev, vertModule, NULL);

    if (pipeResult != VK_SUCCESS) {
        fprintf(stderr, "Failed to create graphics pipeline\n");
        teardown(); return 1;
    }
    printf("Graphics pipeline created\n");

    /* ----------------------------------------------------------------
       Render loop
       ---------------------------------------------------------------- */
    static float angle = 0.0f;

    while (!vkb_nativeSurfaceShouldClose(&ns)) {
        vkb_nativeSurfacePollEvents();

        uint32_t imageIndex;
        VkCommandBuffer cmd;
        if (vkb_beginFrame(&base, &ctx, &swapchain, &imageIndex, &cmd) != VK_SUCCESS) {
            fprintf(stderr, "Failed to begin frame\n");
            break;
        }

        /* Begin render pass — clears color to dark blue-grey and depth to 1.0 */
        VkClearValue clearValues[2];
        clearValues[0].color        = (VkClearColorValue){{0.05f, 0.05f, 0.1f, 1.0f}};
        clearValues[1].depthStencil = (VkClearDepthStencilValue){1.0f, 0};

        VkRenderPassBeginInfo rpBegin = *vkb_emptyRenderPassBegin();
        rpBegin.renderPass        = renderPass;
        rpBegin.framebuffer       = framebuffers[imageIndex];
        rpBegin.renderArea.offset = (VkOffset2D){0, 0};
        rpBegin.renderArea.extent = swapchain.extent;
        rpBegin.clearValueCount   = 2;
        rpBegin.pClearValues      = clearValues;
        vkCmdBeginRenderPass(cmd, &rpBegin, VK_SUBPASS_CONTENTS_INLINE);

        /* Dynamic viewport and scissor */
        VkViewport viewport = {
            .x = 0.0f, .y = 0.0f,
            .width  = (float)swapchain.extent.width,
            .height = (float)swapchain.extent.height,
            .minDepth = 0.0f, .maxDepth = 1.0f,
        };
        VkRect2D scissor = {{0, 0}, swapchain.extent};
        vkCmdSetViewport(cmd, 0, 1, &viewport);
        vkCmdSetScissor(cmd, 0, 1, &scissor);

        /* Bind pipeline and texture descriptor */
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                pipeLayout, 0, 1, &descSet, 0, NULL);

        /* Compute and push MVP — slow Y-axis spin */
        angle += 0.008f;
        float aspect = (float)swapchain.extent.width / (float)swapchain.extent.height;
        Mat4 model = mat4_rotY(angle);
        Mat4 view  = mat4_lookat(0.0f, 0.7f, 2.0f,   /* eye */
                                  0.0f, 0.2f, 0.0f);  /* target */
        Mat4 proj  = mat4_perspective(0.8f, aspect, 0.1f, 100.0f);
        Mat4 mvp   = mat4_mul(mat4_mul(proj, view), model);
        vkCmdPushConstants(cmd, pipeLayout, VK_SHADER_STAGE_VERTEX_BIT,
                           0, sizeof(Mat4), mvp.m);

        /* Bind geometry and draw */
        VkDeviceSize vtxOffset = 0;
        vkCmdBindVertexBuffers(cmd, 0, 1, &vtxBuffer.buffer, &vtxOffset);
        vkCmdBindIndexBuffer(cmd, idxBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed(cmd, 18, 1, 0, 0, 0);

        vkCmdEndRenderPass(cmd);

        if (vkb_endFrame(&base, &ctx, &swapchain, imageIndex) != VK_SUCCESS) {
            fprintf(stderr, "Failed to end frame\n");
            break;
        }
    }

    teardown();
    printf("Done\n");
    return 0;
}
