#include "vkbase.h"

VkInstanceCreateInfo simpleInstance(VkApplicationInfo *pApplicationInfo, VkBool32 debug) {
  VkInstanceCreateInfo infoInstance = *emptyInstance();

  infoInstance.pApplicationInfo = pApplicationInfo;
  if (debug) {
    infoInstance.enabledLayerCount = NUM_INSTANCE_LAYERS_DEBUG;
    infoInstance.ppEnabledLayerNames = s_instanceLayersDebug;
    infoInstance.enabledExtensionCount = NUM_INSTANCE_EXTENSIONS_DEBUG;
    infoInstance.ppEnabledExtensionNames = s_instanceExtensionsDebug;
  } else {
    infoInstance.enabledLayerCount = 0;
    infoInstance.ppEnabledLayerNames = NULL;
    infoInstance.enabledExtensionCount = NUM_INSTANCE_EXTENSIONS;
    infoInstance.ppEnabledExtensionNames = s_instanceExtensions;
  }

  return infoInstance;
}

VkDeviceCreateInfo simpleDevice(uint32_t queueCreateInfoCount, VkDeviceQueueCreateInfo *pQueueCreateInfos) {
  VkDeviceCreateInfo info = *emptyDevice();

  info.queueCreateInfoCount = queueCreateInfoCount;
  info.pQueueCreateInfos = pQueueCreateInfos;
  info.enabledLayerCount = 0;
  info.ppEnabledLayerNames = NULL;
  info.enabledExtensionCount = NUM_DEVICE_EXTENSIONS;
  info.ppEnabledExtensionNames = DEVICE_EXTENSIONS;
  info.pEnabledFeatures = NULL;

  return info;
}

VkShaderModuleCreateInfo simpleShaderModule(size_t codeSize, const uint32_t *pCode) {
  VkShaderModuleCreateInfo info = *emptyShaderModule();

  info.codeSize = codeSize;
  info.pCode = pCode;

  return info;
}


VkPipelineShaderStageCreateInfo simpleShaderStage(VkShaderStageFlagBits stage, VkShaderModule module) {
  VkPipelineShaderStageCreateInfo info = *emptyShaderStage();

  info.stage = stage;
  info.module = module;
  info.pName = "main";

  return info;
}

VkPipelineVertexInputStateCreateInfo simpleVertexInputState(uint32_t bindingCount, const VkVertexInputBindingDescription *pBindings, uint32_t attribCount, const VkVertexInputAttributeDescription *pAttribs) {
  VkPipelineVertexInputStateCreateInfo info = *emptyVertexInputState();

  info.vertexBindingDescriptionCount = bindingCount;
  info.pVertexBindingDescriptions = pBindings;
  info.vertexAttributeDescriptionCount = attribCount;
  info.pVertexAttributeDescriptions = pAttribs;

  return info;
}

VkPipelineInputAssemblyStateCreateInfo simpleInputAssemblyState(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable) {
  VkPipelineInputAssemblyStateCreateInfo info = *emptyInputAssemblyState();

  info.topology = topology;
  info.primitiveRestartEnable = primitiveRestartEnable;

  return info;
}

VkPipelineTessellationStateCreateInfo simpleTessellationState(uint32_t patchControlPoints) {
  VkPipelineTessellationStateCreateInfo info = *emptyTessellationState();

  info.patchControlPoints = patchControlPoints;

  return info;
}

VkPipelineViewportStateCreateInfo simpleViewportState(const VkViewport *pViewport, const VkRect2D *pScissor) {
  VkPipelineViewportStateCreateInfo info = *emptyViewportState();

  info.viewportCount = 1;
  info.pViewports = pViewport;
  info.scissorCount = 1;
  info.pScissors = pScissor;

  return info;
}


VkPipelineRasterizationStateCreateInfo simpleRasterizationState(VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace) {
  VkPipelineRasterizationStateCreateInfo info = *emptyRasterizationState();

  info.polygonMode = polygonMode;
  info.cullMode = cullMode;
  info.frontFace = frontFace;
  info.lineWidth = 1.0f;

  return info;
}

VkPipelineMultisampleStateCreateInfo simpleMultisampleState(VkSampleCountFlagBits samples) {
  VkPipelineMultisampleStateCreateInfo info = *emptyMultisampleState();
  
  info.rasterizationSamples = samples;

  return info;
}

VkPipelineDepthStencilStateCreateInfo simpleDepthStencilState(VkBool32 depthWriteEnable, VkCompareOp depthCompareOp) {
  VkPipelineDepthStencilStateCreateInfo info = *emptyDepthStencilState();

  info.depthTestEnable = VK_TRUE;
  info.depthWriteEnable = depthWriteEnable;
  info.depthCompareOp = depthCompareOp;

  return info;
}

VkPipelineColorBlendStateCreateInfo simpleColorBlendState(const VkPipelineColorBlendAttachmentState *pAttachment, float *blendConstants) {
  VkPipelineColorBlendStateCreateInfo info = *emptyColorBlendState();

  info.pAttachments = pAttachment;
  for (int i = 0; i < 4; i++) info.blendConstants[i] = blendConstants[i];

  return info;
}

VkPipelineDynamicStateCreateInfo simpleDynamicState(uint32_t dynamicStateCount, const VkDynamicState *pDynamicStates) {
  VkPipelineDynamicStateCreateInfo info = *emptyDynamicState();

  info.dynamicStateCount = dynamicStateCount;
  info.pDynamicStates = pDynamicStates;

  return info;
}

