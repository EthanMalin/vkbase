#ifndef VK_BASE_H
#define VK_BASE_H

/* Includes */
#include <vulkan/vulkan.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/* Macros */

/* VK_CHECK can wrap a Vulkan function call and check if the return was success.
    If return was not success, it prints an error message and returns the VkResult.
    This macro can only be used in functions which themselves return VkResult. */
#define VK_CHECK(f) {					                                                        \
  {                                                                                   \
    VkResult err = (f);                                                               \
    if (VK_SUCCESS != err)							                                              \
    {								                                                                  \
        fprintf(stderr, "VULKAN_ERROR: %d : file(%s) : line(%d)\n", err, __FILE__, __LINE__);	\
        fprintf(stderr, "\t%s\n", #f);	                                              \
        return err;						                                                        \
    }								                                                                  \
  }								                                                                    \
}								                                                                      \

/* VK_CHECK2 is the same as VK_CHECK but checks greater than or equal to VK_SUCCESS,
    which allows for wider range of success (i.e. VK_INCOMPLETE). */
#define VK_CHECK2(f) {					                                                      \
  {                                                                                   \
    VkResult err = (f);                                                               \
    if (VK_SUCCESS > err)							                                                \
    {								                                                                  \
        fprintf(stderr, "VULKAN_ERROR: %d : file(%s) : line(%d)\n", err, __FILE__, __LINE__);	\
        fprintf(stderr, "\t%s\n", #f);	                                              \
        return err;						                                                        \
    }								                                                                  \
  }								                                                                    \
}						                                                                          \

#define VKB_MAX_SWAPCHAIN_IMAGES 8
#define VKB_MAX_SWAPCHAIN_SURFACE_FORMATS 128
#define VKB_MAX_SWAPCHAIN_PRESENT_MODES 4
#define VKB_MAX_FRAMES_IN_FLIGHT 3

typedef struct VkbNativeSurface VkbNativeSurface;

/* --- Data types ---  */
typedef struct VkbBuffer {
  VkBuffer buffer;
  VkDeviceMemory memory;
} VkbBuffer;

typedef struct VkbImage {
  VkImage image;
  VkDeviceMemory memory;
} VkbImage;

typedef struct VkbShaderImage {
  VkbImage image;
  VkImageView view;
} VkbShaderImage;

typedef struct VkbTexture {
  VkbShaderImage image;
  VkSampler      sampler;
} VkbTexture;

typedef struct VkbQueueFamilyIndices {
  uint32_t graphicsIndex;
  uint32_t computeIndex;
  uint32_t presentIndex;
  VkBool32 hasGraphics;
  VkBool32 hasCompute;
  VkBool32 hasPresent;
} VkbQueueFamilyIndices;

typedef struct VkbSwapchainSupportDetails {
  uint32_t numFormats;
  uint32_t numPresentModes;
  VkBool32 presentSupportedOnQueue;
  VkSurfaceCapabilitiesKHR capabilities;
  VkSurfaceFormatKHR formats[VKB_MAX_SWAPCHAIN_SURFACE_FORMATS];
  VkPresentModeKHR presentModes[VKB_MAX_SWAPCHAIN_PRESENT_MODES];
} VkbSwapchainSupportDetails;

typedef struct VkbSwapchain {
  VkSwapchainKHR chain;
  uint32_t imageCount;
  VkFormat format;
  VkExtent2D extent;
  VkImage images[VKB_MAX_SWAPCHAIN_IMAGES];
  VkImageView views[VKB_MAX_SWAPCHAIN_IMAGES];
} VkbSwapchain;

typedef struct VkbBase {
  VkInstance inst;
  VkPhysicalDevice pdev;
  VkDevice dev;
  VkQueue gq; // graphics queue
  VkQueue pq; // present queue
  VkbQueueFamilyIndices qfi;
} VkbBase;

typedef struct VkbFrameContext {
  VkCommandPool   cmdPool;
  uint32_t        frameCount;
  uint32_t        currentFrame;
  VkCommandBuffer cmdBuffers[VKB_MAX_FRAMES_IN_FLIGHT];
  VkSemaphore     imageAvailable[VKB_MAX_FRAMES_IN_FLIGHT];
  VkSemaphore     renderFinished[VKB_MAX_FRAMES_IN_FLIGHT];
  VkFence         inFlight[VKB_MAX_FRAMES_IN_FLIGHT];
} VkbFrameContext;


