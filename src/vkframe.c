#include "vkbase.h"

VkResult vkb_createFrameContext(VkbBase *base, uint32_t frameCount, VkbFrameContext *out) {
  if (frameCount == 0 || frameCount > VKB_MAX_FRAMES_IN_FLIGHT)
    return VK_ERROR_INITIALIZATION_FAILED;

  out->frameCount   = frameCount;
  out->currentFrame = 0;

  VkCommandPoolCreateInfo poolInfo = *vkb_emptyCommandPool();
  poolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = base->qfi.graphicsIndex;
  VkResult result = vkCreateCommandPool(base->dev, &poolInfo, NULL, &out->cmdPool);
  if (result != VK_SUCCESS) return result;

  VkCommandBufferAllocateInfo allocInfo = *vkb_emptyCommandBuffer();
  allocInfo.commandPool        = out->cmdPool;
  allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = frameCount;
  result = vkAllocateCommandBuffers(base->dev, &allocInfo, out->cmdBuffers);
  if (result != VK_SUCCESS) {
    vkDestroyCommandPool(base->dev, out->cmdPool, NULL);
    return result;
  }

  VkSemaphoreCreateInfo semInfo   = *vkb_emptySemaphore();
  VkFenceCreateInfo     fenceInfo = *vkb_emptyFence();
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; /* pre-signaled so first frame doesn't stall */

  for (uint32_t i = 0; i < frameCount; i++) {
    out->imageAvailable[i] = VK_NULL_HANDLE;
    out->renderFinished[i] = VK_NULL_HANDLE;
    out->inFlight[i]       = VK_NULL_HANDLE;
  }

  for (uint32_t i = 0; i < frameCount; i++) {
    if (vkCreateSemaphore(base->dev, &semInfo,   NULL, &out->imageAvailable[i]) != VK_SUCCESS ||
        vkCreateSemaphore(base->dev, &semInfo,   NULL, &out->renderFinished[i]) != VK_SUCCESS ||
        vkCreateFence    (base->dev, &fenceInfo, NULL, &out->inFlight[i])       != VK_SUCCESS) {
      vkb_destroyFrameContext(base, out);
      return VK_ERROR_INITIALIZATION_FAILED;
    }
  }

  return VK_SUCCESS;
}

void vkb_destroyFrameContext(VkbBase *base, VkbFrameContext *ctx) {
  for (uint32_t i = 0; i < ctx->frameCount; i++) {
    if (ctx->imageAvailable[i]) vkDestroySemaphore(base->dev, ctx->imageAvailable[i], NULL);
    if (ctx->renderFinished[i]) vkDestroySemaphore(base->dev, ctx->renderFinished[i], NULL);
    if (ctx->inFlight[i])       vkDestroyFence    (base->dev, ctx->inFlight[i],       NULL);
  }
  if (ctx->cmdPool) vkDestroyCommandPool(base->dev, ctx->cmdPool, NULL);
}

VkResult vkb_beginFrame(VkbBase *base, VkbFrameContext *ctx, VkbSwapchain *swapchain,
                         uint32_t *imageIndex, VkCommandBuffer *cmd) {
  uint32_t f = ctx->currentFrame;

  vkWaitForFences(base->dev, 1, &ctx->inFlight[f], VK_TRUE, UINT64_MAX);
  vkResetFences  (base->dev, 1, &ctx->inFlight[f]);

  VkResult result = vkAcquireNextImageKHR(base->dev, swapchain->chain, UINT64_MAX,
                                          ctx->imageAvailable[f], VK_NULL_HANDLE, imageIndex);
  if (result != VK_SUCCESS) return result;

  vkResetCommandBuffer(ctx->cmdBuffers[f], 0);

  VkCommandBufferBeginInfo beginInfo = *vkb_emptyCommandBufferBegin();
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  result = vkBeginCommandBuffer(ctx->cmdBuffers[f], &beginInfo);
  if (result != VK_SUCCESS) return result;

  *cmd = ctx->cmdBuffers[f];
  return VK_SUCCESS;
}

VkResult vkb_endFrame(VkbBase *base, VkbFrameContext *ctx, VkbSwapchain *swapchain,
                       uint32_t imageIndex) {
  uint32_t f = ctx->currentFrame;

  vkEndCommandBuffer(ctx->cmdBuffers[f]);

  VkPipelineStageFlags waitStage  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  VkSubmitInfo         submitInfo = *vkb_emptySubmit();
  submitInfo.waitSemaphoreCount   = 1;
  submitInfo.pWaitSemaphores      = &ctx->imageAvailable[f];
  submitInfo.pWaitDstStageMask    = &waitStage;
  submitInfo.commandBufferCount   = 1;
  submitInfo.pCommandBuffers      = &ctx->cmdBuffers[f];
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores    = &ctx->renderFinished[f];
  VkResult result = vkQueueSubmit(base->gq, 1, &submitInfo, ctx->inFlight[f]);
  if (result != VK_SUCCESS) return result;

  VkPresentInfoKHR presentInfo = *vkb_emptyPresent();
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores    = &ctx->renderFinished[f];
  presentInfo.swapchainCount     = 1;
  presentInfo.pSwapchains        = &swapchain->chain;
  presentInfo.pImageIndices      = &imageIndex;
  result = vkQueuePresentKHR(base->pq, &presentInfo);

  ctx->currentFrame = (ctx->currentFrame + 1) % ctx->frameCount;
  return result;
}
