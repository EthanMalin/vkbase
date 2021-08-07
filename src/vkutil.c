#include "vkbase.h"

size_t readFile(char *filename, char *buffer) {
  FILE *fp;
  size_t fileSize;
  fp = fopen(filename, "rb");
  if (fp) {
    if (0 == fseek(fp, 0, SEEK_END)) {
      fileSize = ftell(fp);
      if (fileSize != -1) {
        buffer = malloc(fileSize * sizeof(char));
        rewind(fp);
        fileSize = fread(buffer, sizeof(char), fileSize, fp);
        if (ferror(fp)) {
          free(buffer);
          fileSize = 0;
        }
      }
    }
  }
  return fileSize;
}

// gets index of a queue family on device which supports operations described in queueSupport
int32_t getQueueFamilyIndex(VkPhysicalDevice physicalDevice, VkQueueFlagBits queueSupport) {
  uint32_t queueFamilyCount = 16;
  VkQueueFamilyProperties queueFamilyProperties[16];

  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, NULL);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties);
  for (int32_t i = 0; i < queueFamilyCount; i++) {
    if ((queueFamilyProperties[i].queueFlags & queueSupport) == queueSupport) {
      return  i;
    }
  }
  return -1;
}

void getQueueFamilyIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, QueueFamilyIndices *qfi) {
  uint32_t queueFamilyCount = 16;
  VkQueueFamilyProperties queueProperties[16];
  VkBool32 presentSupport = VK_FALSE;

  VkBool32 foundGraphics = VK_FALSE;
  VkBool32 foundCompute = VK_FALSE;
  VkBool32 foundPresent = VK_FALSE;

  qfi->hasGraphics = VK_FALSE;
  qfi->hasPresent = VK_FALSE;
  qfi->hasCompute = VK_FALSE;

  qfi->graphicsIndex = UINT32_MAX;
  qfi->presentIndex = UINT32_MAX;
  qfi->computeIndex = UINT32_MAX;

  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, NULL);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueProperties);

  for (uint32_t i = 0; i < queueFamilyCount; i++) {
    if (VK_FALSE == foundGraphics) {
      if (VK_QUEUE_GRAPHICS_BIT & queueProperties[i].queueFlags) {
        qfi->graphicsIndex = i;
        foundGraphics = VK_TRUE;
        qfi->hasGraphics = VK_TRUE;
      }
    }
    if (VK_FALSE == foundCompute) {
      if (VK_QUEUE_COMPUTE_BIT & queueProperties[i].queueFlags) {
        qfi->computeIndex = i;
        foundCompute = VK_TRUE;
        qfi->hasCompute = VK_TRUE;
      }
    }
    if (VK_FALSE == foundPresent) {
      vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
      if (VK_TRUE == presentSupport) {
        qfi->presentIndex = i;
        foundPresent = VK_TRUE;
        qfi->hasPresent = VK_TRUE;
      }
    }
  }
}

VkBool32 checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice) {
  VkBool32 allSupported = VK_TRUE;
  VkBool32 extensionSupported = VK_TRUE;
  uint32_t extensionCount = 0;
  VkExtensionProperties *availableExtensions = NULL;

  vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &extensionCount, NULL);
  availableExtensions = malloc(extensionCount * sizeof(VkExtensionProperties));
  vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &extensionCount, availableExtensions);

  for (uint32_t i = 0; i < NUM_DEVICE_EXTENSIONS; i++) {
    extensionSupported = VK_FALSE;
    for (uint32_t j = 0; j < extensionCount; j++) {
      if (0 == strcmp(availableExtensions[j].extensionName, s_deviceExtensions[i])) {
        extensionSupported = VK_TRUE;
      }
    }
    allSupported &= extensionSupported;
  }

  free(availableExtensions);
  return allSupported;
}

VkBool32 isPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice) {
  int32_t index = getQueueFamilyIndex(physicalDevice, VK_QUEUE_GRAPHICS_BIT);
  VkBool32 extensionSupportPresent = checkDeviceExtensionSupport(physicalDevice);

  return (index >= 0) && extensionSupportPresent ? VK_TRUE : VK_FALSE;
}

