#include "data.h"
#include <assert.h>
#include <string.h>
#include <vk_mem_alloc.h>

VkExtent2D extent = {.width = 800, .height = 600};

/***********************************************
 * These are here if they need to be destroyed *
 ***********************************************/
VkInstance instance = VK_NULL_HANDLE;
VkSurfaceKHR surface = VK_NULL_HANDLE;
VkDevice device = VK_NULL_HANDLE;
VmaAllocator allocator;
VkSwapchainKHR swapchain = VK_NULL_HANDLE;
VkImage swapchain_images[3] = {VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE};
VkImageView swapchain_image_views[3] = {VK_NULL_HANDLE, VK_NULL_HANDLE,
                                        VK_NULL_HANDLE};
VkImage depth_stencil_image = VK_NULL_HANDLE;
VmaAllocation depth_stencil_image_allocation = NULL;
VkImageView depth_stencil_image_view = VK_NULL_HANDLE;
VkRenderPass render_pass = VK_NULL_HANDLE;
VkFramebuffer framebuffers[3] = {VK_NULL_HANDLE, VK_NULL_HANDLE,
                                 VK_NULL_HANDLE};
VkBuffer uniform_buffer = VK_NULL_HANDLE;
VmaAllocation uniform_buffer_allocation = NULL;
VkDescriptorSetLayout descriptor_set_layout = VK_NULL_HANDLE;
VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
VkSampler texture_image_sampler;
VkBuffer vertex_buffer = VK_NULL_HANDLE;
VmaAllocation vertex_buffer_allocation = NULL;
VkBuffer index_buffer = VK_NULL_HANDLE;
VmaAllocation index_buffer_allocation = NULL;
VkImageView *texture_image_views;
VkCommandPool command_pool = VK_NULL_HANDLE;
VkSemaphore wait_semaphore = VK_NULL_HANDLE;
VkFence queue_submit_fence = VK_NULL_HANDLE;
VkFence acquire_next_image_fence = VK_NULL_HANDLE;

static void error_callback(int error, const char *description)
{
        fprintf(stderr, "Error %d: %s\n", error, description);
}

inline static void init_create_window()
{
        VkApplicationInfo application_info;
        static const VkApplicationInfo EmptyVkApplicationInfo;
        application_info = EmptyVkApplicationInfo;
        application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        application_info.apiVersion = VK_API_VERSION_1_0;
        application_info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
        application_info.pApplicationName = "Cheako Vulkan Test 12";

        glfwSetErrorCallback(error_callback);
        assert(glfwInit());

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        data_window = glfwCreateWindow(extent.width, extent.height,
                                       application_info.pApplicationName, NULL, NULL);
        assert(glfwVulkanSupported());

        VkInstanceCreateInfo instance_create_info;
        static const VkInstanceCreateInfo EmptyVkInstanceCreateInfo;
        instance_create_info = EmptyVkInstanceCreateInfo;
        instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_create_info.pApplicationInfo = &application_info;

        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        instance_create_info.enabledExtensionCount = glfwExtensionCount;
        instance_create_info.ppEnabledExtensionNames = glfwExtensions;

        VkResult err;
        err = vkCreateInstance(&instance_create_info, NULL, &instance);
        assert((err == VK_SUCCESS) && "vkCreateInstance: failed.");

        err = glfwCreateWindowSurface(instance, data_window, NULL, &surface);
        assert(err == VK_SUCCESS && "glfwCreateWindowSurface: Failed.");
}

inline static void
find_device_surface(VkPhysicalDevice *gpu)
{
        uint32_t gpu_count;
        VkResult err;
        err = vkEnumeratePhysicalDevices(instance, &gpu_count, NULL);
        assert((err == VK_SUCCESS) && gpu_count > 0 &&
               "vkEnumeratePhysicalDevices: Failed to count gpus.");

        {
                VkPhysicalDevice gpus[gpu_count];
                err = vkEnumeratePhysicalDevices(instance, &gpu_count, gpus);
                assert((err == VK_SUCCESS) &&
                       "vkEnumeratePhysicalDevices: Failed to discover gpus.");
                /*	  for (size_t i = 0; i < gpu_count; i++) */
                {
                        /* TODO: This just selects first GPU. */
                        *gpu = gpus[0];

                        VkSurfaceCapabilitiesKHR surface_capabilities;
                        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*gpu, surface,
                                                                  &surface_capabilities);
                        if (surface_capabilities.currentExtent.width < UINT32_MAX)
                        {
                                extent = surface_capabilities.currentExtent;
                        }
                        else
                        {
                                int width, height;
                                glfwGetWindowSize(data_window, &width, &height);
                                extent = (VkExtent2D){.width = width, .height = height};
                        }
                }
                assert((*gpu != VK_NULL_HANDLE) &&
                       "Vulkan ERROR: Usable device not found.");
        }
}

inline static void create_device_queu_family(VkPhysicalDevice gpu,
                                             uint32_t *queue_family_index)
{
        uint32_t queue_count;
        vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_count, NULL);
        assert((queue_count > 0) &&
               "vkGetPhysicalDeviceQueueFamilyProperties: Failed to count queue.");
        VkQueueFamilyProperties queue_props[queue_count];
        vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_count, queue_props);
        assert((queue_count > 0) &&
               "vkGetPhysicalDeviceQueueFamilyProperties: Failed to list queue.");

        *queue_family_index = UINT32_MAX;
        for (size_t i = 0; i < queue_count; i++)
        {
                VkBool32 supports_present;
                VkResult err;
                err = vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface,
                                                           &supports_present);
                assert((err == VK_SUCCESS) &&
                       "vkGetPhysicalDeviceSurfaceSupportKHR: Failed.");
                if (supports_present && (queue_props[i].queueFlags &
                                         VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT)
                {
                        *queue_family_index = i;
                        break;
                }
        }
        assert((*queue_family_index != UINT32_MAX) &&
               "Vulkan ERROR: Usable queue family not found.");

        static const float queue_priorities[1] = {1.0f};
        VkDeviceQueueCreateInfo device_queue_create_info;
        static const VkDeviceQueueCreateInfo EmptyVkDeviceQueueCreateInfo;
        device_queue_create_info = EmptyVkDeviceQueueCreateInfo;
        device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        device_queue_create_info.queueFamilyIndex = *queue_family_index;
        device_queue_create_info.queueCount = 1;
        device_queue_create_info.pQueuePriorities = queue_priorities;

        VkDeviceCreateInfo device_create_info;
        static const VkDeviceCreateInfo EmptyVkDeviceCreateInfo;
        device_create_info = EmptyVkDeviceCreateInfo;
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.queueCreateInfoCount = 1;
        device_create_info.pQueueCreateInfos = &device_queue_create_info;
        device_create_info.enabledExtensionCount = 1;
        const char *enable_extension_names[1] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
        device_create_info.ppEnabledExtensionNames = enable_extension_names;

        VkResult err;
        err = vkCreateDevice(gpu, &device_create_info, NULL, &device);
        assert((err == VK_SUCCESS) && "vkCreateDevice: Failed.");
}

