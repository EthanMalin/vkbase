# vkbase

Small C library of helpers for Vulkan projects.

---

## Dependencies

- [Vulkan SDK](https://vulkan.lunarg.com/) — `VULKAN_SDK` environment variable must be set
- [SCons](https://scons.org/) — build tool

## Building

```sh
scons mac      # macOS
scons windows  # Windows
```

Output is placed in `dist/`:

```
dist/
  lib/libvkbase.a   # static library
  include/vkbase.h  # public header
```

---

## Usage

The typical initialization flow:

1. Use `vkb_emptyInstance()` / `vkb_simpleInstance()` to build a `VkInstanceCreateInfo`, then call `vkCreateInstance`.
2. Call `vkb_pickPhysicalDevice()` to select a suitable GPU. Use `vkb_getQueueFamilyIndices()` to find graphics/present/compute family indices.
3. Use `vkb_simpleDevice()` to build a `VkDeviceCreateInfo`, then call `vkCreateDevice`.
4. For windowed rendering, call `vkb_swapchainSupportDetails()` then `vkb_swapchainCreateInfo()` to configure the swapchain.
5. Allocate buffers/images with `vkb_createBuffer` / `vkb_createImage`, which handle memory allocation automatically.
6. Use `VK_CHECK(expr)` throughout to catch and surface Vulkan errors with file/line context.

All `vkb_empty*` functions return a zero-initialized create-info struct of the matching type — use them as a baseline and fill in only what you need. All `vkb_simple*` functions take the most commonly varied parameters and return a fully initialized struct.

---

## API Reference

### Macros

| Name | Description |
|------|-------------|
| `VK_CHECK(f)` | Wraps a `VkResult`-returning call; prints error and returns on failure (`!= VK_SUCCESS`) |
| `VK_CHECK2(f)` | Same, but allows positive non-zero results (e.g. `VK_INCOMPLETE`) |

### Data Types

| Type | Fields | Description |
|------|--------|-------------|
| `VkbBuffer` | `buffer`, `memory` | Buffer + backing device memory |
| `VkbImage` | `image`, `memory` | Image + backing device memory |
| `VkbShaderImage` | `image`, `view` | Image with an associated image view |
| `VkbQueueFamilyIndices` | `graphicsIndex`, `computeIndex`, `presentIndex`, `has*` | Queue family indices with presence flags |
| `VkbSwapchainSupportDetails` | `capabilities`, `formats[]`, `presentModes[]` | Queried swapchain capabilities for a surface |
| `VkbSwapchain` | `chain`, `imageCount`, `format`, `extent`, `images[]`, `views[]` | Full swapchain state |
| `VkbBase` | `inst`, `pdev`, `dev`, `gq`, `pq`, `qfi` | Core instance, device, and queue bundle |

### Resources — `vkresource.c`

| Function | Description |
|----------|-------------|
| `vkb_createBuffer(pd, d, info, memProps, out)` | Creates a `VkBuffer` and allocates memory matching `memProps` |
| `vkb_createImage(pd, d, info, memProps, out)` | Creates a `VkImage` and allocates memory matching `memProps` |
| `vkb_createShaderImage(pd, d, info, memProps, aspect, out)` | Creates a `VkbShaderImage` (image + view) with the given aspect flags |
| `vkb_destroyBuffer(d, buffer)` | Destroys buffer and frees its memory |
| `vkb_destroyImage(d, image)` | Destroys image and frees its memory |
| `vkb_destroyShaderImage(d, image)` | Destroys image view, image, and frees its memory |

### Simple create-info builders — `vksimple.c`

| Function | Description |
|----------|-------------|
| `vkb_simpleInstance(appInfo, debug)` | Instance create-info; enables validation layers when `debug` is true |
| `vkb_simpleDevice(queueCount, pQueues)` | Device create-info with required swapchain extension |
| `vkb_simpleShaderModule(codeSize, pCode)` | Shader module create-info from SPIR-V bytes |
| `vkb_simpleShaderStage(stage, module)` | Pipeline shader stage create-info |
| `vkb_simpleVertexInputState(...)` | Vertex input state with binding and attribute descriptions |
| `vkb_simpleInputAssemblyState(topology, primitiveRestart)` | Input assembly state |
| `vkb_simpleTessellationState(patchControlPoints)` | Tessellation state |
| `vkb_simpleViewportState(pViewport, pScissor)` | Viewport state with a single viewport and scissor |
| `vkb_simpleRasterizationState(polygonMode, cullMode, frontFace)` | Rasterization state |
| `vkb_simpleMultisampleState(sample)` | Multisample state |
| `vkb_simpleDepthStencilState(depthWrite, compareOp)` | Depth/stencil state |
| `vkb_simpleColorBlendState(pAttachment, blendConstants)` | Color blend state |
| `vkb_simpleDynamicState(count, pStates)` | Dynamic state |
| `vkb_simpleRenderPass(...)` | Render pass create-info |
| `vkb_simpleFramebuffer(...)` | Framebuffer create-info |

### Zero-initialized create-infos — `vkempties.c`

Each function returns a `const` pointer to a zero-initialized struct of the named Vulkan type. Use as a template then populate required fields.

`vkb_emptyInstance`, `vkb_emptyApplication`, `vkb_emptyDeviceQueue`, `vkb_emptyDevice`, `vkb_emptyCommandPool`, `vkb_emptyCommandBuffer`, `vkb_emptySubmit`, `vkb_emptyFence`, `vkb_emptySemaphore`, `vkb_emptyRenderPass`, `vkb_emptyFramebuffer`, `vkb_emptyMemory`, `vkb_emptyBuffer`, `vkb_emptyBufferView`, `vkb_emptyImage`, `vkb_emptyImageView`, `vkb_emptySampler`, `vkb_emptyDescriptorSetLayout`, `vkb_emptyPipelineLayout`, `vkb_emptyDescriptorPool`, `vkb_emptyDescriptorSet`, `vkb_emptyShaderModule`, `vkb_emptyShaderStage`, `vkb_emptyVertexInputState`, `vkb_emptyInputAssemblyState`, `vkb_emptyTessellationState`, `vkb_emptyViewportState`, `vkb_emptyRasterizationState`, `vkb_emptyMultisampleState`, `vkb_emptyDepthStencilState`, `vkb_emptyColorBlendState`, `vkb_emptyDynamicState`, `vkb_emptyGraphicsPipeline`, `vkb_emptySwapchain`, `vkb_emptyPresent`, `vkb_emptyWriteDescriptorSet`, `vkb_emptyCopyDescriptorSet`, `vkb_emptyCommandBufferBegin`, `vkb_emptyRenderPassBegin`

### Utilities — `vkutil.c`

| Function | Description |
|----------|-------------|
| `vkb_readFile(filename, buffer)` | Reads a file into a heap-allocated buffer; returns byte count |
| `vkb_getQueueFamilyIndex(pd, queueFlags)` | Returns first queue family index supporting `queueFlags` |
| `vkb_getQueueFamilyIndices(pd, surface, out)` | Fills `VkbQueueFamilyIndices` for graphics, compute, and present |
| `vkb_checkDeviceSupportsExtensions(pd)` | Returns true if the device supports required extensions |
| `vkb_isPhysicalDeviceSuitable(pd)` | Returns true if the device meets minimum requirements |
| `vkb_pickPhysicalDevice(instance, out)` | Selects the first suitable physical device |
| `vkb_findMemoryTypeIndex(pd, typeFilter, props)` | Finds a memory type index matching filter and property flags |

### Commands — `vkcmd.c`

| Function | Description |
|----------|-------------|
| `vkb_cmdTransitionImageLayout(cb, image, format, oldLayout, newLayout)` | Records a pipeline barrier to transition an image layout |

### Swapchain — `vkswap.c`

| Function | Description |
|----------|-------------|
| `vkb_swapchainSupportDetails(pd, queueFamily, surface)` | Queries surface capabilities, formats, and present modes |
| `vkb_swapchainCreateInfo(pd, queueFamily, surface, windowExtent)` | Builds a swapchain create-info with sensible defaults |

### Format — `vkformat.c`

| Function | Description |
|----------|-------------|
| `vkb_isDepthFormat(format)` | Returns true if the format is a depth or depth-stencil format |
