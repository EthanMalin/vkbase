#include "vkbase.h"

/* instance */
static const VkApplicationInfo s_emptyApplication = {
  VK_STRUCTURE_TYPE_APPLICATION_INFO, NULL,
  "", 0, "", 0, 0 
};

static const VkInstanceCreateInfo s_emptyInstance = {
  VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, NULL, 0,
  NULL, 0, NULL, 0, NULL
};

static const VkDeviceQueueCreateInfo s_emptyDeviceQueue = {
  VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, NULL, 0,
  0, 0, NULL
};

static const VkDeviceCreateInfo s_emptyDevice = {
  VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, NULL, 0,
  0, NULL, 0, NULL, 0, NULL, NULL
};

/* command buffer */
static const VkCommandPoolCreateInfo s_emptyCommandPool = {
  VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, NULL, 0, 0
};

static const VkCommandBufferAllocateInfo s_emptyCommandBuffer = {
  VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, NULL,
  VK_NULL_HANDLE, VK_COMMAND_BUFFER_LEVEL_MAX_ENUM, 0
};

static const VkSubmitInfo s_emptySubmit = { VK_STRUCTURE_TYPE_SUBMIT_INFO };

/* synchronization */
static const VkSemaphoreCreateInfo s_emptySemaphore = {
  VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, NULL, 0
};

static const VkFenceCreateInfo s_emptyFence = {
  VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, NULL, 0
};

static const VkRenderPassCreateInfo s_emptyRenderPass = {
  VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO, NULL, 0,
  0, NULL, 0, NULL, 0, NULL
};

/* resources */
static const VkMemoryAllocateInfo s_emptyMemory = {
  VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, NULL, 0, 0
};

static const VkBufferCreateInfo s_emptyBuffer = {
  VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO, NULL, 0,
  0, VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM, VK_SHARING_MODE_MAX_ENUM,
  0, NULL
};

static const VkBufferViewCreateInfo s_emptyBufferView = {
  VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO, NULL, 0,
  VK_NULL_HANDLE, VK_FORMAT_UNDEFINED, 0, 0
};

static const VkImageCreateInfo s_emptyImage = {
  VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO, NULL, 0,
  VK_IMAGE_TYPE_MAX_ENUM, VK_FORMAT_UNDEFINED, {0},
  0, 0, VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM, VK_IMAGE_TILING_MAX_ENUM,
  VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM, VK_SHARING_MODE_MAX_ENUM,
  0, NULL, VK_IMAGE_LAYOUT_UNDEFINED
};

static const VkImageViewCreateInfo s_emptyImageView = {
  VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, NULL, 0,
  VK_NULL_HANDLE, VK_IMAGE_VIEW_TYPE_MAX_ENUM, VK_FORMAT_UNDEFINED,
  {0}, {0}
};

/* pipelines */
static const VkDescriptorSetLayoutCreateInfo s_emptyDescriptorSetLayout = {
  VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO, NULL, 0, 0, NULL
};

static const VkPipelineLayoutCreateInfo s_emptyPipelineLayout = {
  VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO, NULL, 0, 0, NULL, 0, NULL
};

static const VkDescriptorPoolCreateInfo s_emptyDescriptorPool = {
  VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO, NULL, 0, 0, 0, NULL
};

static const VkDescriptorSetAllocateInfo s_emptyDescriptorSet = {
  VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, NULL, VK_NULL_HANDLE, 0, NULL
};

static const VkShaderModuleCreateInfo s_emptyShaderModule = {
  VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, NULL, 0, 0, NULL
};

static const VkPipelineShaderStageCreateInfo s_emptyShaderStage = {
  VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, NULL, 0, 0, VK_NULL_HANDLE, NULL, NULL
};

static const VkPipelineVertexInputStateCreateInfo s_emptyVertexInputState = {
  VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO, NULL, 0,
  0, NULL, 0, NULL
};

static const VkPipelineInputAssemblyStateCreateInfo s_emptyInputAssemblyState = {
  VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO, NULL, 0,
  VK_PRIMITIVE_TOPOLOGY_MAX_ENUM, VK_FALSE
};

static const VkPipelineTessellationStateCreateInfo s_emptyTessellationState = {
  VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO, NULL, 0, 0
};

static const VkPipelineViewportStateCreateInfo s_emptyViewportState = {
  VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO, NULL, 0,
  0, NULL, 0, NULL
};

static const VkPipelineRasterizationStateCreateInfo s_emptyRasterizationState = {
  VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO, NULL, 0,
  VK_FALSE, VK_FALSE, VK_POLYGON_MODE_MAX_ENUM, VK_CULL_MODE_FLAG_BITS_MAX_ENUM, VK_FRONT_FACE_MAX_ENUM,
  VK_FALSE, 0.0f, 0.0f, 0.0f, 0.0f
};

static const VkPipelineMultisampleStateCreateInfo s_emptyMultisampleState = {
  VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, NULL, 0,
  VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM, VK_FALSE, 0.0f, NULL, VK_FALSE, VK_FALSE
};

static const VkPipelineDepthStencilStateCreateInfo s_emptyDepthStencilState = {
  VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO, NULL, 0,
  VK_FALSE, VK_FALSE, VK_COMPARE_OP_ALWAYS, VK_FALSE, VK_FALSE, {0}, {0}, 0.0f, 0.0f
};

