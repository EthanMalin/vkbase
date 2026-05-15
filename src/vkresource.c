#include "vkbase.h"

VkResult vkb_createBuffer(VkPhysicalDevice pd, VkDevice d, VkBufferCreateInfo *info, VkMemoryPropertyFlags memProps, VkbBuffer *out) {
  VkMemoryAllocateInfo infoMemory = *vkb_emptyMemory();
  VkMemoryRequirements memRequirements = {0};

  VK_CHECK(vkCreateBuffer(d, info, NULL, &(out->buffer)));

  vkGetBufferMemoryRequirements(d, out->buffer, &memRequirements);

  int32_t memTypeIndex = findMemoryTypeIndex(pd, memRequirements.memoryTypeBits, memProps);
  if (memTypeIndex < 0) return VK_ERROR_FEATURE_NOT_PRESENT;
  infoMemory.allocationSize = memRequirements.size;
  infoMemory.memoryTypeIndex = (uint32_t)memTypeIndex;
  VK_CHECK(vkAllocateMemory(d, &infoMemory, NULL, &(out->memory)));

  VK_CHECK(vkBindBufferMemory(d, out->buffer, out->memory, 0));

  return VK_SUCCESS;
}

void vkb_destroyBuffer(VkDevice d, VkbBuffer buffer) {
  vkDestroyBuffer(d, buffer.buffer, NULL);
  vkFreeMemory(d, buffer.memory, NULL);
}

VkResult vkb_createImage(VkPhysicalDevice pd, VkDevice d, VkImageCreateInfo *info, VkMemoryPropertyFlags memProps, VkbImage *out) {
  VkMemoryAllocateInfo infoMemory = *vkb_emptyMemory();
  VkMemoryRequirements memRequirements = {0};

  VK_CHECK(vkCreateImage(d, info, NULL, &(out->image)));

  vkGetImageMemoryRequirements(d, out->image, &memRequirements);

  int32_t memTypeIndex = findMemoryTypeIndex(pd, memRequirements.memoryTypeBits, memProps);
  if (memTypeIndex < 0) return VK_ERROR_FEATURE_NOT_PRESENT;
  infoMemory.allocationSize = memRequirements.size;
  infoMemory.memoryTypeIndex = (uint32_t)memTypeIndex;
  VK_CHECK(vkAllocateMemory(d, &infoMemory, NULL, &(out->memory)));

  VK_CHECK(vkBindImageMemory(d, out->image, out->memory, 0));

  return VK_SUCCESS;
}

void vkb_destroyImage(VkDevice d, VkbImage image) {
  vkDestroyImage(d, image.image, NULL);
  vkFreeMemory(d, image.memory, NULL);
}