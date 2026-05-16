#include "vkbase.h"
#include "vkbnative.h"

int main(void) {
    /* Instance */
    VkApplicationInfo appInfo = vkb_simpleApplication("pyramid", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0);
    VkInstanceCreateInfo instInfo = vkb_simpleInstance(&appInfo, VK_FALSE);

    VkInstance instance;
    VkResult result = vkCreateInstance(&instInfo, NULL, &instance);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to create VkInstance: %d\n", result);
        return 1;
    }

    /* Native surface — created before physical device selection so we can
       query present support against the real surface. */
    VkbNativeSurface ns;
    result = vkb_createNativeSurface(instance, "pyramid", 800, 600, &ns);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to create native surface: %d\n", result);
        vkDestroyInstance(instance, NULL);
        return 1;
    }

    /* Physical device + logical device + queues */
    VkbBase base;
    result = vkb_createBase(instance, ns.surface, NULL, &base);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to create base: %d\n", result);
        vkb_destroyNativeSurface(instance, &ns);
        vkDestroyInstance(instance, NULL);
        return 1;
    }

    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(base.pdev, &props);
    printf("Physical device: %s\n", props.deviceName);

    /* Swapchain */
    VkbSwapchain swapchain = {0};
    result = vkb_createSwapchain(&base, &ns, NULL, &swapchain);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to create swapchain: %d\n", result);
        vkb_destroyBase(&base);
        vkb_destroyNativeSurface(instance, &ns);
        vkDestroyInstance(instance, NULL);
        return 1;
    }
    printf("Swapchain created (%u images, %ux%u)\n",
           swapchain.imageCount, swapchain.extent.width, swapchain.extent.height);

    /* Frame context — command pool, command buffers, and sync objects per frame */
    VkbFrameContext ctx;
    result = vkb_createFrameContext(&base, VKB_MAX_FRAMES_IN_FLIGHT, &ctx);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to create frame context: %d\n", result);
        vkb_destroySwapchain(&base, &swapchain);
        vkb_destroyBase(&base);
        vkb_destroyNativeSurface(instance, &ns);
        vkDestroyInstance(instance, NULL);
        return 1;
    }
    printf("Frame context created (%u frames in flight)\n", ctx.frameCount);

    /* Acquire-present loop */
    VkClearColorValue       clearBlack = {{0.0f, 0.0f, 0.0f, 1.0f}};
    VkImageSubresourceRange colorRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

    while (!vkb_nativeSurfaceShouldClose(&ns)) {
        vkb_nativeSurfacePollEvents();

        uint32_t imageIndex;
        VkCommandBuffer cmd;
        result = vkb_beginFrame(&base, &ctx, &swapchain, &imageIndex, &cmd);
        if (result != VK_SUCCESS) {
            fprintf(stderr, "Failed to begin frame: %d\n", result);
            break;
        }

        vkb_cmdTransitionImageLayout(cmd, swapchain.images[imageIndex], swapchain.format,
                                     VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        vkCmdClearColorImage(cmd, swapchain.images[imageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                             &clearBlack, 1, &colorRange);
        vkb_cmdTransitionImageLayout(cmd, swapchain.images[imageIndex], swapchain.format,
                                     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        result = vkb_endFrame(&base, &ctx, &swapchain, imageIndex);
        if (result != VK_SUCCESS) {
            fprintf(stderr, "Failed to end frame: %d\n", result);
            break;
        }
    }

    /* Cleanup */
    vkDeviceWaitIdle(base.dev);
    vkb_destroyFrameContext(&base, &ctx);
    vkb_destroySwapchain(&base, &swapchain);
    vkb_destroyBase(&base);
    vkb_destroyNativeSurface(instance, &ns);
    vkDestroyInstance(instance, NULL);
    printf("Done\n");
    return 0;
}
