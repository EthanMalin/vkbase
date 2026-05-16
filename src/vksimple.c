#include "vkbase.h"

#define VKB_NUM_INSTANCE_LAYERS_DEBUG 1
#define VKB_INSTANCE_LAYERS_DEBUG s_instanceLayersDebug
static const char *const s_instanceLayersDebug[] = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef __APPLE__
#ifdef VK_KHR_portability_enumeration
#define VKB_NUM_INSTANCE_EXTENSIONS_DEBUG 4
#define VKB_INSTANCE_EXTENSIONS_DEBUG s_instanceExtensionsDebug
static const char *const s_instanceExtensionsDebug[] = {
  VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
  "VK_KHR_surface",
  "VK_EXT_metal_surface",
  VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
};

#define VKB_NUM_INSTANCE_EXTENSIONS 3
#define VKB_INSTANCE_EXTENSIONS s_instanceExtensions
static const char *const s_instanceExtensions[] = {
  "VK_KHR_surface",
  "VK_EXT_metal_surface",
  VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
};
#else
#define VKB_NUM_INSTANCE_EXTENSIONS_DEBUG 3
#define VKB_INSTANCE_EXTENSIONS_DEBUG s_instanceExtensionsDebug
static const char *const s_instanceExtensionsDebug[] = {
  VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
  "VK_KHR_surface",
  "VK_EXT_metal_surface",
};

#define VKB_NUM_INSTANCE_EXTENSIONS 2
#define VKB_INSTANCE_EXTENSIONS s_instanceExtensions
static const char *const s_instanceExtensions[] = {
  "VK_KHR_surface",
  "VK_EXT_metal_surface",
};
#endif /* VK_KHR_portability_enumeration */
#endif /* __APPLE__ */

#ifdef _WIN32
#define VKB_NUM_INSTANCE_EXTENSIONS_DEBUG 3
#define VKB_INSTANCE_EXTENSIONS_DEBUG s_instanceExtensionsDebug
static const char *const s_instanceExtensionsDebug[] = {
  VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
  "VK_KHR_surface",
  "VK_KHR_win32_surface",
};

#define VKB_NUM_INSTANCE_EXTENSIONS 2
#define VKB_INSTANCE_EXTENSIONS s_instanceExtensions
static const char *const s_instanceExtensions[] = {
  "VK_KHR_surface",
  "VK_KHR_win32_surface",
};
#endif

#if defined(__APPLE__) && defined(VK_KHR_portability_subset)
#define VKB_NUM_DEVICE_EXTENSIONS 2
static const char *const s_deviceExtensions[] = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME,
  VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME,
};
#else
#define VKB_NUM_DEVICE_EXTENSIONS 1
static const char *const s_deviceExtensions[] = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};
#endif
#define VKB_DEVICE_EXTENSIONS s_deviceExtensions

VkApplicationInfo vkb_simpleApplication(const char *pApplicationName, uint32_t applicationVersion, uint32_t apiVersion) {
  VkApplicationInfo info = *vkb_emptyApplication();
  info.pApplicationName = pApplicationName;
  info.applicationVersion = applicationVersion;
  info.apiVersion = apiVersion;
  return info;
}

VkInstanceCreateInfo vkb_simpleInstance(VkApplicationInfo *pApplicationInfo, VkBool32 debug) {
  VkInstanceCreateInfo infoInstance =  *vkb_emptyInstance();

#if defined(__APPLE__) && defined(VK_KHR_portability_enumeration)
  infoInstance.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

  infoInstance.pApplicationInfo = pApplicationInfo;
  if (debug) {
    infoInstance.enabledLayerCount = VKB_NUM_INSTANCE_LAYERS_DEBUG;
    infoInstance.ppEnabledLayerNames = s_instanceLayersDebug;
    infoInstance.enabledExtensionCount = VKB_NUM_INSTANCE_EXTENSIONS_DEBUG;
    infoInstance.ppEnabledExtensionNames = s_instanceExtensionsDebug;
  } else {
    infoInstance.enabledLayerCount = 0;
    infoInstance.ppEnabledLayerNames = NULL;
    infoInstance.enabledExtensionCount = VKB_NUM_INSTANCE_EXTENSIONS;
    infoInstance.ppEnabledExtensionNames = s_instanceExtensions;
  }

  return infoInstance;
}

VkDeviceCreateInfo vkb_simpleDevice(uint32_t queueCreateInfoCount, VkDeviceQueueCreateInfo *pQueueCreateInfos) {
  VkDeviceCreateInfo info =  *vkb_emptyDevice();

  info.queueCreateInfoCount = queueCreateInfoCount;
  info.pQueueCreateInfos = pQueueCreateInfos;
  info.enabledLayerCount = 0;
  info.ppEnabledLayerNames = NULL;
  info.enabledExtensionCount = VKB_NUM_DEVICE_EXTENSIONS;
  info.ppEnabledExtensionNames = VKB_DEVICE_EXTENSIONS;
  info.pEnabledFeatures = NULL;

  return info;
}

