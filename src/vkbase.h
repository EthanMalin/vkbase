#ifndef VK_BASE_H
#define VK_BASE_H

#include <vulkan/vulkan.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define VK_CHECK(f) {					                                                        \
  {                                                                                   \
    VkResult err = (f);                                                               \
    if (VK_SUCCESS != err)							                                              \
    {								                                                                  \
        printf("VULKAN_ERROR: %d : file(%s) : line(%d)\n", err, __FILE__, __LINE__);	\
        printf("\t%s\n", #f);	                                                        \
        return err;						                                                        \
    }								                                                                  \
  }								                                                                    \
}								                                                                      \


#define VK_CHECK2(f) {					                                                      \
  {                                                                                   \
    VkResult err = (f);                                                               \
    if (VK_SUCCESS > err)							                                                \
    {								                                                                  \
        printf("VULKAN_ERROR: %d : file(%s) : line(%d)\n", err, __FILE__, __LINE__);	\
        printf("\t%s\n", #f);	                                                        \
        return err;						                                                        \
    }								                                                                  \
  }								                                                                    \
}						                                                                          \

#define VKB_MAX_SWAPCHAIN_IMAGES 8
#define VKB_MAX_SWAPCHAIN_SURFACE_FORMATS 128
#define VKB_MAX_SWAPCHAIN_PRESENT_MODES 4

/* --- data types ---  */
typedef struct VkbBuffer {
  VkBuffer buffer;
  VkDeviceMemory memory;
} VkbBuffer;

typedef struct VkbImage {
  VkImage image;
  VkDeviceMemory memory;
} VkbImage;

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
  VkInstance instance;
  VkPhysicalDevice physical;
  VkDevice logical;  
  VkQueue queue;
  VkbQueueFamilyIndices queueFamilyIndices;
} VkbBase;

/* --- variables --- */
#define VKB_NUM_INSTANCE_LAYERS_DEBUG 1
#define VKB_INSTANCE_LAYERS_DEBUG s_instanceLayersDebug
static const char *const s_instanceLayersDebug[] = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef __APPLE__
#define VKB_NUM_INSTANCE_EXTENSIONS_DEBUG 3
#define VKB_INSTANCE_EXTENSIONS_DEBUG s_instanceExtensionsDebug
static const char *const s_instanceExtensionsDebug[] = {
  VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
  "VK_KHR_surface",
  "VK_EXT_metal_surface"
};

#define VKB_NUM_INSTANCE_EXTENSIONS 2
#define VKB_INSTANCE_EXTENSIONS s_instanceExtensions
static const char *const s_instanceExtensions[] = {
  "VK_KHR_surface",
  "VK_EXT_metal_surface"
};
#endif

#ifdef _WIN32
#define VKB_NUM_INSTANCE_EXTENSIONS_DEBUG 2
#define VKB_INSTANCE_EXTENSIONS_DEBUG s_instanceExtensionsDebug
static const char *const s_instanceExtensionsDebug[] = {
  VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
  "VK_KHR_surface",
};

#define VKB_NUM_INSTANCE_EXTENSIONS 1
#define VKB_INSTANCE_EXTENSIONS s_instanceExtensions
static const char *const s_instanceExtensions[] = {
  "VK_KHR_surface",
};
#endif

#define VKB_NUM_DEVICE_EXTENSIONS 1
#define VKB_DEVICE_EXTENSIONS s_deviceExtensions
static const char *const s_deviceExtensions[] = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

/* --- prototypes --- */
/* vkresource.c */
extern VkResult vkb_createBuffer(VkPhysicalDevice pd, VkDevice d, VkBufferCreateInfo *info, VkMemoryPropertyFlags memProps, VkbBuffer *out);
extern VkResult vkb_createImage(VkPhysicalDevice pd, VkDevice d, VkImageCreateInfo *info, VkMemoryPropertyFlags memProps, VkbImage *out);
extern void vkb_destroyBuffer(VkDevice d, VkbBuffer buffer);
extern void vkb_destroyImage(VkDevice d, VkbImage image);

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

/* vkutil.c */
extern size_t readFile(char *filename, char *buffer);

extern int32_t getQueueFamilyIndex(VkPhysicalDevice physicalDevice, VkQueueFlagBits queueSupport);
extern void getQueueFamilyIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkbQueueFamilyIndices *qfi);

extern VkBool32 checkDeviceSupportsExtensions(VkPhysicalDevice physicalDevice);
extern VkBool32 isPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice);
extern VkResult pickPhysicalDevice(VkInstance instance, VkPhysicalDevice *physicalDevice);

extern int32_t findMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

/* vkcmd.c */
extern void cmdTransitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);

/* vkswap.c */
extern VkbSwapchainSupportDetails vkb_swapchainSupportDetails(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface);
extern VkSwapchainCreateInfoKHR vkb_swapchainCreateInfo(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface, VkExtent2D window);
#endif /* VK_BASE_H */
