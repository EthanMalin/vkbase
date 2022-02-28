#include "vkbase.h"

VkResult vkb_createBuffer(VkPhysicalDevice pd, VkDevice d, VkBufferCreateInfo *info, VkMemoryPropertyFlags memProps, Buffer *out) {
  VkMemoryAllocateInfo infoMemory = *vkb_emptyMemory();
  VkMemoryRequirements memRequirements = {0};

  VK_CHECK(vkCreateBuffer(d, info, NULL, &(out->buffer)));

  vkGetBufferMemoryRequirements(d, out->buffer, &memRequirements);

  infoMemory.allocationSize = memRequirements.size;
  infoMemory.memoryTypeIndex = findMemoryTypeIndex(pd, memRequirements.memoryTypeBits, memProps);
  VK_CHECK(vkAllocateMemory(d, &infoMemory, NULL, &(out->memory)));

  VK_CHECK(vkBindBufferMemory(d, out->buffer, out->memory, 0));

  return VK_SUCCESS;
}

void vkb_destroyBuffer(VkDevice d, Buffer buffer) {
  vkFreeMemory(d, buffer.memory, NULL);
  vkDestroyBuffer(d, buffer.buffer, NULL);
}

VkResult vkb_createImage(VkPhysicalDevice pd, VkDevice d, VkImageCreateInfo *info, VkMemoryPropertyFlags memProps, Image *out) {
  VkMemoryAllocateInfo infoMemory = *vkb_emptyMemory();
  VkMemoryRequirements memRequirements = {0};

  VK_CHECK(vkCreateImage(d, info, NULL, &(out->image)));

  vkGetImageMemoryRequirements(d, out->image, &memRequirements);

  infoMemory.allocationSize = memRequirements.size;
  infoMemory.memoryTypeIndex = findMemoryTypeIndex(pd, memRequirements.memoryTypeBits, memProps);
  VK_CHECK(vkAllocateMemory(d, &infoMemory, NULL, &(out->memory)));

  VK_CHECK(vkBindImageMemory(d, out->image, out->memory, 0));

  return VK_SUCCESS;
}

void vkb_destroyImage(VkDevice d, Image image) {
  vkFreeMemory(d, image.memory, NULL);
  vkDestroyImage(d, image.image, NULL);
}