/* --- prototypes --- */
/* vkresource.c */
extern VkResult vkb_createBuffer(VkPhysicalDevice pd, VkDevice d, VkBufferCreateInfo *info, VkMemoryPropertyFlags memProps, VkbBuffer *out);
extern VkResult vkb_createImage(VkPhysicalDevice pd, VkDevice d, VkImageCreateInfo *info, VkMemoryPropertyFlags memProps, VkbImage *out);
extern VkResult vkb_createShaderImage(VkPhysicalDevice pd, VkDevice d, VkImageCreateInfo *info, VkMemoryPropertyFlags memProps, VkImageAspectFlags aspect, VkbShaderImage *out);
extern void vkb_destroyBuffer(VkDevice d, VkbBuffer buffer);
extern void vkb_destroyImage(VkDevice d, VkbImage image);
extern void vkb_destroyShaderImage(VkDevice d, VkbShaderImage image);
extern VkResult vkb_createTexture(VkPhysicalDevice pd, VkDevice d, VkImageCreateInfo *imgInfo, VkMemoryPropertyFlags memProps, VkImageAspectFlags aspect, VkSamplerCreateInfo *samplerInfo, VkbTexture *out);
extern void vkb_destroyTexture(VkDevice d, VkbTexture texture);

/* vkempties.c */
extern const VkInstanceCreateInfo *const vkb_emptyInstance();
extern const VkApplicationInfo *const vkb_emptyApplication();
extern const VkDeviceQueueCreateInfo *const vkb_emptyDeviceQueue();
extern const VkDeviceCreateInfo *const vkb_emptyDevice();
extern const VkCommandPoolCreateInfo *const vkb_emptyCommandPool();
extern const VkCommandBufferAllocateInfo *const vkb_emptyCommandBuffer();
extern const VkSubmitInfo *const vkb_emptySubmit();
extern const VkFenceCreateInfo *const vkb_emptyFence();
extern const VkSemaphoreCreateInfo *const vkb_emptySemaphore();

extern const VkRenderPassCreateInfo *const vkb_emptyRenderPass();
extern const VkFramebufferCreateInfo *const vkb_emptyFramebuffer();

extern const VkMemoryAllocateInfo *const vkb_emptyMemory();
extern const VkBufferCreateInfo *const vkb_emptyBuffer();
extern const VkBufferViewCreateInfo *const vkb_emptyBufferView();
extern const VkImageCreateInfo *const vkb_emptyImage();
extern const VkImageViewCreateInfo *const vkb_emptyImageView();
extern const VkSamplerCreateInfo *const vkb_emptySampler();

extern const VkDescriptorSetLayoutCreateInfo *const vkb_emptyDescriptorSetLayout();
extern const VkPipelineLayoutCreateInfo *const vkb_emptyPipelineLayout();
extern const VkDescriptorPoolCreateInfo *const vkb_emptyDescriptorPool();
extern const VkDescriptorSetAllocateInfo *const vkb_emptyDescriptorSet();

extern const VkShaderModuleCreateInfo *const vkb_emptyShaderModule();
extern const VkPipelineShaderStageCreateInfo *const vkb_emptyShaderStage();
extern const VkPipelineVertexInputStateCreateInfo *const vkb_emptyVertexInputState();
extern const VkPipelineInputAssemblyStateCreateInfo *const vkb_emptyInputAssemblyState();
extern const VkPipelineTessellationStateCreateInfo *const vkb_emptyTessellationState();
extern const VkPipelineViewportStateCreateInfo *const vkb_emptyViewportState();
extern const VkPipelineRasterizationStateCreateInfo *const vkb_emptyRasterizationState();
extern const VkPipelineMultisampleStateCreateInfo *const vkb_emptyMultisampleState();
extern const VkPipelineDepthStencilStateCreateInfo *const vkb_emptyDepthStencilState();
extern const VkPipelineColorBlendStateCreateInfo *const vkb_emptyColorBlendState();
extern const VkPipelineDynamicStateCreateInfo *const vkb_emptyDynamicState();
extern const VkGraphicsPipelineCreateInfo *const vkb_emptyGraphicsPipeline();

extern const VkSwapchainCreateInfoKHR *const vkb_emptySwapchain();
extern const VkPresentInfoKHR *const vkb_emptyPresent();

extern const VkWriteDescriptorSet *const vkb_emptyWriteDescriptorSet();
extern const VkCopyDescriptorSet *const vkb_emptyCopyDescriptorSet();

extern const VkCommandBufferBeginInfo *const vkb_emptyCommandBufferBegin();
extern const VkRenderPassBeginInfo *const vkb_emptyRenderPassBegin();