VkResult pickPhysicalDevice(VkInstance instance, VkPhysicalDevice *physicalDevice) {
  VkPhysicalDevice devices[10];
  uint32_t deviceCount = 10;
  
  VK_CHECK(vkEnumeratePhysicalDevices(instance, &deviceCount, devices));
  for (uint32_t i = 0; i < deviceCount; i++) {
    if (isPhysicalDeviceSuitable(devices[i])) {
      *physicalDevice = devices[i];
      return VK_SUCCESS;
    }
  }
  return VK_ERROR_INITIALIZATION_FAILED;
}

int32_t findMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
  for (int32_t i = 0; i < memProperties.memoryTypeCount; i++) {
      if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
          return i;
      }
  }
  return -1;
}

// VkResult createSwapchain(VkPhysicalDevice physicalDevice, Device device, VkSurfaceKHR surface, GLFWwindow *window, Swapchain *swapchain) {
//   SwapchainSupportDetails swapchainSupport = querySwapchainSupportDetails(physicalDevice, device, surface);
//   VkSwapchainCreateInfoKHR infoSwapchain = {0};
//   VkImageViewCreateInfo infoView = {0};
//   VkComponentMapping componentMapping = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
//   VkImageSubresourceRange subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

//   if (swapchainSupport.numFormats > 0 && swapchainSupport.numPresentModes > 0 && swapchainSupport.presentSupportedOnQueue) {
//     infoSwapchain = swapchainInfo(physicalDevice, device, surface, window);
//     swapchain->format = infoSwapchain.imageFormat;
//     swapchain->extent = infoSwapchain.imageExtent;
//     VK_CHECK(vkCreateSwapchainKHR(device.logical, &infoSwapchain, NULL, &(swapchain->chain)));
//     VK_CHECK(vkGetSwapchainImagesKHR(device.logical, swapchain->chain, &(swapchain->imageCount), NULL));
//     VK_CHECK(vkGetSwapchainImagesKHR(device.logical, swapchain->chain, &(swapchain->imageCount), swapchain->images));
    
//     infoView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//     infoView.pNext = NULL;
//     infoView.flags = 0;
//     infoView.viewType = VK_IMAGE_VIEW_TYPE_2D;
//     infoView.format = swapchain->format;
//     infoView.components = componentMapping;
//     infoView.subresourceRange = subresourceRange;
//     for (uint32_t i = 0; i < swapchain->imageCount; i++) {
//       infoView.image = swapchain->images[i];
//       VK_CHECK(vkCreateImageView(device.logical, &infoView, NULL, &(swapchain->views[i])));
//     }
//   } else {
//     return VK_ERROR_INITIALIZATION_FAILED;
//   }
//   return VK_SUCCESS;
// }

// VkResult createDevice(VkInstance instance, VkPhysicalDevice physicalDevice, Device *device) {
//   const float priority = 1.0f;
//   VkDeviceQueueCreateInfo queueInfo = {
//     VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, NULL, 0,
//     0, 1, &priority
//   };
//   VkDeviceCreateInfo deviceInfo = {
//     VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, NULL, 0,
//     1, &queueInfo, 0, NULL,
//     NUM_DEVICE_EXTENSIONS, s_deviceExtensions,
//     NULL
//   };
//   device->queueFamilyIndex = getQueueFamilyIndex(physicalDevice, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);
//   queueInfo.queueFamilyIndex = device->queueFamilyIndex;
//   VK_CHECK(vkCreateDevice(physicalDevice, &deviceInfo, NULL, &(device->logical)));
//   vkGetDeviceQueue(device->logical, device->queueFamilyIndex, 0, &(device->queue));
//   return VK_SUCCESS;
// }

