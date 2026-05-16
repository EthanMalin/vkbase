#ifndef VK_BASE_NATIVE_H
#define VK_BASE_NATIVE_H

#include "vkbase.h"

typedef struct VkbNativeSurface {
    VkSurfaceKHR surface;
    VkExtent2D   extent;
    void        *handle; /* opaque: NSWindow* on Cocoa, GLFWwindow* on GLFW */
} VkbNativeSurface;

extern VkResult  vkb_createNativeSurface(VkInstance instance, const char *title, uint32_t width, uint32_t height, VkbNativeSurface *out);
extern void      vkb_destroyNativeSurface(VkInstance instance, VkbNativeSurface *surface);
extern VkBool32  vkb_nativeSurfaceShouldClose(VkbNativeSurface *surface);
extern void      vkb_nativeSurfacePollEvents(void);

#endif /* VK_BASE_NATIVE_H */
