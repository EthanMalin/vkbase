#include "vkbase.h"
#include "vkbnative.h"

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

VkResult vkb_createSwapchain(VkbBase *base, VkbNativeSurface *ns, VkFormat *requiredFormat, VkbSwapchain *out) {
  VkbSwapchainSupportDetails details = vkb_swapchainSupportDetails(base->pdev, base->qfi.presentIndex, ns->surface);

  VkSurfaceFormatKHR surfaceFormat;
  if (requiredFormat != NULL) {
    VkBool32 found = VK_FALSE;
    for (uint32_t i = 0; i < details.numFormats; i++) {
      if (details.formats[i].format == *requiredFormat) {
        surfaceFormat = details.formats[i];
        found = VK_TRUE;
        break;
      }
    }
    if (!found) return VK_ERROR_FORMAT_NOT_SUPPORTED;
  } else {
    surfaceFormat = s_chooseSwapchainSurfaceFormat(details.numFormats, details.formats);
  }

  VkPresentModeKHR presentMode = s_chooseSwapchainPresentMode(details.numPresentModes, details.presentModes);
  VkExtent2D extent = s_chooseSwapchainExtent(ns->extent, details.capabilities);

  uint32_t minImageCount = details.capabilities.minImageCount + 1;
  if (details.capabilities.maxImageCount > 0 && minImageCount > details.capabilities.maxImageCount)
    minImageCount = details.capabilities.maxImageCount;
  if (minImageCount > VKB_MAX_SWAPCHAIN_IMAGES)
    minImageCount = VKB_MAX_SWAPCHAIN_IMAGES;

  VkSwapchainCreateInfoKHR info = {
    VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR, NULL, 0,
    ns->surface,
    minImageCount, surfaceFormat.format, surfaceFormat.colorSpace,
    extent, 1, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    VK_SHARING_MODE_EXCLUSIVE, 0, NULL,
    details.capabilities.currentTransform,
    VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
    presentMode,
    VK_TRUE,
    VK_NULL_HANDLE
  };

  VkResult result = vkCreateSwapchainKHR(base->dev, &info, NULL, &out->chain);
  if (result != VK_SUCCESS) return result;

  out->format = surfaceFormat.format;
  out->extent = extent;
  out->imageCount = VKB_MAX_SWAPCHAIN_IMAGES;
  vkGetSwapchainImagesKHR(base->dev, out->chain, &out->imageCount, out->images);

  for (uint32_t i = 0; i < out->imageCount; i++) {
    VkImageViewCreateInfo viewInfo = *vkb_emptyImageView();
    viewInfo.image    = out->images[i];
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format   = out->format;
    viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel   = 0;
    viewInfo.subresourceRange.levelCount     = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount     = 1;
    result = vkCreateImageView(base->dev, &viewInfo, NULL, &out->views[i]);
    if (result != VK_SUCCESS) {
      for (uint32_t j = 0; j < i; j++) vkDestroyImageView(base->dev, out->views[j], NULL);
      vkDestroySwapchainKHR(base->dev, out->chain, NULL);
      return result;
    }
  }

  return VK_SUCCESS;
}

void vkb_destroySwapchain(VkbBase *base, VkbSwapchain *swapchain) {
  for (uint32_t i = 0; i < swapchain->imageCount; i++)
    vkDestroyImageView(base->dev, swapchain->views[i], NULL);
  vkDestroySwapchainKHR(base->dev, swapchain->chain, NULL);
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
    if (formats[i].format == VK_FORMAT_R8G8B8A8_UNORM) return formats[i];
  }
  for (uint32_t i = 0; i < numFormats; i++) {
    if (formats[i].format == VK_FORMAT_B8G8R8A8_UNORM) return formats[i];
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
  VkExtent2D extent;

  if (capabilities.currentExtent.width != UINT32_MAX) {
    extent = capabilities.currentExtent;
  } else {
    uint32_t w = window.width;
    uint32_t h = window.height;
    uint32_t minW = capabilities.minImageExtent.width;
    uint32_t maxW = capabilities.maxImageExtent.width;
    uint32_t minH = capabilities.minImageExtent.height;
    uint32_t maxH = capabilities.maxImageExtent.height;
    extent.width  = w < minW ? minW : (w > maxW ? maxW : w);
    extent.height = h < minH ? minH : (h > maxH ? maxH : h);
  }

  return extent;
}