// VkResult createTexture(VkPhysicalDevice physicalDevice,
//                         Device device,
//                         VkFormat format,
//                         VkExtent2D size,
//                         VkImageUsageFlags usage,
//                         VkMemoryPropertyFlags properties,
//                         VkImageAspectFlags aspects,
//                         Texture *texture) {
//   VkImageCreateInfo infoImage = {
//     VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO, NULL, 0,
//     VK_IMAGE_TYPE_2D,
//     format,
//     {size.width, size.height, 1},
//     1, 1, VK_SAMPLE_COUNT_1_BIT,
//     VK_IMAGE_TILING_OPTIMAL,
//     usage,
//     VK_SHARING_MODE_EXCLUSIVE, 0, 0,
//     VK_IMAGE_LAYOUT_UNDEFINED
//   };

//   VkImageViewCreateInfo infoView = {
//     VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, NULL, 0,
//     VK_NULL_HANDLE,
//     VK_IMAGE_VIEW_TYPE_2D,
//     format,
//     {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
//     {aspects, 0, 1, 0, 1}
//   };

//   VkMemoryRequirements memRequirements = {0};
//   int32_t memoryTypeIndex = -1;
//   VkMemoryAllocateInfo infoMem = {
//     VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, NULL, 0, 0
//   };

//   VK_CHECK(vkCreateImage(device.logical, &infoImage, NULL, &(texture->image)));

//   vkGetImageMemoryRequirements(device.logical, texture->image, &memRequirements);
//   infoMem.allocationSize = memRequirements.size;
//   memoryTypeIndex = findMemoryTypeIndex(physicalDevice, memRequirements.memoryTypeBits, properties);
//   if (-1 == memoryTypeIndex) {
//     vkDestroyImage(device.logical, texture->image, NULL);
//     return VK_ERROR_INITIALIZATION_FAILED;
//   } else {
//     infoMem.memoryTypeIndex = memoryTypeIndex;
//   }
//   VK_CHECK(vkAllocateMemory(device.logical, &infoMem, NULL, &(texture->memory)));
//   VK_CHECK(vkBindImageMemory(device.logical, texture->image, texture->memory, 0));

//   infoView.image = texture->image;
//   VK_CHECK(vkCreateImageView(device.logical, &infoView, NULL, &(texture->view)));
//   return VK_SUCCESS;
// }

// VkResult createBuffer(VkPhysicalDevice physicalDevice, Device device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, Buffer *buffer) {
//   VkBufferCreateInfo bufferInfo = {
//     VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO, NULL, 0,
//     size,
//     usage,
//     VK_SHARING_MODE_EXCLUSIVE, 0, 0
//   };
//   VkMemoryRequirements memRequirements = {0};
//   VkMemoryAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, NULL, 0, -1 };

//   VK_CHECK(vkCreateBuffer(device.logical, &bufferInfo, NULL, &(buffer->buffer)));
//   vkGetBufferMemoryRequirements(device.logical, buffer->buffer, &memRequirements);
//   allocInfo.allocationSize = memRequirements.size;
//   allocInfo.memoryTypeIndex = findMemoryTypeIndex(physicalDevice, memRequirements.memoryTypeBits, properties);
//   VK_CHECK(vkAllocateMemory(device.logical, &allocInfo, NULL, &(buffer->memory)));
//   VK_CHECK(vkBindBufferMemory(device.logical, buffer->buffer, buffer->memory, 0));
//   if (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT & properties) {
//     vkMapMemory(device.logical, buffer->memory, 0, size, 0, &buffer->ptr);
//   }
//   return VK_SUCCESS;
// }

// void destroyBuffer(Device device, Buffer buffer) {
//   vkFreeMemory(device.logical, buffer.memory, NULL);
//   vkDestroyBuffer(device.logical, buffer.buffer, NULL);
// }

void destroyTexture(Device device, Texture texture) {
  vkDestroyImageView(device.logical, texture.view, NULL);
  vkFreeMemory(device.logical, texture.memory, NULL);
  vkDestroyImage(device.logical, texture.image, NULL);
}

void destroyDevice(VkInstance instance, Device device) {
  vkDestroyDevice(device.logical, NULL);
}

void destroySwapchain(Device device, Swapchain swapchain) {
  for (uint32_t i = 0; i < swapchain.imageCount; i++) {
    vkDestroyImageView(device.logical, swapchain.views[i], NULL);
  }
  vkDestroySwapchainKHR(device.logical, swapchain.chain, NULL);
}