inline static void discover_surface_format(VkPhysicalDevice gpu,
                                           VkSurfaceFormatKHR *surface_format)
{
        VkResult err;
        surface_format->format = VK_FORMAT_MAX_ENUM;
        surface_format->colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        uint32_t format_count;
        err = vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &format_count, NULL);
        assert((err == VK_SUCCESS) && (format_count > 0) &&
               "vkGetPhysicalDeviceSurfaceFormatsKHR: Failed to count formats.");
        VkSurfaceFormatKHR formats[format_count];
        err = vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &format_count,
                                                   formats);
        assert((err == VK_SUCCESS) &&
               "vkGetPhysicalDeviceSurfaceFormatsKHR: Failed to list formats.");

        if (formats[0].format == VK_FORMAT_UNDEFINED)
        {
                surface_format->format = VK_FORMAT_B8G8R8A8_UNORM;
        }
        else
                *surface_format = formats[0];
        assert((surface_format->format != VK_FORMAT_MAX_ENUM) &&
               "Vulkan ERROR: Usable surface format not found.");
}

inline static void
fifo_or_mailbox_present_mode(VkPhysicalDevice gpu,
                             VkPresentModeKHR *present_mode,
                             uint32_t *swapchain_image_count)
{
        *present_mode = VK_PRESENT_MODE_FIFO_KHR;
        uint32_t mode_count;
        VkResult err;
        err = vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &mode_count,
                                                        NULL);
        if ((err != VK_SUCCESS) && (mode_count > 0))
                return;

        {
                VkPresentModeKHR modes[mode_count];
                err = vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &mode_count,
                                                                modes);
                if ((err != VK_SUCCESS) && (mode_count > 0))
                        return;

                for (size_t i = 0; i < mode_count; i++)
                {
                        if (modes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
                        {
                                *present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
                                *swapchain_image_count = 3;
                                return;
                        }
                }
        }
}

inline static void create_swapchain(uint32_t swapchain_image_count,
                                    VkSurfaceFormatKHR surface_format,
                                    VkPresentModeKHR present_mode)
{
        VkSwapchainCreateInfoKHR swapchain_create_info;
        static const VkSwapchainCreateInfoKHR EmptyVkSwapchainCreateInfoKHR;
        swapchain_create_info = EmptyVkSwapchainCreateInfoKHR;
        swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchain_create_info.surface = surface;
        swapchain_create_info.minImageCount = swapchain_image_count;
        swapchain_create_info.imageFormat = surface_format.format;
        swapchain_create_info.imageColorSpace = surface_format.colorSpace;
        swapchain_create_info.imageExtent = extent;
        swapchain_create_info.imageArrayLayers = 1;
        swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_create_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchain_create_info.presentMode = present_mode;
        swapchain_create_info.clipped = VK_TRUE;
        swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

        VkResult err;
        err = vkCreateSwapchainKHR(device, &swapchain_create_info, NULL, &swapchain);
        assert((err == VK_SUCCESS) && "vkCreateSwapchainKHR: Failed.");
}

inline static void
discover_swapchain_image_count(VkPresentModeKHR present_mode,
                               uint32_t *swapchain_image_count)
{
        uint32_t image_count = (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) ? 3 : 2;
        VkResult err;
        err = vkGetSwapchainImagesKHR(device, swapchain, swapchain_image_count, NULL);
        assert((err == VK_SUCCESS) &&
               "vkGetSwapchainImagesKHR: Failed to count swapchain images.");
        assert((*swapchain_image_count == image_count) &&
               "vkGetSwapchainImagesKHR: Count swapchain images wrong.");
        err = vkGetSwapchainImagesKHR(device, swapchain, &image_count,
                                      swapchain_images);
        assert((err == VK_SUCCESS) &&
               "vkGetSwapchainImagesKHR: Failed to get swapchain images.");
        assert((image_count == *swapchain_image_count) &&
               "vkGetSwapchainImagesKHR: Get wrong count swapchain images.");
}

inline static void
create_swapchain_image_views(VkSurfaceFormatKHR surface_format,
                             uint32_t swapchain_image_count)
{
        VkImageViewCreateInfo image_view_create_info;
        static const VkImageViewCreateInfo EmptyVkImageViewCreateInfo;
        image_view_create_info = EmptyVkImageViewCreateInfo;
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.format = surface_format.format;
        image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.subresourceRange.aspectMask =
            VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_create_info.subresourceRange.baseMipLevel = 0;
        image_view_create_info.subresourceRange.levelCount = 1;
        image_view_create_info.subresourceRange.baseArrayLayer = 0;
        image_view_create_info.subresourceRange.layerCount = 1;
        for (size_t i = 0; i < swapchain_image_count; i++)
        {
                image_view_create_info.image = swapchain_images[i];

                VkResult err;
                err = vkCreateImageView(device, &image_view_create_info, NULL,
                                        &swapchain_image_views[i]);
                assert((err == VK_SUCCESS) && "vkCreateImageView: Failed swapchain.");
        }
}

inline static void setup_swapchain(VkPhysicalDevice gpu,
                                   VkSurfaceFormatKHR surface_format,
                                   uint32_t *swapchain_image_count)
{
        VkPresentModeKHR present_mode;
        fifo_or_mailbox_present_mode(gpu, &present_mode, swapchain_image_count);
        create_swapchain(*swapchain_image_count, surface_format, present_mode);
        discover_swapchain_image_count(present_mode, swapchain_image_count);
        create_swapchain_image_views(surface_format, *swapchain_image_count);
}

inline static void choose_depth_stencil_format(VkPhysicalDevice gpu,
                                               VkFormat *depth_stencil_format)
{
        VkFormatProperties format_properties;
        vkGetPhysicalDeviceFormatProperties(gpu, VK_FORMAT_D32_SFLOAT_S8_UINT,
                                            &format_properties);
        if ((format_properties.optimalTilingFeatures &
             VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) ==
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
                *depth_stencil_format = VK_FORMAT_D32_SFLOAT_S8_UINT;
                return;
        }
        assert(0 && "vkGetPhysicalDeviceFormatProperties: Couldn't find usable depth "
                    "stencil format.");
}

inline static void create_vma(VkPhysicalDevice gpu)
{
        VmaAllocatorCreateInfo allocatorInfo;
        static const VmaAllocatorCreateInfo EmptyVmaAllocatorCreateInfo;
        allocatorInfo = EmptyVmaAllocatorCreateInfo;
        allocatorInfo.physicalDevice = gpu;
        allocatorInfo.device = device;

        VkResult err;
        err = vmaCreateAllocator(&allocatorInfo, &allocator);
        assert((err == VK_SUCCESS) && "vmaCreateAllocator: Faileld.");
}

inline static void create_depth_stencil_image(VkFormat depth_stencil_format)
{
        VkImageCreateInfo image_create_info;
        static const VkImageCreateInfo EmptyVkImageViewCreateInfo;
        image_create_info = EmptyVkImageViewCreateInfo;
        image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_create_info.imageType = VK_IMAGE_TYPE_2D;
        image_create_info.format = depth_stencil_format;
        image_create_info.extent.width = extent.width;
        image_create_info.extent.height = extent.height;
        image_create_info.extent.depth = 1;
        image_create_info.mipLevels = 1;
        image_create_info.arrayLayers = 1;
        image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_create_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_create_info.queueFamilyIndexCount = VK_QUEUE_FAMILY_IGNORED;
        image_create_info.pQueueFamilyIndices = NULL;
        image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VmaAllocationCreateInfo alloc_create_info;
        static const VmaAllocationCreateInfo EmptyVmaAllocationCreateInfo;
        alloc_create_info = EmptyVmaAllocationCreateInfo;
        alloc_create_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        VkResult err;
        err = vmaCreateImage(allocator, &image_create_info, &alloc_create_info, &depth_stencil_image, &depth_stencil_image_allocation, NULL);
        assert((err == VK_SUCCESS) && "vkCreateImage: Failed depth stencil.");
}