static const VkPipelineColorBlendStateCreateInfo s_emptyColorBlendState = {
  VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO, NULL, 0,
  VK_FALSE, VK_LOGIC_OP_MAX_ENUM, 0, NULL,
  {0.0f, 0.0f, 0.0f, 0.0f}
};

static const VkPipelineDynamicStateCreateInfo s_emptyDynamicState = {
  VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO, NULL, 0, 0, NULL
};

static const VkGraphicsPipelineCreateInfo s_emptyGraphicsPipeline = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };

static const VkSwapchainCreateInfoKHR s_emptySwapchain = {
  VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR, NULL, 0,
  VK_NULL_HANDLE, UINT32_MAX, VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_MAX_ENUM_KHR,
  {0, 0}, 0, VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM, VK_SHARING_MODE_MAX_ENUM,
  0, NULL, VK_SURFACE_TRANSFORM_FLAG_BITS_MAX_ENUM_KHR, VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR,
  VK_PRESENT_MODE_MAX_ENUM_KHR, VK_FALSE, VK_NULL_HANDLE
};

static const VkPresentInfoKHR s_emptyPresent = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };

static const VkWriteDescriptorSet s_emptyWriteDescriptorSet = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
static const VkCopyDescriptorSet s_emptyCopyDescriptorSet = { VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET };

const VkInstanceCreateInfo *const emptyInstance() { 
  return &s_emptyInstance;
}

const VkApplicationInfo *const emptyApplication() { 
  return &s_emptyApplication;
}

const VkDeviceQueueCreateInfo *const emptyDeviceQueue() {
  return &s_emptyDeviceQueue;
};

const VkDeviceCreateInfo *const emptyDevice() {
  return &s_emptyDevice;
};

const VkCommandPoolCreateInfo *const emptyCommandPool() {
  return &s_emptyCommandPool;
}

const VkCommandBufferAllocateInfo *const emptyCommandBuffer() {
  return &s_emptyCommandBuffer;
}

const VkSubmitInfo *const emptySubmit() {
  return &s_emptySubmit;
}

const VkSemaphoreCreateInfo *const emptySemaphore() {
  return &s_emptySemaphore;
}

const VkFenceCreateInfo *const emptyFence() {
  return &s_emptyFence;
}

const VkRenderPassCreateInfo *const emptyRenderPass() {
  return &s_emptyRenderPass;
}

const VkMemoryAllocateInfo *const emptyMemory() {
  return &s_emptyMemory;
}

const VkBufferCreateInfo *const emptyBuffer() {
  return &s_emptyBuffer;
}

const VkBufferViewCreateInfo *const emptyBufferView() {
  return &s_emptyBufferView;
}

const VkImageCreateInfo *const emptyImage() {
  return &s_emptyImage;
}

const VkImageViewCreateInfo *const emptyImageView() {
  return &s_emptyImageView;
}

const VkDescriptorSetLayoutCreateInfo *const emptyDescriptorSetLayout() {
  return &s_emptyDescriptorSetLayout;
}

const VkPipelineLayoutCreateInfo *const emptyPipelineLayout() {
  return &s_emptyPipelineLayout;
}

const VkDescriptorPoolCreateInfo *const emptyDescriptorPool() {
  return &s_emptyDescriptorPool;
}

const VkDescriptorSetAllocateInfo *const emptyDescriptorSet() {
  return &s_emptyDescriptorSet;
}

const VkShaderModuleCreateInfo *const emptyShaderModule() {
  return &s_emptyShaderModule;
}

const VkPipelineShaderStageCreateInfo *const emptyShaderStage() {
  return &s_emptyShaderStage;
}

const VkPipelineVertexInputStateCreateInfo *const emptyVertexInputState() {
  return &s_emptyVertexInputState;
}

const VkPipelineInputAssemblyStateCreateInfo *const emptyInputAssemblyState() {
  return &s_emptyInputAssemblyState;
}

const VkPipelineTessellationStateCreateInfo *const emptyTessellationState() {
  return &s_emptyTessellationState;
}

const VkPipelineViewportStateCreateInfo *const emptyViewportState() {
  return &s_emptyViewportState;
}

const VkPipelineRasterizationStateCreateInfo *const emptyRasterizationState() {
  return &s_emptyRasterizationState;
}

const VkPipelineMultisampleStateCreateInfo *const emptyMultisampleState() {
  return &s_emptyMultisampleState;
}

const VkPipelineDepthStencilStateCreateInfo *const emptyDepthStencilState() {
  return &s_emptyDepthStencilState;
}

const VkPipelineColorBlendStateCreateInfo *const emptyColorBlendState() {
  return &s_emptyColorBlendState;
}

const VkPipelineDynamicStateCreateInfo *const emptyDynamicState() {
  return &s_emptyDynamicState;
}

const VkGraphicsPipelineCreateInfo *const emptyGraphicsPipeline() {
  return &s_emptyGraphicsPipeline;
}

const VkSwapchainCreateInfoKHR *const emptySwapchain() {
  return &s_emptySwapchain;
}

const VkPresentInfoKHR *const emptyPresent() {
  return &s_emptyPresent;
}

const VkWriteDescriptorSet *const emptyWriteDescriptorSet() {
  return &s_emptyWriteDescriptorSet;
}

const VkCopyDescriptorSet *const emptyCopyDescriptorSet() {
  return &s_emptyCopyDescriptorSet;
}
