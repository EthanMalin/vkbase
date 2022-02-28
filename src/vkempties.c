#include "vkbase.h"

static const VkApplicationInfo s_emptyApplication = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
static const VkInstanceCreateInfo s_emptyInstance = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
static const VkDeviceQueueCreateInfo s_emptyDeviceQueue = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
static const VkDeviceCreateInfo s_emptyDevice = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
static const VkCommandPoolCreateInfo s_emptyCommandPool = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
static const VkCommandBufferAllocateInfo s_emptyCommandBuffer = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
static const VkCommandBufferBeginInfo s_emptyCommandBufferBegin = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };

static const VkSubmitInfo s_emptySubmit = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
static const VkSemaphoreCreateInfo s_emptySemaphore = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
static const VkFenceCreateInfo s_emptyFence = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
static const VkFramebufferCreateInfo s_emptyFramebuffer = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
static const VkRenderPassCreateInfo s_emptyRenderPass = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
static const VkRenderPassBeginInfo s_emptyRenderPassBegin = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };

static const VkMemoryAllocateInfo s_emptyMemory = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
static const VkBufferCreateInfo s_emptyBuffer = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
static const VkBufferViewCreateInfo s_emptyBufferView = { VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO };
static const VkImageCreateInfo s_emptyImage = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
static const VkImageViewCreateInfo s_emptyImageView = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
static const VkSamplerCreateInfo s_emptySampler = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };

static const VkDescriptorSetLayoutCreateInfo s_emptyDescriptorSetLayout = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
static const VkPipelineLayoutCreateInfo s_emptyPipelineLayout = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
static const VkShaderModuleCreateInfo s_emptyShaderModule = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
static const VkPipelineShaderStageCreateInfo s_emptyShaderStage = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
static const VkPipelineVertexInputStateCreateInfo s_emptyVertexInputState = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
static const VkPipelineInputAssemblyStateCreateInfo s_emptyInputAssemblyState = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
static const VkPipelineTessellationStateCreateInfo s_emptyTessellationState = { VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO };
static const VkPipelineViewportStateCreateInfo s_emptyViewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
static const VkPipelineRasterizationStateCreateInfo s_emptyRasterizationState = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
static const VkPipelineMultisampleStateCreateInfo s_emptyMultisampleState = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
static const VkPipelineDepthStencilStateCreateInfo s_emptyDepthStencilState = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
static const VkPipelineColorBlendStateCreateInfo s_emptyColorBlendState = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
static const VkPipelineDynamicStateCreateInfo s_emptyDynamicState = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
static const VkGraphicsPipelineCreateInfo s_emptyGraphicsPipeline = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };

static const VkSwapchainCreateInfoKHR s_emptySwapchain = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
static const VkPresentInfoKHR s_emptyPresent = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };

static const VkDescriptorPoolCreateInfo s_emptyDescriptorPool = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
static const VkDescriptorSetAllocateInfo s_emptyDescriptorSet = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
static const VkWriteDescriptorSet s_emptyWriteDescriptorSet = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
static const VkCopyDescriptorSet s_emptyCopyDescriptorSet = { VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET };


const VkInstanceCreateInfo *const vkb_emptyInstance() { 
  return &s_emptyInstance;
}

const VkApplicationInfo *const vkb_emptyApplication() { 
  return &s_emptyApplication;
}

const VkDeviceQueueCreateInfo *const vkb_emptyDeviceQueue() {
  return &s_emptyDeviceQueue;
};

const VkDeviceCreateInfo *const vkb_emptyDevice() {
  return &s_emptyDevice;
};

const VkCommandPoolCreateInfo *const vkb_emptyCommandPool() {
  return &s_emptyCommandPool;
}

const VkCommandBufferAllocateInfo *const vkb_emptyCommandBuffer() {
  return &s_emptyCommandBuffer;
}

const VkSubmitInfo *const vkb_emptySubmit() {
  return &s_emptySubmit;
}

const VkSemaphoreCreateInfo *const vkb_emptySemaphore() {
  return &s_emptySemaphore;
}

