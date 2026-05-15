#include "vkbase.h"

void vkb_cmdTransitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
  VkImageAspectFlags aspect = vkb_isDepthFormat(format)
    ? VK_IMAGE_ASPECT_DEPTH_BIT
    : VK_IMAGE_ASPECT_COLOR_BIT;
  VkImageSubresourceRange wholeImage = {
    aspect, 0, 1, 0, 1
  };
  VkImageMemoryBarrier barrier = {
    VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER, NULL,
    VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT,
    VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT,
    oldLayout, newLayout,
    VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
    image, wholeImage
  };
  vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, NULL, 0, NULL, 1, &barrier);
  return;
}