inline static void
create_depth_stencil_image_view(VkFormat depth_stencil_format)
{
        VkImageViewCreateInfo image_view_create_info;
        static const VkImageViewCreateInfo EmptyVkImageViewCreateInfo;
        image_view_create_info = EmptyVkImageViewCreateInfo;
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.image = depth_stencil_image;
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.format = depth_stencil_format;
        image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.subresourceRange.aspectMask =
            VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        image_view_create_info.subresourceRange.baseMipLevel = 0;
        image_view_create_info.subresourceRange.levelCount = 1;
        image_view_create_info.subresourceRange.baseArrayLayer = 0;
        image_view_create_info.subresourceRange.layerCount = 1;

        VkResult err;
        err = vkCreateImageView(device, &image_view_create_info, NULL,
                                &depth_stencil_image_view);
        assert((err == VK_SUCCESS) && "vkCreateImageView: Failed depth stencil.");
}

inline static void setup_depth_stencil_image(
    VkPhysicalDevice gpu, VkFormat *depth_stencil_format)
{
        choose_depth_stencil_format(gpu, depth_stencil_format);
        create_depth_stencil_image(*depth_stencil_format);
        create_depth_stencil_image_view(*depth_stencil_format);
}

inline static void create_render_pass(VkFormat depth_stencil_format,
                                      VkSurfaceFormatKHR surface_format)
{
        VkAttachmentDescription attachments[2];
        static const VkAttachmentDescription EmptyVkAttachmentDescription;
        attachments[0] = attachments[1] = EmptyVkAttachmentDescription;
        attachments[0].format = depth_stencil_format;
        attachments[1].format = surface_format.format;
        attachments[0].samples = attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
        attachments[0].loadOp = attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[0].stencilLoadOp = attachments[1].stencilLoadOp =
            VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachments[0].stencilStoreOp = attachments[1].stencilStoreOp =
            VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachments[0].initialLayout = attachments[1].initialLayout =
            VK_IMAGE_LAYOUT_UNDEFINED;
        attachments[0].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachments[1].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference sub_passes_depth_stencil_attachment[1];
        static const VkAttachmentReference EmptyVkAttachmentReference;
        sub_passes_depth_stencil_attachment[0] = EmptyVkAttachmentReference;
        sub_passes_depth_stencil_attachment[0].attachment = 0;
        sub_passes_depth_stencil_attachment[0].layout =
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference sub_passes_color_attachments[1][1];
        sub_passes_color_attachments[0][0] = EmptyVkAttachmentReference;
        sub_passes_color_attachments[0][0].attachment = 1;
        sub_passes_color_attachments[0][0].layout =
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription sub_passes[1];
        static const VkSubpassDescription EmptyVkSubpassDescription;
        sub_passes[0] = EmptyVkSubpassDescription;
        sub_passes[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        sub_passes[0].colorAttachmentCount = 1;
        sub_passes[0].pColorAttachments = sub_passes_color_attachments[0];
        sub_passes[0].pDepthStencilAttachment =
            &sub_passes_depth_stencil_attachment[0];

        VkRenderPassCreateInfo render_pass_create_info;
        static const VkRenderPassCreateInfo EmptyVkRenderPassCreateInfo;
        render_pass_create_info = EmptyVkRenderPassCreateInfo;
        render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_create_info.attachmentCount = 2;
        render_pass_create_info.pAttachments = attachments;
        render_pass_create_info.subpassCount = 1;
        render_pass_create_info.pSubpasses = sub_passes;

        VkResult err;
        err =
            vkCreateRenderPass(device, &render_pass_create_info, NULL, &render_pass);
        assert((err == VK_SUCCESS) && "vkCreateRenderPass: Failed.");
}

inline static void create_framebuffers(uint32_t swapchain_image_count)
{
        VkImageView attachments[] = {depth_stencil_image_view, VK_NULL_HANDLE};

        VkFramebufferCreateInfo framebuffer_create_info;
        static const VkFramebufferCreateInfo EmptyVkFramebufferCreateInfo;
        framebuffer_create_info = EmptyVkFramebufferCreateInfo;
        framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_create_info.renderPass = render_pass;
        framebuffer_create_info.attachmentCount = 2;
        framebuffer_create_info.pAttachments = attachments;
        framebuffer_create_info.width = extent.width;
        framebuffer_create_info.height = extent.height;
        framebuffer_create_info.layers = 1;

        for (size_t i = 0; i < swapchain_image_count; i++)
        {
                attachments[1] = swapchain_image_views[i];

                VkResult err;
                err = vkCreateFramebuffer(device, &framebuffer_create_info, NULL,
                                          &framebuffers[i]);
                assert((err == VK_SUCCESS) && "vkCreateFramebuffer: Failed.");
        }
}

inline static void create_uniform_buffer()
{
        VkBufferCreateInfo buffer_info_create;
        static const VkBufferCreateInfo EmptyVkBufferCreateInfo;
        buffer_info_create = EmptyVkBufferCreateInfo;
        buffer_info_create.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info_create.size = UNIFORM_SIZE;
        buffer_info_create.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        buffer_info_create.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo alloc_create_info;
        static const VmaAllocationCreateInfo EmptyVmaAllocationCreateInfo;
        alloc_create_info = EmptyVmaAllocationCreateInfo;
        alloc_create_info.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        alloc_create_info.preferredFlags = VK_MEMORY_PROPERTY_HOST_CACHED_BIT;

        VkResult err;
        err = vmaCreateBuffer(allocator, &buffer_info_create, &alloc_create_info,
                              &uniform_buffer, &uniform_buffer_allocation, NULL);
        assert((err == VK_SUCCESS) && "vkCreateBuffer: Failed uniform buffer.");
}

inline static void
setup_uniform_buffer()
{
        create_uniform_buffer();

        VkResult err;
        err = vmaMapMemory(allocator, uniform_buffer_allocation, (void **)&data_uniform_ptr);
        assert((err == VK_SUCCESS) && "vkMapMemory: Failed uniform buffer.");
}

inline static void create_descriptor_set_layout()
{
        VkDescriptorSetLayoutBinding set_layout_bindings[2];
        static const VkDescriptorSetLayoutBinding EmptyVkDescriptorSetLayoutBinding;
        set_layout_bindings[0] = EmptyVkDescriptorSetLayoutBinding;
        set_layout_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        set_layout_bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        set_layout_bindings[0].binding = 0;
        set_layout_bindings[0].descriptorCount = 1;

        set_layout_bindings[1] = EmptyVkDescriptorSetLayoutBinding;
        set_layout_bindings[1].descriptorType =
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        set_layout_bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        set_layout_bindings[1].binding = 1;
        set_layout_bindings[1].descriptorCount = 1;

        VkDescriptorSetLayoutCreateInfo set_layout_create_info;
        static const VkDescriptorSetLayoutCreateInfo
            EmptyVkDescriptorSetLayoutCreateInfo;
        set_layout_create_info = EmptyVkDescriptorSetLayoutCreateInfo;
        set_layout_create_info.sType =
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        set_layout_create_info.pBindings = set_layout_bindings;
        set_layout_create_info.bindingCount =
            sizeof(set_layout_bindings) / sizeof(set_layout_bindings[0]);

        VkResult err;
        err = vkCreateDescriptorSetLayout(device, &set_layout_create_info, NULL,
                                          &descriptor_set_layout);
        assert((err == VK_SUCCESS) && "vkCreateDescriptorSetLayout: Failed.");
}

inline static void create_descriptor_pool()
{
        VkDescriptorPoolSize pool_sizes[] = {
            {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
            {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
            {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

        VkDescriptorPoolCreateInfo pool_info;
        static const VkDescriptorPoolCreateInfo EmptyVkDescriptorPoolCreateInfo;
        pool_info = EmptyVkDescriptorPoolCreateInfo;
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.poolSizeCount = sizeof(pool_sizes) / sizeof(pool_sizes[0]);
        pool_info.pPoolSizes = pool_sizes;
        pool_info.maxSets = pool_sizes[0].descriptorCount * pool_info.poolSizeCount;

        VkResult err;
        err = vkCreateDescriptorPool(device, &pool_info, NULL, &descriptor_pool);
        assert((err == VK_SUCCESS) && "vkCreateDescriptorPool: Failed.");
}

inline static void allocate_descriptor_set(VkDescriptorSet *descriptor_set)
{
        VkDescriptorSetAllocateInfo alloc_info;
        static const VkDescriptorSetAllocateInfo EmptyVkDescriptorSetAllocateInfo;
        alloc_info = EmptyVkDescriptorSetAllocateInfo;
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool = descriptor_pool;
        alloc_info.pSetLayouts = &descriptor_set_layout;
        alloc_info.descriptorSetCount = 1;

        VkResult err;
        err = vkAllocateDescriptorSets(device, &alloc_info, descriptor_set);
        assert((err == VK_SUCCESS) && "vkAllocateDescriptorSets: Failed.");
}

inline static void update_descriptor_set(uniform_map_t *map)
{
        VkDescriptorBufferInfo uniform_descriptors[1];
        static const VkDescriptorBufferInfo EmptyVkDescriptorBufferInfo;
        uniform_descriptors[0] = EmptyVkDescriptorBufferInfo;
        uniform_descriptors[0].buffer = uniform_buffer;
        uniform_descriptors[0].offset = FROM_UNIFORM_ALLOC(map->offset);
        uniform_descriptors[0].range = FROM_UNIFORM_ALLOC(map->range);

        VkWriteDescriptorSet write_descriptor_sets[2];
        static const VkWriteDescriptorSet EmptyVkWriteDescriptorSet;
        write_descriptor_sets[0] = EmptyVkWriteDescriptorSet;
        write_descriptor_sets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_descriptor_sets[0].dstSet = map->descriptor_set;
        write_descriptor_sets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write_descriptor_sets[0].dstBinding = 0;
        write_descriptor_sets[0].pBufferInfo = uniform_descriptors;
        write_descriptor_sets[0].descriptorCount = 1;

        VkDescriptorImageInfo image_info;
        static const VkDescriptorImageInfo EmptyVkDescriptorImageInfo;
        image_info = EmptyVkDescriptorImageInfo;
        image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_info.imageView = texture_image_views[0];
        image_info.sampler = texture_image_sampler;

        write_descriptor_sets[1] = EmptyVkWriteDescriptorSet;
        write_descriptor_sets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_descriptor_sets[1].dstSet = map->descriptor_set;
        write_descriptor_sets[1].descriptorType =
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write_descriptor_sets[1].dstBinding = 1;
        write_descriptor_sets[1].pImageInfo = &image_info;
        write_descriptor_sets[1].descriptorCount = 1;

        vkUpdateDescriptorSets(
            device, sizeof(write_descriptor_sets) / sizeof(write_descriptor_sets[0]),
            write_descriptor_sets, 0, NULL);
}

void setup_model_instance(uniform_map_t *uniform_map)
{
        if (uniform_map->descriptor_set == VK_NULL_HANDLE)
                allocate_descriptor_set(&uniform_map->descriptor_set);
        update_descriptor_set(uniform_map);
}

inline static void create_pipeline_layout()
{
        VkPipelineLayoutCreateInfo pipeline_layout_create_info;
        static const VkPipelineLayoutCreateInfo EmptyVkPipelineLayoutCreateInfo;
        pipeline_layout_create_info = EmptyVkPipelineLayoutCreateInfo;
        pipeline_layout_create_info.sType =
            VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_create_info.setLayoutCount = 1;
        pipeline_layout_create_info.pSetLayouts = &descriptor_set_layout;

        VkResult err;
        err = vkCreatePipelineLayout(device, &pipeline_layout_create_info, NULL,
                                     &pipeline_layout);
        assert((err == VK_SUCCESS) && "vkCreatePipelineLayout: Failed.");
}

inline static void create_texture_image_sampler()
{
        VkSamplerCreateInfo sampler_info;
        static const VkSamplerCreateInfo EmptyVkSamplerCreateInfo;
        sampler_info = EmptyVkSamplerCreateInfo;
        sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_info.magFilter = VK_FILTER_LINEAR;
        sampler_info.minFilter = VK_FILTER_LINEAR;
        sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.anisotropyEnable = VK_FALSE;
        sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        sampler_info.unnormalizedCoordinates = VK_FALSE;
        sampler_info.compareEnable = VK_FALSE;
        sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
        sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler_info.mipLodBias = 0.0f;
        sampler_info.minLod = 0.0f;
        sampler_info.maxLod = 0.0f;

        VkResult err;
        err =
            vkCreateSampler(device, &sampler_info, NULL, &texture_image_sampler);
        assert((err == VK_SUCCESS) && "vkCreateSampler: Failed.");
}

inline static void setup_descriptor_set()
{
        create_descriptor_set_layout();
        create_descriptor_pool();
        create_pipeline_layout();
        create_texture_image_sampler();
}

inline static void create_vertex_buffer()
{
        VkBufferCreateInfo buffer_info_create;
        static const VkBufferCreateInfo EmptyVkBufferCreateInfo;
        buffer_info_create = EmptyVkBufferCreateInfo;
        buffer_info_create.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info_create.size = data_model_vertex_size;
        buffer_info_create.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        buffer_info_create.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo alloc_create_info;
        static const VmaAllocationCreateInfo EmptyVmaAllocationCreateInfo;
        alloc_create_info = EmptyVmaAllocationCreateInfo;
        alloc_create_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

        VkResult err;
        err = vmaCreateBuffer(allocator, &buffer_info_create, &alloc_create_info, &vertex_buffer, &vertex_buffer_allocation, NULL);
        assert((err == VK_SUCCESS) && "vmaCreateBuffer: Failed vertex buffer.");
}

inline static void
setup_vertex_buffer()
{
        create_vertex_buffer();

        void *data;
        VkResult err;
        err = vmaMapMemory(allocator, vertex_buffer_allocation, &data);
        assert((err == VK_SUCCESS) && "vmaMapMemory: Failed vertex buffer.");

        memcpy(data, data_model_vertex, data_model_vertex_size);

        vmaUnmapMemory(allocator, vertex_buffer_allocation);
        vmaFlushAllocation(allocator, vertex_buffer_allocation, 0, data_model_vertex_size);
}

inline static void create_index_buffer()
{
        VkBufferCreateInfo buffer_info_create;
        static const VkBufferCreateInfo EmptyVkBufferCreateInfo;
        buffer_info_create = EmptyVkBufferCreateInfo;
        buffer_info_create.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info_create.size = data_model_index_size;
        buffer_info_create.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        buffer_info_create.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo alloc_create_info;
        static const VmaAllocationCreateInfo EmptyVmaAllocationCreateInfo;
        alloc_create_info = EmptyVmaAllocationCreateInfo;
        alloc_create_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

        VkResult err;
        err = vmaCreateBuffer(allocator, &buffer_info_create, &alloc_create_info, &index_buffer, &index_buffer_allocation, NULL);
        assert((err == VK_SUCCESS) && "vmaCreateBuffer: Failed index buffer.");
}

inline static void
setup_index_buffer()
{
        create_index_buffer();

        void *data;
        VkResult err;
        err = vmaMapMemory(allocator, index_buffer_allocation, &data);
        assert((err == VK_SUCCESS) && "vmaMapMemory: Failed index buffer.");

        memcpy(data, data_model_index, data_model_index_size);

        vmaUnmapMemory(allocator, index_buffer_allocation);
        vmaFlushAllocation(allocator, index_buffer_allocation, 0, data_model_index_size);
}

inline static void create_texture_images(VkImage *texture_images, VmaAllocation *texture_image_allocations)
{
        VkImageCreateInfo image_create_info;
        static const VkImageCreateInfo EmptyVkImageViewCreateInfo;
        image_create_info = EmptyVkImageViewCreateInfo;
        image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_create_info.imageType = VK_IMAGE_TYPE_2D;
        image_create_info.format = data_materials[0].texture_format;
        image_create_info.extent.width = data_materials[0].texture_width;
        image_create_info.extent.height = data_materials[0].texture_height;
        image_create_info.extent.depth = 1;
        image_create_info.mipLevels = 1;
        image_create_info.arrayLayers = 1;
        image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_create_info.tiling = VK_IMAGE_TILING_LINEAR;
        image_create_info.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
        image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_create_info.queueFamilyIndexCount = VK_QUEUE_FAMILY_IGNORED;
        image_create_info.pQueueFamilyIndices = NULL;
        image_create_info.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;

        VmaAllocationCreateInfo alloc_create_info;
        static const VmaAllocationCreateInfo EmptyVmaAllocationCreateInfo;
        alloc_create_info = EmptyVmaAllocationCreateInfo;
        alloc_create_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

        VkResult err;
        err = vmaCreateImage(allocator, &image_create_info, &alloc_create_info, &texture_images[0], &texture_image_allocations[0], NULL);
        assert((err == VK_SUCCESS) && "vkCreateImage: Failed texture.");
}

inline static void
setup_texture_images(VkImage *texture_images, VmaAllocation *texture_image_allocations)
{
        create_texture_images(texture_images, texture_image_allocations);

        void *data;
        VkResult err;
        err = vmaMapMemory(allocator, texture_image_allocations[0], &data);
        assert((err == VK_SUCCESS) && "vmaMapMemory: Failed vertex buffer.");

        memcpy(data, data_materials[0].texture, data_materials[0].texture_size);

        vmaUnmapMemory(allocator, texture_image_allocations[0]);
        vmaFlushAllocation(allocator, texture_image_allocations[0], 0, data_materials[0].texture_size);
}

inline static void create_image_views(VkImage *texture_images)
{
        VkImageViewCreateInfo image_view_create_info;
        static const VkImageViewCreateInfo EmptyVkImageViewCreateInfo;
        image_view_create_info = EmptyVkImageViewCreateInfo;
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.image = texture_images[0];
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.format = data_materials[0].texture_format;
        image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.subresourceRange.aspectMask =
            VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_create_info.subresourceRange.baseMipLevel = 0;
        image_view_create_info.subresourceRange.levelCount = 1;
        image_view_create_info.subresourceRange.baseArrayLayer = 0;
        image_view_create_info.subresourceRange.layerCount = 1;

        VkResult err;
        err = vkCreateImageView(device, &image_view_create_info, NULL,
                                &texture_image_views[0]);
        assert((err == VK_SUCCESS) && "vkCreateImageView: Failed texture.");
}

inline static void
setup_textures(VkImage *texture_images,
               VmaAllocation *texture_image_allocations)
{
        setup_texture_images(texture_images, texture_image_allocations);
        create_image_views(texture_images);
}

inline static void create_shaders(VkShaderModule *vert_modules,
                                  VkShaderModule *frag_modules)
{
        VkShaderModuleCreateInfo shader_module_create_info;
        static const VkShaderModuleCreateInfo EmptyVkShaderModuleCreateInfo;
        shader_module_create_info = EmptyVkShaderModuleCreateInfo;
        shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

        shader_module_create_info.codeSize = *data_material_shaders[0].vert.size;
        shader_module_create_info.pCode =
            (uint32_t *)data_material_shaders[0].vert.spv;
        VkResult err;
        err = vkCreateShaderModule(device, &shader_module_create_info, NULL,
                                   &vert_modules[0]);
        assert((err == VK_SUCCESS) && "vkCreateShaderModule: Failed vert.");

        shader_module_create_info.codeSize = *data_material_shaders[0].frag.size;
        shader_module_create_info.pCode =
            (uint32_t *)data_material_shaders[0].frag.spv;
        err = vkCreateShaderModule(device, &shader_module_create_info, NULL,
                                   &frag_modules[0]);
        assert((err == VK_SUCCESS) && "vkCreateShaderModule: Failed frag.");
}

inline static void create_graphice_pipelines(VkShaderModule *vert_modules,
                                             VkShaderModule *frag_modules,
                                             VkPipeline *graphics_pipelines)
{
        VkPipelineShaderStageCreateInfo shader_stages_create_info[2];
        static const VkPipelineShaderStageCreateInfo
            EmptyVkPipelineShaderStageCreateInfo;
        shader_stages_create_info[0] = shader_stages_create_info[1] =
            EmptyVkPipelineShaderStageCreateInfo;
        shader_stages_create_info[0].sType = shader_stages_create_info[1].sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages_create_info[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shader_stages_create_info[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shader_stages_create_info[0].module = vert_modules[0];
        shader_stages_create_info[1].module = frag_modules[0];
        shader_stages_create_info[0].pName = shader_stages_create_info[1].pName =
            "main";

        VkPipelineVertexInputStateCreateInfo vertex_input_create_info;
        static const VkPipelineVertexInputStateCreateInfo
            EmptyVkPipelineVertexInputStateCreateInfo;
        vertex_input_create_info = EmptyVkPipelineVertexInputStateCreateInfo;
        vertex_input_create_info.sType =
            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_create_info.vertexBindingDescriptionCount = 1;
        vertex_input_create_info.pVertexBindingDescriptions =
            &data_binding_description;
        vertex_input_create_info.vertexAttributeDescriptionCount =
            data_attribute_descriptions_count;
        vertex_input_create_info.pVertexAttributeDescriptions =
            data_attribute_descriptions;

        VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info;
        static const VkPipelineInputAssemblyStateCreateInfo
            EmptyVkPipelineInputAssemblyStateCreateInfo;
        input_assembly_create_info = EmptyVkPipelineInputAssemblyStateCreateInfo;
        input_assembly_create_info.sType =
            VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        VkViewport viewport;
        static const VkViewport EmptyVkViewport;
        viewport = EmptyVkViewport;
        viewport = (VkViewport){.x = 0.0f,
                                .y = 0.0f,
                                .width = extent.width,
                                .height = extent.height,
                                .minDepth = 0.0f,
                                .maxDepth = 1.0f};

        VkRect2D scissor;
        static const VkRect2D EmptyVkRect2D;
        scissor = EmptyVkRect2D;
        scissor = (VkRect2D){.offset = {0, 0}, .extent = extent};

        VkPipelineViewportStateCreateInfo viewport_state_create_info;
        static const VkPipelineViewportStateCreateInfo
            EmptyVkPipelineViewportStateCreateInfo;
        viewport_state_create_info = EmptyVkPipelineViewportStateCreateInfo;
        viewport_state_create_info.sType =
            VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state_create_info.viewportCount = 1;
        viewport_state_create_info.pViewports = &viewport;
        viewport_state_create_info.scissorCount = 1;
        viewport_state_create_info.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterization_create_info;
        static const VkPipelineRasterizationStateCreateInfo
            EmptyVkPipelineRasterizationStateCreateInfo;
        rasterization_create_info = EmptyVkPipelineRasterizationStateCreateInfo;
        rasterization_create_info.sType =
            VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterization_create_info.depthClampEnable = VK_FALSE;
        rasterization_create_info.rasterizerDiscardEnable = VK_FALSE;
        rasterization_create_info.polygonMode = VK_POLYGON_MODE_FILL;
        rasterization_create_info.lineWidth = 1.0f;
        rasterization_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterization_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterization_create_info.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling_create_info;
        static const VkPipelineMultisampleStateCreateInfo
            EmptyVkPipelineMultisampleStateCreateInfo;
        multisampling_create_info = EmptyVkPipelineMultisampleStateCreateInfo;
        multisampling_create_info.sType =
            VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling_create_info.sampleShadingEnable = VK_FALSE;
        multisampling_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineDepthStencilStateCreateInfo depth_stencil_create_info;
        static const VkPipelineDepthStencilStateCreateInfo
            EmptyVkPipelineDepthStencilStateCreateInfo;
        depth_stencil_create_info = EmptyVkPipelineDepthStencilStateCreateInfo;
        depth_stencil_create_info.sType =
            VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depth_stencil_create_info.depthTestEnable = VK_TRUE;
        depth_stencil_create_info.depthWriteEnable = VK_TRUE;
        depth_stencil_create_info.depthCompareOp = VK_COMPARE_OP_LESS;
        depth_stencil_create_info.depthBoundsTestEnable = VK_FALSE;
        depth_stencil_create_info.minDepthBounds = 0.0f;
        depth_stencil_create_info.maxDepthBounds = 1.0f;
        depth_stencil_create_info.stencilTestEnable = VK_FALSE;

        VkPipelineColorBlendAttachmentState color_blend_attachment;
        static const VkPipelineColorBlendAttachmentState
            EmptyVkPipelineColorBlendAttachmentState;
        color_blend_attachment = EmptyVkPipelineColorBlendAttachmentState;
        color_blend_attachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo color_blending_create_info;
        static const VkPipelineColorBlendStateCreateInfo
            EmptyVkPipelineColorBlendStateCreateInfo;
        color_blending_create_info = EmptyVkPipelineColorBlendStateCreateInfo;
        color_blending_create_info.sType =
            VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blending_create_info.logicOpEnable = VK_FALSE;
        color_blending_create_info.attachmentCount = 1;
        color_blending_create_info.pAttachments = &color_blend_attachment;

        VkDynamicState dynamic_states[2] = {VK_DYNAMIC_STATE_VIEWPORT,
                                            VK_DYNAMIC_STATE_LINE_WIDTH};

        VkPipelineDynamicStateCreateInfo dynamic_state_create_info;
        static const VkPipelineDynamicStateCreateInfo
            EmptyVkPipelineDynamicStateCreateInfo;
        dynamic_state_create_info = EmptyVkPipelineDynamicStateCreateInfo;
        dynamic_state_create_info.sType =
            VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state_create_info.dynamicStateCount = 2;
        dynamic_state_create_info.pDynamicStates = dynamic_states;

        VkGraphicsPipelineCreateInfo pipeline_create_info;
        static const VkGraphicsPipelineCreateInfo EmptyVkGraphicsPipelineCreateInfo;
        pipeline_create_info = EmptyVkGraphicsPipelineCreateInfo;
        pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_create_info.stageCount = 2;
        pipeline_create_info.pStages = shader_stages_create_info;
        pipeline_create_info.pVertexInputState = &vertex_input_create_info;
        pipeline_create_info.pInputAssemblyState = &input_assembly_create_info;
        pipeline_create_info.pViewportState = &viewport_state_create_info;
        pipeline_create_info.pRasterizationState = &rasterization_create_info;
        pipeline_create_info.pMultisampleState = &multisampling_create_info;
        pipeline_create_info.pDepthStencilState = &depth_stencil_create_info;
        pipeline_create_info.pColorBlendState = &color_blending_create_info;
        pipeline_create_info.pDynamicState = &dynamic_state_create_info;
        pipeline_create_info.layout = pipeline_layout;
        pipeline_create_info.renderPass = render_pass;
        pipeline_create_info.subpass = 0;

        VkResult err;
        err = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1,
                                        &pipeline_create_info, NULL,
                                        &graphics_pipelines[0]);
        assert((err == VK_SUCCESS) && "vkCreateGraphicsPipelines: Failed.");
}

inline static void setup_materials(VkImage *texture_images,
                                   VmaAllocation *texture_image_allocations,
                                   VkShaderModule *vert_modules,
                                   VkShaderModule *frag_modules,
                                   VkPipeline *graphics_pipelines)
{
        setup_textures(texture_images, texture_image_allocations);
        create_shaders(vert_modules, frag_modules);
        create_graphice_pipelines(vert_modules, frag_modules, graphics_pipelines);
}

inline static void create_command_pool(uint32_t queue_family_index)
{
        VkCommandPoolCreateInfo pool_create_info;
        static const VkCommandPoolCreateInfo EmptyVkCommandPoolCreateInfo;
        pool_create_info = EmptyVkCommandPoolCreateInfo;
        pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_create_info.queueFamilyIndex = queue_family_index;
        pool_create_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |
                                 VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        VkResult err;
        err = vkCreateCommandPool(device, &pool_create_info, NULL, &command_pool);
        assert((err == VK_SUCCESS) && "vkCreateCommandPool: Failed.");
}

inline static void allocate_command_buffers(VkCommandBuffer *command_buffer)
{
        VkCommandBufferAllocateInfo command_buffer_allocate_info;
        static const VkCommandBufferAllocateInfo EmptyVkCommandBufferAllocateInfo;
        command_buffer_allocate_info = EmptyVkCommandBufferAllocateInfo;
        command_buffer_allocate_info.sType =
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        command_buffer_allocate_info.commandPool = command_pool;
        command_buffer_allocate_info.commandBufferCount = 1;
        command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

        VkResult err;
        err = vkAllocateCommandBuffers(device, &command_buffer_allocate_info,
                                       command_buffer);
        assert((err == VK_SUCCESS) && "vkAllocateCommandBuffers: Failed.");
}

inline static void create_presentation_semaphore()
{
        VkSemaphoreCreateInfo semaphore_create_info;
        static const VkSemaphoreCreateInfo EmptyVkSemaphoreCreateInfo;
        semaphore_create_info = EmptyVkSemaphoreCreateInfo;
        semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        vkCreateSemaphore(device, &semaphore_create_info, NULL, &wait_semaphore);
}

inline static void create_queue_submit_fence()
{
        VkFenceCreateInfo fence_create_info;
        static const VkFenceCreateInfo EmptyVkFenceCreateInfo;
        fence_create_info = EmptyVkFenceCreateInfo;
        fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

        VkResult err;
        err = vkCreateFence(device, &fence_create_info, NULL, &queue_submit_fence);
        assert((err == VK_SUCCESS) && "vkCreateFence: Failed.");
}

inline static void create_acquire_next_image_fence()
{
        VkFenceCreateInfo fence_create_info;
        static const VkFenceCreateInfo EmptyVkFenceCreateInfo;
        fence_create_info = EmptyVkFenceCreateInfo;
        fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

        VkResult err;
        err = vkCreateFence(device, &fence_create_info, NULL,
                            &acquire_next_image_fence);
        assert((err == VK_SUCCESS) && "vkCreateFence: Failed.");
}

inline static void
get_next_swapchain_image(VkQueue queue, uint32_t *active_swapchain_image_id)
{
        VkResult err;
        err = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, 0,
                                    acquire_next_image_fence,
                                    active_swapchain_image_id);
        assert((err == VK_SUCCESS) && "vkAcquireNextImageKHR: Failed.");
        err = vkWaitForFences(device, 1, &acquire_next_image_fence, VK_FALSE,
                              UINT64_MAX);
        assert((err == VK_SUCCESS) && "vkWaitForFences: Failed acquire next image.");
        err = vkQueueWaitIdle(queue);
        assert((err == VK_SUCCESS) && "vkQueueWaitIdle: Failed from render loop.");
}

inline static void begin_command_buffer(VkCommandBuffer command_buffer)
{
        VkCommandBufferBeginInfo command_buffer_begin_info;
        static const VkCommandBufferBeginInfo EmptyVkCommandBufferBeginInfo;
        command_buffer_begin_info = EmptyVkCommandBufferBeginInfo;
        command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VkResult err;
        err = vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);
        assert((err == VK_SUCCESS) && "vkBeginCommandBuffer: Failed.");
}

inline static void image_barriers(VkCommandBuffer command_buffer, VkImage *texture_images)
{
        VkImageMemoryBarrier barriers[1];
        static const VkImageMemoryBarrier EmptyVkImageMemoryBarrier;
        barriers[0] = EmptyVkImageMemoryBarrier;
        barriers[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barriers[0].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barriers[0].oldLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
        barriers[0].newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barriers[0].image = texture_images[0];
        barriers[0].subresourceRange = (VkImageSubresourceRange){VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

        vkCmdPipelineBarrier(
            command_buffer,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0,
            0, VK_NULL_HANDLE,
            0, VK_NULL_HANDLE,
            1, barriers);
}

inline static void begin_render_pass(VkCommandBuffer command_buffer,
                                     uint32_t active_swapchain_image_id)
{
        VkRect2D render_area;
        static const VkRect2D EmptyVkRect2D;
        render_area = EmptyVkRect2D;
        render_area.offset.x = 0;
        render_area.offset.y = 0;
        render_area.extent = extent;

        VkClearValue clear_values[2];
        static const VkClearValue EmptyVkClearValue;
        clear_values[0] = clear_values[1] = EmptyVkClearValue;
        clear_values[0].depthStencil.depth = 1.0f;
        clear_values[0].depthStencil.stencil = 0;
        clear_values[1].color.float32[0] = 1.0f;
        clear_values[1].color.float32[1] = 0.0f;
        clear_values[1].color.float32[2] = 0.0f;
        clear_values[1].color.float32[3] = 0.0f;

        VkRenderPassBeginInfo render_pass_begin_info;
        static const VkRenderPassBeginInfo EmptyVkRenderPassBeginInfo;
        render_pass_begin_info = EmptyVkRenderPassBeginInfo;
        render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_begin_info.renderPass = render_pass;
        render_pass_begin_info.framebuffer = framebuffers[active_swapchain_image_id];
        render_pass_begin_info.renderArea = render_area;
        render_pass_begin_info.clearValueCount = 2;
        render_pass_begin_info.pClearValues = clear_values;

        vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info,
                             VK_SUBPASS_CONTENTS_INLINE);
}

inline static void set_viewport(VkCommandBuffer command_buffer)
{
        VkViewport viewport;
        static const VkViewport EmptyVkViewport;
        viewport = EmptyVkViewport;
        viewport.maxDepth = 1.0f;
        viewport.minDepth = 0.0f;
        viewport.width = extent.width;
        viewport.height = extent.height;
        viewport.x = 0;
        viewport.y = 0;

        vkCmdSetViewport(command_buffer, 0, 1, &viewport);
}

inline static void record_commands(VkCommandBuffer command_buffer, VkImage *texture_images,
                                   uint32_t active_swapchain_image_id,
                                   VkPipeline *graphics_pipelines)
{
        begin_command_buffer(command_buffer);

        static bool init = true;
        if (init)
                image_barriers(command_buffer, texture_images);
        init = false;

        begin_render_pass(command_buffer, active_swapchain_image_id);
        set_viewport(command_buffer);

        static const VkDeviceSize ZeroVkDeviceSize = 0;
        vkCmdBindVertexBuffers(command_buffer, 0, 1, &vertex_buffer,
                               &ZeroVkDeviceSize);

        vkCmdBindIndexBuffer(command_buffer, index_buffer, 0, VK_INDEX_TYPE_UINT32);

        data_model_instance_t *ptr =
            list_entry(data_model_instances.next, data_model_instance_t, list);
        vkCmdBindPipeline(
            command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
            graphics_pipelines[data_materials[data_models[ptr->model].material]
                                   .shader]);

        vkCmdBindDescriptorSets(
            command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout, 0, 1,
            &ptr->uniform_map->descriptor_set,
            0, NULL);

        vkCmdDrawIndexed(command_buffer, data_models[ptr->model].count, 1,
                         data_models[ptr->model].first, 0, 0);

        vkCmdEndRenderPass(command_buffer);

        VkResult err;
        err = vkEndCommandBuffer(command_buffer);
        assert((err == VK_SUCCESS) && "vkEndCommandBuffer: Failed.");
}

inline static void submit_queue(VkQueue queue, VkCommandBuffer command_buffer)
{
        VkSubmitInfo submit_info;
        static const VkSubmitInfo EmptyVkSubmitInfo;
        submit_info = EmptyVkSubmitInfo;
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &command_buffer;
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = &wait_semaphore;

        VkResult err;
        err = vkQueueSubmit(queue, 1, &submit_info, queue_submit_fence);
        assert((err == VK_SUCCESS) && "vkQueueSubmit: Failed.");
}

inline static void fences()
{
        VkResult err;
        err = vkWaitForFences(device, 1, &queue_submit_fence, VK_FALSE, UINT64_MAX);
        assert((err == VK_SUCCESS) && "vkWaitForFences: Failed queue submit.");

        err = vkResetFences(
            device, 2, (VkFence[]){acquire_next_image_fence, queue_submit_fence});
        assert((err == VK_SUCCESS) && "vkResetFences: Failed.");
}

inline static void present_image(VkQueue queue,
                                 uint32_t *active_swapchain_image_id)
{
        VkResult present_results[1];
        VkPresentInfoKHR present_info;
        static const VkPresentInfoKHR EmptyPresentInfoKHR;
        present_info = EmptyPresentInfoKHR;
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = &wait_semaphore;
        present_info.swapchainCount = 1;
        present_info.pSwapchains = &swapchain;
        present_info.pImageIndices = active_swapchain_image_id;
        present_info.pResults = present_results;

        VkResult err;
        err = vkQueuePresentKHR(queue, &present_info);
        assert((err == VK_SUCCESS) && "vkQueuePresentKHR: Failed.");
        assert((present_results[0] == VK_SUCCESS) &&
               "vkQueuePresentKHR results: Failed.");
}

inline static void blocking_render_loop(uint32_t queue_family_index,
                                        VkImage *texture_images,
                                        VkPipeline *graphics_pipelines)
{
        VkCommandBuffer command_buffer;
        allocate_command_buffers(&command_buffer);
        create_presentation_semaphore();
        create_queue_submit_fence();
        create_acquire_next_image_fence();

        VkQueue queue;
        vkGetDeviceQueue(device, queue_family_index, VK_NULL_HANDLE, &queue);

        while (!glfwWindowShouldClose(data_window))
        {
                glfwPollEvents();

                data_update();

                uint32_t active_swapchain_image_id;
                get_next_swapchain_image(queue, &active_swapchain_image_id);

                VkResult err;
                err = vkResetCommandPool(device, command_pool, 0);
                assert((err == VK_SUCCESS) && "vkResetCommandPool: Failed.");
                record_commands(command_buffer, texture_images,
                                active_swapchain_image_id,
                                graphics_pipelines);
                submit_queue(queue, command_buffer);
                fences();
                present_image(queue, &active_swapchain_image_id);

                err = vkQueueWaitIdle(queue);
                assert((err == VK_SUCCESS) && "vkQueueWaitIdle: Failed from bottom half.");
        }
        vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
}

inline static void
clean_up(VkImage *texture_images, VmaAllocation *texture_image_allocations,
         VkPipeline *graphics_pipelines, VkShaderModule *frag_modules,
         VkShaderModule *vert_modules, uint32_t swapchain_image_count)
{
        vkDestroyFence(device, acquire_next_image_fence, NULL);
        acquire_next_image_fence = VK_NULL_HANDLE;
        vkDestroyFence(device, queue_submit_fence, NULL);
        queue_submit_fence = VK_NULL_HANDLE;
        vkDestroySemaphore(device, wait_semaphore, NULL);
        wait_semaphore = VK_NULL_HANDLE;
        vkDestroyCommandPool(device, command_pool, NULL);
        command_pool = VK_NULL_HANDLE;
        vkDestroyPipeline(device, graphics_pipelines[0], NULL);
        graphics_pipelines[0] = VK_NULL_HANDLE;
        vkDestroyShaderModule(device, frag_modules[0], NULL);
        frag_modules[0] = VK_NULL_HANDLE;
        vkDestroyShaderModule(device, vert_modules[0], NULL);
        vert_modules[0] = VK_NULL_HANDLE;
        vkDestroyPipelineLayout(device, pipeline_layout, NULL);
        pipeline_layout = VK_NULL_HANDLE;
        vmaDestroyBuffer(allocator, index_buffer, index_buffer_allocation);
        index_buffer_allocation = NULL;
        index_buffer = VK_NULL_HANDLE;
        vmaDestroyBuffer(allocator, vertex_buffer, vertex_buffer_allocation);
        vertex_buffer_allocation = NULL;
        vertex_buffer = VK_NULL_HANDLE;
        vkDestroyPipelineLayout(device, pipeline_layout, NULL);
        pipeline_layout = VK_NULL_HANDLE;
        vkDestroyDescriptorPool(device, descriptor_pool, NULL);
        descriptor_pool = VK_NULL_HANDLE;
        vkDestroyDescriptorSetLayout(device, descriptor_set_layout, NULL);
        descriptor_set_layout = VK_NULL_HANDLE;
        vkDestroySampler(device, texture_image_sampler, NULL);
        texture_image_sampler = VK_NULL_HANDLE;
        vkDestroyImageView(device, texture_image_views[0], NULL);
        texture_image_views[0] = VK_NULL_HANDLE;
        vmaDestroyImage(allocator, texture_images[0], texture_image_allocations[0]);
        texture_image_allocations[0] = VK_NULL_HANDLE;
        texture_images[0] = VK_NULL_HANDLE;
        vmaUnmapMemory(allocator, uniform_buffer_allocation);
        data_model_instance_t *ptr;
        list_for_each_entry(ptr, &data_model_instances, list)
        {
                ptr->ubo = NULL;
                ptr->uniform_map->descriptor_set = VK_NULL_HANDLE;
        }
        vmaDestroyBuffer(allocator, uniform_buffer, uniform_buffer_allocation);
        uniform_buffer_allocation = NULL;
        uniform_buffer = VK_NULL_HANDLE;
        for (size_t i = 0; i < swapchain_image_count; i++)
        {
                vkDestroyFramebuffer(device, framebuffers[i], NULL);
                framebuffers[i] = VK_NULL_HANDLE;
                vkDestroyImageView(device, swapchain_image_views[i], NULL);
                swapchain_image_views[i] = VK_NULL_HANDLE;
        }
        vkDestroyRenderPass(device, render_pass, NULL);
        render_pass = VK_NULL_HANDLE;
        vkDestroyImageView(device, depth_stencil_image_view, NULL);
        depth_stencil_image_view = VK_NULL_HANDLE;
        vmaDestroyImage(allocator, depth_stencil_image, depth_stencil_image_allocation);
        depth_stencil_image_allocation = NULL;
        depth_stencil_image = VK_NULL_HANDLE;
        vmaDestroyAllocator(allocator);
        allocator = NULL;
        vkDestroyImage(device, depth_stencil_image, NULL);
        depth_stencil_image = VK_NULL_HANDLE;
        vkDestroySwapchainKHR(device, swapchain, NULL);
        swapchain = VK_NULL_HANDLE;
        vkDestroyDevice(device, NULL);
        device = VK_NULL_HANDLE;
        vkDestroySurfaceKHR(instance, surface, NULL);
        surface = VK_NULL_HANDLE;
        vkDestroyInstance(instance, NULL);
        instance = VK_NULL_HANDLE;

        glfwDestroyWindow(data_window);

        glfwTerminate();
}

int main(int argc, char *argv[])
{
        init_create_window();
        VkPhysicalDevice gpu = VK_NULL_HANDLE;
        find_device_surface(&gpu);

        uint32_t queue_family_index;
        create_device_queu_family(gpu, &queue_family_index);

        VkSurfaceFormatKHR surface_format;
        discover_surface_format(gpu, &surface_format);
        uint32_t swapchain_image_count = 2;
        setup_swapchain(gpu, surface_format, &swapchain_image_count);

        create_vma(gpu);

        VkFormat depth_stencil_format;
        setup_depth_stencil_image(gpu, &depth_stencil_format);
        create_render_pass(depth_stencil_format, surface_format);

        create_framebuffers(swapchain_image_count);

        setup_uniform_buffer();
        setup_descriptor_set();

        setup_vertex_buffer();
        setup_index_buffer();

        VkImage texture_images[] = {VK_NULL_HANDLE};
        VmaAllocation texture_image_allocations[] = {VK_NULL_HANDLE};
        VkImageView _texture_image_views[] = {VK_NULL_HANDLE};
        texture_image_views = _texture_image_views;
        VkShaderModule vert_modules[] = {VK_NULL_HANDLE};
        VkShaderModule frag_modules[] = {VK_NULL_HANDLE};
        VkPipeline graphics_pipelines[] = {VK_NULL_HANDLE};
        setup_materials(texture_images, texture_image_allocations, vert_modules, frag_modules, graphics_pipelines);

        data_setup_model_instance = setup_model_instance;
        data_init(extent);

        create_command_pool(queue_family_index);
        blocking_render_loop(queue_family_index, texture_images, graphics_pipelines);

        clean_up(texture_images, texture_image_allocations, graphics_pipelines,
                 frag_modules, vert_modules, swapchain_image_count);
        exit(0);
}