/* vksimple.c */
extern VkApplicationInfo vkb_simpleApplication(const char *pApplicationName, uint32_t applicationVersion, uint32_t apiVersion);
extern VkInstanceCreateInfo vkb_simpleInstance(VkApplicationInfo *pApplicationInfo, VkBool32 debug);
extern VkDeviceCreateInfo vkb_simpleDevice(uint32_t queueCreateInfoCount, VkDeviceQueueCreateInfo *pQueueCreateInfos);
extern VkShaderModuleCreateInfo vkb_simpleShaderModule(size_t codeSize, const uint32_t *pCode);
extern VkPipelineShaderStageCreateInfo vkb_simpleShaderStage(VkShaderStageFlagBits stage, VkShaderModule module);
extern VkPipelineVertexInputStateCreateInfo vkb_simpleVertexInputState(uint32_t bindingCount, const VkVertexInputBindingDescription *pBindings, uint32_t attribCount, const VkVertexInputAttributeDescription *pAttribs);
extern VkPipelineInputAssemblyStateCreateInfo vkb_simpleInputAssemblyState(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable);
extern VkPipelineTessellationStateCreateInfo vkb_simpleTessellationState(uint32_t patchControlPoints);
extern VkPipelineViewportStateCreateInfo vkb_simpleViewportState(const VkViewport *pViewport, const VkRect2D *pScissor); 
extern VkPipelineRasterizationStateCreateInfo vkb_simpleRasterizationState(VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace);
extern VkPipelineMultisampleStateCreateInfo vkb_simpleMultisampleState(VkSampleCountFlagBits sample);
extern VkPipelineDepthStencilStateCreateInfo vkb_simpleDepthStencilState(VkBool32 depthWriteEnable, VkCompareOp depthCompareOp);
extern VkPipelineColorBlendStateCreateInfo vkb_simpleColorBlendState(const VkPipelineColorBlendAttachmentState *pAttachment, float *blendConstants);
extern VkPipelineDynamicStateCreateInfo vkb_simpleDynamicState(uint32_t dynamicStateCount, const VkDynamicState *pDynamicStates);
extern VkRenderPassCreateInfo vkb_simpleRenderPass(uint32_t attachmentCount, VkAttachmentDescription const *pAttachments, uint32_t subpassCount, VkSubpassDescription const *pSubpasses, uint32_t dependencyCount, VkSubpassDependency const *pDependencies);
extern VkFramebufferCreateInfo vkb_simpleFramebuffer(VkRenderPass renderPass, uint32_t attachmentCount, const VkImageView *pAttachments, uint32_t width, uint32_t height, uint32_t layers);
extern VkImageCreateInfo vkb_simpleImage2D(VkFormat format, VkExtent2D extent, VkImageUsageFlags usage);
extern VkSamplerCreateInfo vkb_simpleSampler(VkFilter filter, VkSamplerAddressMode addressMode);
extern VkAttachmentDescription vkb_simpleColorAttachment(VkFormat format, VkImageLayout finalLayout);
extern VkAttachmentDescription vkb_simpleDepthAttachment(VkFormat format);
extern VkAttachmentReference vkb_colorAttachmentRef(uint32_t index);
extern VkAttachmentReference vkb_depthAttachmentRef(uint32_t index);
extern VkSubpassDescription vkb_simpleGraphicsSubpass(const VkAttachmentReference *pColorRef, const VkAttachmentReference *pDepthRef);
extern VkSubpassDependency vkb_simpleExternalDependency(void);

/* vkutil.c */
extern size_t vkb_readFile(char *filename, char **buffer);

extern int32_t vkb_getQueueFamilyIndex(VkPhysicalDevice physicalDevice, VkQueueFlagBits queueSupport);
extern void vkb_getQueueFamilyIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkbQueueFamilyIndices *qfi);

extern VkBool32 vkb_checkDeviceSupportsExtensions(VkPhysicalDevice physicalDevice);
extern VkBool32 vkb_isPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice);
extern VkResult vkb_pickPhysicalDevice(VkInstance instance, VkPhysicalDevice *physicalDevice);

extern int32_t vkb_findMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
extern VkResult vkb_createBase(VkInstance instance, VkSurfaceKHR surface, VkPhysicalDevice *pdev, VkbBase *out);
extern void     vkb_destroyBase(VkbBase *base);

/* vkcmd.c */
extern void vkb_cmdTransitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

/* vkswap.c */
extern VkbSwapchainSupportDetails vkb_swapchainSupportDetails(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface);
extern VkSwapchainCreateInfoKHR vkb_swapchainCreateInfo(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface, VkExtent2D window);
extern VkResult vkb_createSwapchain(VkbBase *base, VkbNativeSurface *ns, VkFormat *requiredFormat, VkbSwapchain *out);
extern void     vkb_destroySwapchain(VkbBase *base, VkbSwapchain *swapchain);

/* vkframe.c */
extern VkResult vkb_createFrameContext(VkbBase *base, uint32_t frameCount, VkbFrameContext *out);
extern void     vkb_destroyFrameContext(VkbBase *base, VkbFrameContext *ctx);
extern VkResult vkb_beginFrame(VkbBase *base, VkbFrameContext *ctx, VkbSwapchain *swapchain, uint32_t *imageIndex, VkCommandBuffer *cmd);
extern VkResult vkb_endFrame(VkbBase *base, VkbFrameContext *ctx, VkbSwapchain *swapchain, uint32_t imageIndex);

/* vkformat.c */
extern bool vkb_isDepthFormat(VkFormat format);
#endif /* VK_BASE_H */