VkShaderModuleCreateInfo vkb_simpleShaderModule(size_t codeSize, const uint32_t *pCode) {
  VkShaderModuleCreateInfo info =  *vkb_emptyShaderModule();

  info.codeSize = codeSize;
  info.pCode = pCode;

  return info;
}


VkPipelineShaderStageCreateInfo vkb_simpleShaderStage(VkShaderStageFlagBits stage, VkShaderModule module) {
  VkPipelineShaderStageCreateInfo info =  *vkb_emptyShaderStage();

  info.stage = stage;
  info.module = module;
  info.pName = "main";

  return info;
}

VkPipelineVertexInputStateCreateInfo vkb_simpleVertexInputState(uint32_t bindingCount, const VkVertexInputBindingDescription *pBindings, uint32_t attribCount, const VkVertexInputAttributeDescription *pAttribs) {
  VkPipelineVertexInputStateCreateInfo info =  *vkb_emptyVertexInputState();
  info.vertexBindingDescriptionCount = bindingCount;
  info.pVertexBindingDescriptions = pBindings;
  info.vertexAttributeDescriptionCount = attribCount;
  info.pVertexAttributeDescriptions = pAttribs;
  return info;
}

VkPipelineInputAssemblyStateCreateInfo vkb_simpleInputAssemblyState(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable) {
  VkPipelineInputAssemblyStateCreateInfo info =  *vkb_emptyInputAssemblyState();
  info.topology = topology;
  info.primitiveRestartEnable = primitiveRestartEnable;
  return info;
}

VkPipelineTessellationStateCreateInfo vkb_simpleTessellationState(uint32_t patchControlPoints) {
  VkPipelineTessellationStateCreateInfo info =  *vkb_emptyTessellationState();
  info.patchControlPoints = patchControlPoints;
  return info;
}

VkPipelineViewportStateCreateInfo vkb_simpleViewportState(const VkViewport *pViewport, const VkRect2D *pScissor) {
  VkPipelineViewportStateCreateInfo info =  *vkb_emptyViewportState();
  info.viewportCount = 1;
  info.pViewports = pViewport;
  info.scissorCount = 1;
  info.pScissors = pScissor;
  return info;
}

VkPipelineRasterizationStateCreateInfo vkb_simpleRasterizationState(VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace) {
  VkPipelineRasterizationStateCreateInfo info =  *vkb_emptyRasterizationState();
  info.polygonMode = polygonMode;
  info.cullMode = cullMode;
  info.frontFace = frontFace;
  info.lineWidth = 1.0f;
  return info;
}

VkPipelineMultisampleStateCreateInfo vkb_simpleMultisampleState(VkSampleCountFlagBits samples) {
  VkPipelineMultisampleStateCreateInfo info =  *vkb_emptyMultisampleState();
  info.rasterizationSamples = samples;
  return info;
}

VkPipelineDepthStencilStateCreateInfo vkb_simpleDepthStencilState(VkBool32 depthWriteEnable, VkCompareOp depthCompareOp) {
  VkPipelineDepthStencilStateCreateInfo info =  *vkb_emptyDepthStencilState();
  info.depthTestEnable = VK_TRUE;
  info.depthWriteEnable = depthWriteEnable;
  info.depthCompareOp = depthCompareOp;
  return info;
}

VkPipelineColorBlendStateCreateInfo vkb_simpleColorBlendState(const VkPipelineColorBlendAttachmentState *pAttachment, float *blendConstants) {
  VkPipelineColorBlendStateCreateInfo info =  *vkb_emptyColorBlendState();
  info.pAttachments = pAttachment;
  for (int i = 0; i < 4; i++) info.blendConstants[i] = blendConstants[i];
  return info;
}

VkPipelineDynamicStateCreateInfo vkb_simpleDynamicState(uint32_t dynamicStateCount, const VkDynamicState *pDynamicStates) {
  VkPipelineDynamicStateCreateInfo info =  *vkb_emptyDynamicState();
  info.dynamicStateCount = dynamicStateCount;
  info.pDynamicStates = pDynamicStates;
  return info;
}

VkRenderPassCreateInfo vkb_simpleRenderPass(uint32_t attachmentCount, VkAttachmentDescription const *pAttachments, uint32_t subpassCount, VkSubpassDescription const *pSubpasses, uint32_t dependencyCount, VkSubpassDependency const *pDependencies) {
  VkRenderPassCreateInfo info =  *vkb_emptyRenderPass();
  info.attachmentCount = attachmentCount;
  info.pAttachments = pAttachments;
  info.subpassCount = subpassCount;
  info.pSubpasses = pSubpasses;
  info.dependencyCount = dependencyCount;
  info.pDependencies = pDependencies;
  return info;
}

VkFramebufferCreateInfo vkb_simpleFramebuffer(VkRenderPass renderPass, uint32_t attachmentCount, const VkImageView *pAttachments, uint32_t width, uint32_t height, uint32_t layers) {
  VkFramebufferCreateInfo info =  *vkb_emptyFramebuffer();
  info.renderPass = renderPass;
  info.attachmentCount = attachmentCount;
  info.pAttachments = pAttachments;
  info.width = width;
  info.height = height;
  info.layers = layers;
  return info;
}

