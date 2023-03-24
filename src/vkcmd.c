#include "vkbase.h"

void vkb_cmdTransitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout) {
  VkImageSubresourceRange wholeImage = {
    VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1
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
}