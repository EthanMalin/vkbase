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

int32_t getQueueFamilyIndex(VkPhysicalDevice physicalDevice, VkQueueFlagBits queueSupport) {
  uint32_t queueFamilyCount = 16;
  VkQueueFamilyProperties queueFamilyProperties[16];

  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties);
  for (int32_t i = 0; i < queueFamilyCount; i++) {
    if ((queueFamilyProperties[i].queueFlags & queueSupport) == queueSupport) {
      return  i;
    }
  }
  return -1;
}

void vkb_getQueueFamilyIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkbQueueFamilyIndices *qfi) {
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
    if (VK_FALSE == foundPresent && surface != VK_NULL_HANDLE) {
      vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
      if (VK_TRUE == presentSupport) {
        qfi->presentIndex = i;
        foundPresent = VK_TRUE;
        qfi->hasPresent = VK_TRUE;
      }
    }
  }
}

VkBool32 checkDeviceSupportsExtensions(VkPhysicalDevice physicalDevice) {
  VkBool32 allSupported = VK_TRUE;
  VkBool32 extensionSupported = VK_TRUE;
  uint32_t extensionCount = 0;
  VkExtensionProperties *availableExtensions = NULL;

  vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &extensionCount, NULL);
  availableExtensions = malloc(extensionCount * sizeof(VkExtensionProperties));
  vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &extensionCount, availableExtensions);

  for (uint32_t i = 0; i < VKB_NUM_DEVICE_EXTENSIONS; i++) {
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
  VkBool32 extensionsSupported = checkDeviceSupportsExtensions(physicalDevice);

  return (index >= 0) && extensionsSupported ? VK_TRUE : VK_FALSE;
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