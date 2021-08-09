#ifndef VK_BASE_H
#define VK_BASE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define VK_CHECK(f) {					          \
  VkResult err = (f);						        \
  if (err)							                \
    {								                    \
      printf("VULKAN_ERROR: %d", err);	\
      return err;						            \
    }								                    \
  }								                      \

#define MAX_SWAPCHAIN_IMAGES 8
#define MAX_SWAPCHAIN_SURFACE_FORMATS 128
#define MAX_SWAPCHAIN_PRESENT_MODES 4

/* --- data types ---  */
typedef struct Buffer {
  VkBuffer buffer;
  VkDeviceMemory memory;
  void *ptr;
} Buffer;

typedef struct Image {
  VkImage image;
  VkDeviceMemory memory;
} Image;

typedef struct QueueFamilyIndices {
  uint32_t graphicsIndex;
  uint32_t computeIndex;
  uint32_t presentIndex;
  VkBool32 hasGraphics;
  VkBool32 hasCompute;
  VkBool32 hasPresent;
} QueueFamilyIndices;

typedef struct SwapchainSupportDetails {
  uint32_t numFormats;
  uint32_t numPresentModes;
  VkBool32 presentSupportedOnQueue;
  VkSurfaceCapabilitiesKHR capabilities;
  VkSurfaceFormatKHR formats[MAX_SWAPCHAIN_SURFACE_FORMATS];
  VkPresentModeKHR presentModes[MAX_SWAPCHAIN_PRESENT_MODES];
} SwapchainSupportDetails;

typedef struct Swapchain {
  VkSwapchainKHR chain;
  uint32_t imageCount;
  VkFormat format;
  VkExtent2D extent;
  VkImage images[MAX_SWAPCHAIN_IMAGES];
  VkImageView views[MAX_SWAPCHAIN_IMAGES];
} Swapchain;

typedef struct VkBase {
  VkInstance instance;
  VkPhysicalDevice physical;
  VkDevice logical;  
  VkQueue queueGraphics;
  QueueFamilyIndices queueFamilyIndices;
} VkBase;

/* --- variables --- */
#define NUM_INSTANCE_LAYERS_DEBUG 1
#define INSTANCE_LAYERS_DEBUG s_instanceLayersDebug
static const char *const s_instanceLayersDebug[] = {
    "VK_LAYER_KHRONOS_validation"
};

#define NUM_INSTANCE_EXTENSIONS_DEBUG 3
#define INSTANCE_EXTENSIONS_DEBUG s_instanceExtensionsDebug
static const char *const s_instanceExtensionsDebug[] = {
  VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
  "VK_KHR_surface",
  "VK_EXT_metal_surface"
};

#define NUM_INSTANCE_EXTENSIONS 2
#define INSTANCE_EXTENSIONS s_instanceExtensions
static const char *const s_instanceExtensions[] = {
  "VK_KHR_surface",
  "VK_EXT_metal_surface"
};

#define NUM_DEVICE_EXTENSIONS 1
#define DEVICE_EXTENSIONS s_deviceExtensions
static const char *const s_deviceExtensions[] = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

/* --- prototypes --- */
/* vkempties.c */
extern const VkInstanceCreateInfo *const emptyInstance();
extern const VkApplicationInfo *const emptyApplication();
extern const VkDeviceQueueCreateInfo *const emptyDeviceQueue();
extern const VkDeviceCreateInfo *const emptyDevice();
extern const VkCommandPoolCreateInfo *const emptyCommandPool();
extern const VkCommandBufferAllocateInfo *const emptyCommandBuffer();
extern const VkSubmitInfo *const emptySubmit();
extern const VkFenceCreateInfo *const emptyFence();
extern const VkSemaphoreCreateInfo *const emptySemaphore();

extern const VkRenderPassCreateInfo *const emptyRenderPass();

extern const VkMemoryAllocateInfo *const emptyMemory();
extern const VkBufferCreateInfo *const emptyBuffer();
extern const VkBufferViewCreateInfo *const emptyBufferView();
extern const VkImageCreateInfo *const emptyImage();
extern const VkImageViewCreateInfo *const emptyImageView();

