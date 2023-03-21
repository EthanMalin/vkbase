#include "vkbase.h"

static VkSurfaceFormatKHR s_chooseSwapchainSurfaceFormat(uint32_t numFormats, VkSurfaceFormatKHR *formats);
static VkPresentModeKHR s_chooseSwapchainPresentMode(uint32_t numPresentModes, VkPresentModeKHR *modes);
static VkExtent2D s_chooseSwapchainExtent(VkExtent2D window, VkSurfaceCapabilitiesKHR capabilities);

VkSwapchainCreateInfoKHR vkb_swapchainCreateInfo(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface, VkExtent2D window) {
  VkbSwapchainSupportDetails details = vkb_swapchainSupportDetails(physicalDevice, queueFamilyIndex, surface);
  VkSurfaceFormatKHR format = s_chooseSwapchainSurfaceFormat(details.numFormats, details.formats);
  VkPresentModeKHR presentMode = s_chooseSwapchainPresentMode(details.numPresentModes, details.presentModes);
  VkExtent2D extent = s_chooseSwapchainExtent(window, details.capabilities);
  uint32_t minImageCount = details.capabilities.minImageCount + 1;
  if (details.capabilities.maxImageCount > 0 && minImageCount > details.capabilities.maxImageCount) {
    minImageCount = details.capabilities.maxImageCount;
  }
  if (minImageCount > VKB_MAX_SWAPCHAIN_IMAGES) {
    minImageCount = VKB_MAX_SWAPCHAIN_IMAGES;
  }
  VkSwapchainCreateInfoKHR info = {
    VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR, NULL, 0,
    surface,
    minImageCount, format.format, format.colorSpace,
    extent, 1, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    VK_SHARING_MODE_EXCLUSIVE, 0, NULL,
    details.capabilities.currentTransform,
    VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
    presentMode,
    VK_TRUE,
    VK_NULL_HANDLE
  };
  return info;
}

VkbSwapchainSupportDetails vkb_swapchainSupportDetails(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface) {
  VkbSwapchainSupportDetails details = {0};
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &details.numFormats, NULL);
  if (details.numFormats != 0) {
    details.numFormats = (details.numFormats > VKB_MAX_SWAPCHAIN_SURFACE_FORMATS) ? VKB_MAX_SWAPCHAIN_SURFACE_FORMATS : details.numFormats;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &details.numFormats, details.formats);
  }
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &details.numPresentModes, NULL);
  if (details.numPresentModes != 0) {
    details.numPresentModes = (details.numPresentModes > VKB_MAX_SWAPCHAIN_PRESENT_MODES) ? VKB_MAX_SWAPCHAIN_PRESENT_MODES : details.numPresentModes;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &details.numPresentModes, details.presentModes);
  }
  vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, &details.presentSupportedOnQueue);
  return details;
}

VkSurfaceFormatKHR s_chooseSwapchainSurfaceFormat(uint32_t numFormats, VkSurfaceFormatKHR *formats) {
  for (uint32_t i = 0; i < numFormats; i++) {
    if (formats[i].format == VK_FORMAT_R8G8B8A8_UNORM) {
      return formats[i];
    }
  }
  return formats[0];
}

VkPresentModeKHR s_chooseSwapchainPresentMode(uint32_t numPresentModes, VkPresentModeKHR *modes) {
  for (uint32_t i = 0; i < numPresentModes; i++) {
    if (modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
      return modes[i];
    }
  }
  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D s_chooseSwapchainExtent(VkExtent2D window, VkSurfaceCapabilitiesKHR capabilities) {
  int width, height;
  VkExtent2D extent;

  if (capabilities.currentExtent.width != UINT32_MAX) {
    extent = capabilities.currentExtent;
  } else {
	  extent.width = window.width;
	  extent.height = window.height;
    #define MAX(x, y) (((x) > (y)) ? (x) : (y))
    #define MIN(x, y) (((x) < (y)) ? (x) : (y))
    extent.width = MAX(capabilities.minImageExtent.width, MIN(capabilities.maxImageExtent.width, extent.width));
    extent.height = MAX(capabilities.minImageExtent.height, MIN(capabilities.maxImageExtent.height, extent.height));
    #undef MAX
    #undef MIN
  }

  return extent;
}