const VkFenceCreateInfo *const vkb_emptyFence() {
  return &s_emptyFence;
}

const VkRenderPassCreateInfo *const vkb_emptyRenderPass() {
  return &s_emptyRenderPass;
}

const VkFramebufferCreateInfo *const vkb_emptyFramebuffer() {
  return &s_emptyFramebuffer;
}

const VkMemoryAllocateInfo *const vkb_emptyMemory() {
  return &s_emptyMemory;
}

const VkBufferCreateInfo *const vkb_emptyBuffer() {
  return &s_emptyBuffer;
}

const VkBufferViewCreateInfo *const vkb_emptyBufferView() {
  return &s_emptyBufferView;
}

const VkImageCreateInfo *const vkb_emptyImage() {
  return &s_emptyImage;
}

const VkImageViewCreateInfo *const vkb_emptyImageView() {
  return &s_emptyImageView;
}

const VkSamplerCreateInfo *const vkb_emptySampler() {
  return &s_emptySampler;
}

const VkDescriptorSetLayoutCreateInfo *const vkb_emptyDescriptorSetLayout() {
  return &s_emptyDescriptorSetLayout;
}

const VkPipelineLayoutCreateInfo *const vkb_emptyPipelineLayout() {
  return &s_emptyPipelineLayout;
}

const VkDescriptorPoolCreateInfo *const vkb_emptyDescriptorPool() {
  return &s_emptyDescriptorPool;
}

const VkDescriptorSetAllocateInfo *const vkb_emptyDescriptorSet() {
  return &s_emptyDescriptorSet;
}

const VkShaderModuleCreateInfo *const vkb_emptyShaderModule() {
  return &s_emptyShaderModule;
}

const VkPipelineShaderStageCreateInfo *const vkb_emptyShaderStage() {
  return &s_emptyShaderStage;
}

const VkPipelineVertexInputStateCreateInfo *const vkb_emptyVertexInputState() {
  return &s_emptyVertexInputState;
}

const VkPipelineInputAssemblyStateCreateInfo *const vkb_emptyInputAssemblyState() {
  return &s_emptyInputAssemblyState;
}

const VkPipelineTessellationStateCreateInfo *const vkb_emptyTessellationState() {
  return &s_emptyTessellationState;
}

const VkPipelineViewportStateCreateInfo *const vkb_emptyViewportState() {
  return &s_emptyViewportState;
}

const VkPipelineRasterizationStateCreateInfo *const vkb_emptyRasterizationState() {
  return &s_emptyRasterizationState;
}

const VkPipelineMultisampleStateCreateInfo *const vkb_emptyMultisampleState() {
  return &s_emptyMultisampleState;
}

const VkPipelineDepthStencilStateCreateInfo *const vkb_emptyDepthStencilState() {
  return &s_emptyDepthStencilState;
}

const VkPipelineColorBlendStateCreateInfo *const vkb_emptyColorBlendState() {
  return &s_emptyColorBlendState;
}

const VkPipelineDynamicStateCreateInfo *const vkb_emptyDynamicState() {
  return &s_emptyDynamicState;
}

const VkGraphicsPipelineCreateInfo *const vkb_emptyGraphicsPipeline() {
  return &s_emptyGraphicsPipeline;
}

const VkSwapchainCreateInfoKHR *const vkb_emptySwapchain() {
  return &s_emptySwapchain;
}

const VkPresentInfoKHR *const vkb_emptyPresent() {
  return &s_emptyPresent;
}

const VkWriteDescriptorSet *const vkb_emptyWriteDescriptorSet() {
  return &s_emptyWriteDescriptorSet;
}

const VkCopyDescriptorSet *const vkb_emptyCopyDescriptorSet() {
  return &s_emptyCopyDescriptorSet;
}

const VkCommandBufferBeginInfo *const vkb_emptyCommandBufferBegin() {
  return &s_emptyCommandBufferBegin;
}
const VkRenderPassBeginInfo *const vkb_emptyRenderPassBegin() {
  return &s_emptyRenderPassBegin;
}
