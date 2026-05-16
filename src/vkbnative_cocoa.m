#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>
#include <vulkan/vulkan_metal.h>
#include "vkbnative.h"

@interface VkbWindowDelegate : NSObject <NSWindowDelegate>
@property (nonatomic) BOOL shouldClose;
@end

@implementation VkbWindowDelegate
- (BOOL)windowShouldClose:(NSWindow *)sender {
    self.shouldClose = YES;
    return NO;
}
@end

/* ARC-managed wrapper so the window and delegate stay alive for the surface lifetime. */
@interface VkbCocoaHandle : NSObject
@property (strong) NSWindow          *window;
@property (strong) VkbWindowDelegate *delegate;
@end

@implementation VkbCocoaHandle
@end

VkResult vkb_createNativeSurface(VkInstance instance, const char *title, uint32_t width, uint32_t height, VkbNativeSurface *out) {
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    [NSApp finishLaunching];

    NSRect frame = NSMakeRect(0, 0, (CGFloat)width, (CGFloat)height);
    NSWindowStyleMask style = NSWindowStyleMaskTitled
                            | NSWindowStyleMaskClosable
                            | NSWindowStyleMaskMiniaturizable;
    NSWindow *window = [[NSWindow alloc] initWithContentRect:frame
                                                   styleMask:style
                                                     backing:NSBackingStoreBuffered
                                                       defer:NO];

    VkbWindowDelegate *delegate = [[VkbWindowDelegate alloc] init];
    delegate.shouldClose = NO;
    [window setDelegate:delegate];
    [window setTitle:[NSString stringWithUTF8String:title]];

    NSView *view = [window contentView];
    [view setWantsLayer:YES];
    CAMetalLayer *layer = [CAMetalLayer layer];
    [view setLayer:layer];

    [window center];
    [window makeKeyAndOrderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];

    VkMetalSurfaceCreateInfoEXT info = {
        .sType  = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT,
        .pNext  = NULL,
        .flags  = 0,
        .pLayer = (__bridge CAMetalLayer *)layer,
    };
    VkResult result = vkCreateMetalSurfaceEXT(instance, &info, NULL, &out->surface);
    if (result != VK_SUCCESS) return result;

    out->extent.width  = width;
    out->extent.height = height;

    VkbCocoaHandle *handle = [[VkbCocoaHandle alloc] init];
    handle.window   = window;
    handle.delegate = delegate;
    out->handle = (__bridge_retained void *)handle;

    return VK_SUCCESS;
}

void vkb_destroyNativeSurface(VkInstance instance, VkbNativeSurface *surface) {
    VkbCocoaHandle *handle = (__bridge_transfer VkbCocoaHandle *)surface->handle;
    vkDestroySurfaceKHR(instance, surface->surface, NULL);
    [handle.window close];
    surface->handle  = NULL;
    surface->surface = VK_NULL_HANDLE;
}

VkBool32 vkb_nativeSurfaceShouldClose(VkbNativeSurface *surface) {
    VkbCocoaHandle *handle = (__bridge VkbCocoaHandle *)surface->handle;
    return handle.delegate.shouldClose ? VK_TRUE : VK_FALSE;
}

void vkb_nativeSurfacePollEvents(void) {
    NSEvent *event;
    while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                       untilDate:[NSDate distantPast]
                                          inMode:NSDefaultRunLoopMode
                                         dequeue:YES])) {
        [NSApp sendEvent:event];
    }
}
