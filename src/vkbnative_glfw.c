#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "vkbnative.h"

VkResult vkb_createNativeSurface(VkInstance instance, const char *title, uint32_t width, uint32_t height, VkbNativeSurface *out) {
    if (!glfwInit()) return VK_ERROR_INITIALIZATION_FAILED;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE,  GLFW_FALSE);

    GLFWwindow *window = glfwCreateWindow((int)width, (int)height, title, NULL, NULL);
    if (!window) {
        glfwTerminate();
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    VkResult result = glfwCreateWindowSurface(instance, window, NULL, &out->surface);
    if (result != VK_SUCCESS) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return result;
    }

    out->extent.width  = width;
    out->extent.height = height;
    out->handle = window;

    return VK_SUCCESS;
}

void vkb_destroyNativeSurface(VkInstance instance, VkbNativeSurface *surface) {
    vkDestroySurfaceKHR(instance, surface->surface, NULL);
    glfwDestroyWindow((GLFWwindow *)surface->handle);
    glfwTerminate();
    surface->handle  = NULL;
    surface->surface = VK_NULL_HANDLE;
}

VkBool32 vkb_nativeSurfaceShouldClose(VkbNativeSurface *surface) {
    return glfwWindowShouldClose((GLFWwindow *)surface->handle) ? VK_TRUE : VK_FALSE;
}

void vkb_nativeSurfacePollEvents(void) {
    glfwPollEvents();
}