extern const VkDescriptorSetLayoutCreateInfo *const emptyDescriptorSetLayout();
extern const VkPipelineLayoutCreateInfo *const emptyPipelineLayout();
extern const VkDescriptorPoolCreateInfo *const emptyDescriptorPool();
extern const VkDescriptorSetAllocateInfo *const emptyDescriptorSet();

extern const VkShaderModuleCreateInfo *const emptyShaderModule();
extern const VkPipelineShaderStageCreateInfo *const emptyShaderStage();
extern const VkPipelineVertexInputStateCreateInfo *const emptyVertexInputState();
extern const VkPipelineInputAssemblyStateCreateInfo *const emptyInputAssemblyState();
extern const VkPipelineTessellationStateCreateInfo *const emptyTessellationState();
extern const VkPipelineViewportStateCreateInfo *const emptyViewportState();
extern const VkPipelineRasterizationStateCreateInfo *const emptyRasterizationState();
extern const VkPipelineMultisampleStateCreateInfo *const emptyMultisampleState();
extern const VkPipelineDepthStencilStateCreateInfo *const emptyDepthStencilState();
extern const VkPipelineColorBlendStateCreateInfo *const emptyColorBlendState();
extern const VkPipelineDynamicStateCreateInfo *const emptyDynamicState();
extern const VkGraphicsPipelineCreateInfo *const emptyGraphicsPipeline();

extern const VkSwapchainCreateInfoKHR *const emptySwapchain();
extern const VkPresentInfoKHR *const emptyPresent();

extern const VkWriteDescriptorSet *const emptyWriteDescriptorSet();
extern const VkCopyDescriptorSet *const emptyCopyDescriptorSet();

/* vksimple.c */
extern VkInstanceCreateInfo simpleInstance(VkApplicationInfo *pApplicationInfo, VkBool32 debug);
extern VkDeviceCreateInfo simpleDevice(uint32_t queueCreateInfoCount, VkDeviceQueueCreateInfo *pQueueCreateInfos);
extern VkShaderModuleCreateInfo simpleShaderModule(size_t codeSize, const uint32_t *pCode);
extern VkPipelineShaderStageCreateInfo simpleShaderStage(VkShaderStageFlagBits stage, VkShaderModule module);
extern VkPipelineVertexInputStateCreateInfo simpleVertexInputState(uint32_t bindingCount, const VkVertexInputBindingDescription *pBindings, uint32_t attribCount, const VkVertexInputAttributeDescription *pAttribs);
extern VkPipelineInputAssemblyStateCreateInfo simpleInputAssemblyState(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable);
extern VkPipelineTessellationStateCreateInfo simpleTessellationState(uint32_t patchControlPoints);
extern VkPipelineViewportStateCreateInfo simpleViewportState(const VkViewport *pViewport, const VkRect2D *pScissor); 
extern VkPipelineRasterizationStateCreateInfo simpleRasterizationState(VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace);
extern VkPipelineMultisampleStateCreateInfo simpleMultisampleState(VkSampleCountFlagBits sample);
extern VkPipelineDepthStencilStateCreateInfo simpleDepthStencilState(VkBool32 depthWriteEnable, VkCompareOp depthCompareOp);
extern VkPipelineColorBlendStateCreateInfo simpleColorBlendState(const VkPipelineColorBlendAttachmentState *pAttachment, float *blendConstants);
extern VkPipelineDynamicStateCreateInfo simpleDynamicState(uint32_t dynamicStateCount, const VkDynamicState *pDynamicStates);

/* vkutil.c */
extern size_t readFile(char *filename, char *buffer);

extern int32_t getQueueFamilyIndex(VkPhysicalDevice physicalDevice, VkQueueFlagBits queueSupport);
extern void getQueueFamilyIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, QueueFamilyIndices *qfi);

extern VkBool32 checkDeviceSupportsExtensions(VkPhysicalDevice physicalDevice);
extern VkBool32 isPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice);
extern VkResult pickPhysicalDevice(VkInstance instance, VkPhysicalDevice *physicalDevice);

extern int32_t findMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

/* vkswap.c */
extern VkSwapchainCreateInfoKHR swapchainInfo(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface, GLFWwindow *window);
extern SwapchainSupportDetails querySwapchainSupportDetails(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface);

/* vkcmd.c */
extern void cmdTransitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);

#endif /* VK_BASE_H */
