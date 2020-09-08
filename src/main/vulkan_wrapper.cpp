#include "vulkan_wrapper.hpp"

#include <memory>
#include <vulkan/vulkan.h>
#include <optional>
#include <set>

namespace vulkan_wrapper {
    namespace {
        const int MAX_FRAMES_IN_FLIGHT = 3;
        void (*resolution_function)(int*, int*);
        struct Command {
            vk::CommandPool pool;
            std::vector<vk::CommandBuffer> buffers;
        };
        struct info {
            vk::Instance instance;
            vk::SurfaceKHR surface;

            vk::PhysicalDevice physical_device;
            vk::Device device;

            vk::Queue graphics_queue;
            vk::Queue present_queue;
            uint32_t graphics_id;
            uint32_t present_id;

            vk::SwapchainKHR swapchain;
            std::vector<vk::Image> swapchain_images;
            vk::Format swapchain_image_format = vk::Format::eB8G8R8A8Unorm;
            vk::Extent2D swapchain_extent;
            std::vector<vk::ImageView> swapchain_image_views;
            std::vector<vk::Framebuffer> swapchain_framebuffers;
            vk::RenderPass render_pass;

            std::vector<Command> commands;
            std::vector<vk::Semaphore> image_available_semaphores;
            std::vector<vk::Semaphore> render_finished_semaphores;
            std::vector<vk::Fence> in_flight_fences;
            std::vector<vk::Fence> images_in_flight;

            size_t current_frame = 0;
            bool draw = false;

            vk::DispatchLoaderDynamic dldi;
#ifdef DEBUG_MODE
            vk::DebugUtilsMessengerEXT debugMessenger;
#endif
        };
        std::unique_ptr<info> info_p;

#ifdef DEBUG_MODE
        VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
            printf("%s\n", pCallbackData->pMessage);
            return VK_FALSE;
        }
        vk::Result createDebugUtilsMessengerEXT() {
            vk::DebugUtilsMessengerCreateInfoEXT createInfo = {vk::DebugUtilsMessengerCreateFlagsEXT(),
                                                               vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
                                                               vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
                                                               debugCallback};
            return info_p->instance.createDebugUtilsMessengerEXT(&createInfo, nullptr, &info_p->debugMessenger, info_p->dldi);
        }
        void destroyDebugUtilsMessengerEXT() {
            info_p->instance.destroyDebugUtilsMessengerEXT(info_p->debugMessenger, nullptr, info_p->dldi);
        }
#endif
        struct queue_family_indices {
            std::optional<uint32_t> graphics_family;
            std::optional<uint32_t> present_family;
            bool is_complete() {
                return graphics_family.has_value() && present_family.has_value();
            }
        };
        queue_family_indices find_queue_families(vk::PhysicalDevice physical_device) {
            queue_family_indices indices;
            std::vector<vk::QueueFamilyProperties> queue_families = physical_device.getQueueFamilyProperties();

            uint32_t i = 0;
            for (const vk::QueueFamilyProperties& properties : queue_families) {
                if (properties.queueCount > 0) {
                    if (properties.queueFlags & vk::QueueFlagBits::eGraphics) {
                        indices.graphics_family = i;
                    }
                    if (physical_device.getSurfaceSupportKHR(i, info_p->surface)) {
                        indices.present_family = i;
                    }
                    if (indices.is_complete()) {
                        break;
                    }
                }
                i++;
            }
            return indices;
        }
        bool check_device_extension_support(vk::PhysicalDevice pd) {
            std::vector<vk::ExtensionProperties> extension_properties = pd.enumerateDeviceExtensionProperties(nullptr);

            std::set<std::string> device_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

            for (vk::ExtensionProperties properties : extension_properties) {
                device_extensions.erase(properties.extensionName);
            }
            return device_extensions.empty();
        }
        struct swapchain_support_details {
            vk::SurfaceCapabilitiesKHR capabilities;
            std::vector<vk::SurfaceFormatKHR> formats;
            std::vector<vk::PresentModeKHR> present_modes;
        };

        swapchain_support_details query_swapchain_support(vk::PhysicalDevice device) {
            swapchain_support_details details;

            details.capabilities = device.getSurfaceCapabilitiesKHR(info_p->surface);
            details.formats = device.getSurfaceFormatsKHR(info_p->surface);
            details.present_modes = device.getSurfacePresentModesKHR(info_p->surface);

            return details;
        }
        vk::SurfaceFormatKHR choose_swapchain_surface_format(const std::vector<vk::SurfaceFormatKHR>& available_formats) {
            if (available_formats.size() == 1 && available_formats[0].format == vk::Format::eUndefined) {
                return {vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eVkColorspaceSrgbNonlinear};
            }

            for (const auto& available_format : available_formats) {
                if (available_format.format == vk::Format::eB8G8R8A8Unorm && available_format.colorSpace == vk::ColorSpaceKHR::eVkColorspaceSrgbNonlinear) {
                    return available_format;
                }
            }
            return available_formats[0];
        }
        vk::PresentModeKHR choose_swapchain_present_mode(const std::vector<vk::PresentModeKHR>& availableModes) {
            vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;

            for (const vk::PresentModeKHR mode : availableModes) {
                if (mode == vk::PresentModeKHR::eMailbox) {
                    return mode;
                }
                if (mode == vk::PresentModeKHR::eImmediate) {
                    bestMode = mode;
                }
            }
            return bestMode;
        }
        vk::Extent2D choose_swapchain_extent(const vk::SurfaceCapabilitiesKHR& capabilities) {
            if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
                return capabilities.currentExtent;
            }
            int width, height;
            resolution_function(&width, &height);

            vk::Extent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

            actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
        bool is_device_suitable(vk::PhysicalDevice physcial_device) {
            queue_family_indices indices = find_queue_families(physcial_device);

            bool extensions_supported = check_device_extension_support(physcial_device);

            bool swapchain_adequate = false;
            if (extensions_supported) {
                swapchain_support_details swapchain_support = query_swapchain_support(physcial_device);
                swapchain_adequate = !swapchain_support.formats.empty() && !swapchain_support.present_modes.empty();
            }

            return indices.is_complete() && extensions_supported && swapchain_adequate;
        }
    }
    bool create_instance(std::vector<const char*> extensions) {
        if (info_p) {
            return false;
        }
        info_p = std::make_unique<info>();

        const std::vector<const char*> validation_layers = {"VK_LAYER_LUNARG_standard_validation"};

#ifdef DEBUG_MODE
        std::vector<vk::LayerProperties> layerProperties = vk::enumerateInstanceLayerProperties();
        for (const char* layerName : validationLayers) {
            bool found = false;
            for (const vk::LayerProperties& props : layerProperties) {
                if (strcmp(layerName, props.layerName) == 0) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                printf("Could not find validation layer");
                return false;
            }
        }
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
        vk::ApplicationInfo app_info = {"InViculum", 0x00400000/* 0000000000,0000000000,000000000000 */, "No Engine", 0x00400000, 0x00401000};
        vk::InstanceCreateInfo create_info = {vk::InstanceCreateFlags(), &app_info,
#ifdef DEBUG_MODE
        static_cast<uint32_t>(validation_layers.size()), validation_layers.data(),
#else
            0, nullptr,
#endif
            static_cast<uint32_t>(extensions.size()), extensions.data()};

        info_p->instance = vk::createInstance(create_info);
        info_p->dldi = vk::DispatchLoaderDynamic(info_p->instance);
#ifdef DEBUG_MODE
        create_debug_utils_messenger_EXT();
#endif
        return true;
    }
    bool create_surface(bool(*fn)(const vk::Instance&, vk::SurfaceKHR&), void (*r)(int*, int*)) {
        if (info_p) {
            resolution_function = r;
            return fn(info_p->instance, info_p->surface);
        }
        return false;
    }

    bool create_others() {
        /////////////////////////
        //// PHYSICAL DEVICE ////
        /////////////////////////
        std::vector<vk::PhysicalDevice> physcial_devices = info_p->instance.enumeratePhysicalDevices();
        for (const vk::PhysicalDevice& d : physcial_devices) {
            if (is_device_suitable(d)) {
                info_p->physical_device = d;
                break;
            }
        }
        if (!info_p->physical_device) {
            return false;
        }

        ////////////////
        //// DEVICE ////
        ////////////////
        queue_family_indices indices = find_queue_families(info_p->physical_device);

        std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
        std::set<uint32_t> unique_queue_families = {indices.graphics_family.value(), indices.present_family.value()};

        float queue_priority = 1.0f;
        for (uint32_t queue_family : unique_queue_families) {
            vk::DeviceQueueCreateInfo queue_create_info = {vk::DeviceQueueCreateFlags(), queue_family, 1, &queue_priority};
                queue_create_infos.push_back(queue_create_info);
        }

        vk::PhysicalDeviceFeatures device_features = {};
        const std::vector<const char*> device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
#ifdef DEBUG_MODE
        const std::vector<const char*> validation_layers = {"VK_LAYER_LUNARG_standard_validation"};
#endif

        vk::DeviceCreateInfo device_create_info = {vk::DeviceCreateFlags(), static_cast<uint32_t>(queue_create_infos.size()), queue_create_infos.data(),
            
#ifdef  DEBUG_MODE
            static_cast<uint32_t>(validation_layers.size()), validationLayers.data(),
#else
            0, nullptr,
#endif
                                                 static_cast<uint32_t>(device_extensions.size()), device_extensions.data(), &device_features};

        info_p->device = info_p->physical_device.createDevice(device_create_info);

        info_p->graphics_queue = info_p->device.getQueue(indices.graphics_family.value(), 0);
        info_p->present_queue = info_p->device.getQueue(indices.present_family.value(), 0);
        info_p->graphics_id = indices.graphics_family.value();
        info_p->present_id = indices.present_family.value();

        ///////////////////////
        //// COMMAND POOLS ////
        ///////////////////////
        info_p->commands.resize(MAX_FRAMES_IN_FLIGHT);
        vk::CommandPoolCreateInfo command_pool_create_info = {vk::CommandPoolCreateFlags(), indices.graphics_family.value()};
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            info_p->commands[i].pool = info_p->device.createCommandPool(command_pool_create_info);
        }

        /////////////////////////
        //// COMMAND BUFFERS ////
        /////////////////////////
        for (Command& cmd : info_p->commands) {
            cmd.buffers.resize(1);
            vk::CommandBufferAllocateInfo command_buffer_allocate_info = {cmd.pool, vk::CommandBufferLevel::ePrimary, 1};
            cmd.buffers = info_p->device.allocateCommandBuffers(command_buffer_allocate_info);
        }

        //////////////////////
        //// SYNC OBJECTS ////
        //////////////////////
        info_p->image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
        info_p->render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
        info_p->in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);

        vk::SemaphoreCreateInfo semaphore_create_info = {vk::SemaphoreCreateFlags()};
        vk::FenceCreateInfo fence_create_info = {vk::FenceCreateFlagBits::eSignaled};
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            info_p->image_available_semaphores[i] = info_p->device.createSemaphore(semaphore_create_info);
            info_p->render_finished_semaphores[i] = info_p->device.createSemaphore(semaphore_create_info);
            info_p->in_flight_fences[i] = info_p->device.createFence(fence_create_info);
        }
        return create_swapchain();
    }

    bool create_swapchain() {
        info_p->device.waitIdle();
        ///////////////////
        //// SWAPCHAIN ////
        ///////////////////
        swapchain_support_details swapchain_support = query_swapchain_support(info_p->physical_device);

        vk::SurfaceFormatKHR surface_format = choose_swapchain_surface_format(swapchain_support.formats);
        vk::PresentModeKHR present_mode = choose_swapchain_present_mode(swapchain_support.present_modes);
        vk::Extent2D extent = choose_swapchain_extent(swapchain_support.capabilities);

        uint32_t image_count = swapchain_support.capabilities.minImageCount + 1;
        if (swapchain_support.capabilities.maxImageCount > 0 && image_count > swapchain_support.capabilities.maxImageCount) {
            image_count = swapchain_support.capabilities.maxImageCount;
        }

        queue_family_indices indices = find_queue_families(info_p->physical_device);
        uint32_t  queue_family_indices[] = {indices.graphics_family.value(), indices.present_family.value()};
        bool queue_different = indices.graphics_family != indices.present_family;

        vk::SwapchainCreateInfoKHR swapchain_create_info = {vk::SwapchainCreateFlagsKHR(), info_p->surface, image_count, surface_format.format,
                                                            surface_format.colorSpace, extent, 1, vk::ImageUsageFlagBits::eColorAttachment,
                                                            queue_different ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive,
                                                            queue_different ? 2U : 0U, queue_different ? queue_family_indices : nullptr,
                                                            swapchain_support.capabilities.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque,
                                                            present_mode, VK_TRUE};

        info_p->swapchain = info_p->device.createSwapchainKHR(swapchain_create_info);

        info_p->swapchain_images = info_p->device.getSwapchainImagesKHR(info_p->swapchain);
        info_p->swapchain_image_format = surface_format.format;
        info_p->swapchain_extent = extent;

        /////////////////////
        //// IMAGE VIEWS ////
        /////////////////////
        info_p->swapchain_image_views.resize(info_p->swapchain_images.size());
        for (uint32_t i = 0; i < info_p->swapchain_images.size(); i++) {
            vk::ImageViewCreateInfo image_view_create_info = {vk::ImageViewCreateFlags(), info_p->swapchain_images[i], vk::ImageViewType::e2D, info_p->swapchain_image_format,
                                                            {vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity},
                                                            {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}};


            info_p->swapchain_image_views[i] = info_p->device.createImageView(image_view_create_info);
        }

        /////////////////////
        //// RENDER PASS ////
        /////////////////////
        vk::AttachmentDescription attachment_description = {vk::AttachmentDescriptionFlags(), info_p->swapchain_image_format, vk::SampleCountFlagBits::e1,
                                                            vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare,
                                                            vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR};

        vk::AttachmentReference attachment_reference = {0, vk::ImageLayout::eColorAttachmentOptimal};

        vk::SubpassDescription subpass_description = {vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics, 0, nullptr, 1, &attachment_reference, nullptr, nullptr, 0, nullptr};

        vk::SubpassDependency subpass_dependency = {~0U, 0, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::AccessFlags(),
                                                    vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite, vk::DependencyFlags()};

        vk::RenderPassCreateInfo render_pass_create_info = {vk::RenderPassCreateFlags(), 1, &attachment_description, 1, &subpass_description, 1, &subpass_dependency};

        info_p->render_pass = info_p->device.createRenderPass(render_pass_create_info);

        //////////////////////
        //// FRAMEBUFFERS ////
        //////////////////////
        info_p->swapchain_framebuffers.resize(info_p->swapchain_image_views.size());

        for (uint32_t i = 0; i < info_p->swapchain_framebuffers.size(); i++) {
            vk::FramebufferCreateInfo framebuffer_create_info = {vk::FramebufferCreateFlags(), info_p->render_pass, 1, &info_p->swapchain_image_views[i],
                                                                info_p->swapchain_extent.width, info_p->swapchain_extent.height, 1};

            info_p->swapchain_framebuffers[i] = info_p->device.createFramebuffer(framebuffer_create_info);
        }
        info_p->images_in_flight.resize(info_p->swapchain_images.size(), vk::Fence());
        return true;
    }

    bool create_vertex_buffer(vk::Buffer& buffer, vk::DeviceMemory& memory, uint32_t size) {
        vk::BufferCreateInfo buffer_create_info = {vk::BufferCreateFlags(), size, vk::BufferUsageFlagBits::eVertexBuffer, vk::SharingMode::eExclusive, 1, &info_p->graphics_id};
        if (!(buffer = info_p->device.createBuffer(buffer_create_info))) {
            return false;
        }
        vk::MemoryRequirements memory_requirements = info_p->device.getBufferMemoryRequirements(buffer);
        vk::PhysicalDeviceMemoryProperties physcial_device_memory_properties = info_p->physical_device.getMemoryProperties();

        uint32_t chosen = std::numeric_limits<uint32_t>::max();
        for (uint32_t i = 0; i < physcial_device_memory_properties.memoryTypeCount; i++) {
            if ((memory_requirements.memoryTypeBits & (1u << i)) && (physcial_device_memory_properties.memoryTypes[i].propertyFlags & (vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent))) {
                chosen = i;
            }
        }
        if (chosen == std::numeric_limits<uint32_t>::max()) {
            info_p->device.destroyBuffer(buffer);
            return false;
        }
        vk::MemoryAllocateInfo memory_allocate_info = {memory_requirements.size, chosen};
        if (!(memory = info_p->device.allocateMemory(memory_allocate_info))) {
            info_p->device.destroyBuffer(buffer);
            return false;
        }
        info_p->device.bindBufferMemory(buffer, memory, 0);
        return true;
    }
    void map_vertex_buffer(const vk::DeviceMemory& memory, uint32_t size, const void* data) {
        void* mapped_memory = info_p->device.mapMemory(memory, 0, size);
        memcpy(mapped_memory, data, size);
        info_p->device.unmapMemory(memory);
    }
    void destroy_vertex_buffer(const vk::Buffer& buffer, const vk::DeviceMemory& memory) {
        info_p->device.destroyBuffer(buffer);
        info_p->device.freeMemory(memory);
    }

    bool create_shader_module(vk::ShaderModule& shader_module, const std::vector<uint8_t>& src) {
        vk::ShaderModuleCreateInfo shader_module_create_info = {vk::ShaderModuleCreateFlags(), src.size(), reinterpret_cast<const uint32_t*>(src.data())};
        shader_module = info_p->device.createShaderModule(shader_module_create_info);
        return true;
    }
    void destroy_shader_module(const vk::ShaderModule& shader_module) {
        info_p->device.destroyShaderModule(shader_module);
    }

    bool create_pipeline_layout(vk::PipelineLayout& pipeline_layout, const vk::PipelineLayoutCreateInfo& pipeline_layout_create_info) {
        pipeline_layout = info_p->device.createPipelineLayout(pipeline_layout_create_info);
        return !!pipeline_layout;
    }
    void destroy_pipeline_layout(const vk::PipelineLayout& pipeline_layout) {
        info_p->device.destroyPipelineLayout(pipeline_layout);
    }
    bool create_pipeline(vk::Pipeline& pipeline, const vk::PipelineLayout& pipeline_layout, uint32_t shader_module_count, const vk::PipelineShaderStageCreateInfo* shader_modules, uint32_t vertex_binding_description_count, const vk::VertexInputBindingDescription* vertex_binding_descriptions, uint32_t vertex_attribute_description_count, const vk::VertexInputAttributeDescription* vertex_attribute_descriptions, float target_aspect) {
        vk::PipelineVertexInputStateCreateInfo pipeline_vertex_input_state_create_info = {vk::PipelineVertexInputStateCreateFlags(), vertex_binding_description_count, vertex_binding_descriptions, vertex_attribute_description_count, vertex_attribute_descriptions};
        vk::PipelineInputAssemblyStateCreateInfo pipeline_assembly_state_create_info = {vk::PipelineInputAssemblyStateCreateFlags(), vk::PrimitiveTopology::eTriangleList, VK_FALSE};

        float width = info_p->swapchain_extent.width;
        float height = info_p->swapchain_extent.height;
        vk::Viewport viewport = {0.0f, 0.0f, width, height, 0.0f, 1.0f};

        float swapchain_aspect = width / height;
        if (swapchain_aspect > target_aspect) {
            float targetW = width * target_aspect / swapchain_aspect;
            viewport.x = (width - targetW) / 2.0f;
            viewport.width = targetW;
        }
        else {
            float targetH = height * swapchain_aspect / target_aspect;
            viewport.y = (height - targetH) / 2.0f;
            viewport.height = targetH;
        }
        vk::Rect2D scissor = {{0, 0}, info_p->swapchain_extent};
        vk::PipelineViewportStateCreateInfo pipeline_viewport_state_create_info = {vk::PipelineViewportStateCreateFlags(), 1, &viewport, 1, &scissor};
        vk::PipelineRasterizationStateCreateInfo pipeline_rasterization_state_create_info = {vk::PipelineRasterizationStateCreateFlags(), VK_FALSE, VK_FALSE, vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone, vk::FrontFace::eClockwise, VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f};
        vk::PipelineMultisampleStateCreateInfo pipeline_multisample_state_create_info = {vk::PipelineMultisampleStateCreateFlags(), vk::SampleCountFlagBits::e1, VK_FALSE, 1.0f, nullptr, VK_FALSE, VK_FALSE};
        vk::PipelineColorBlendAttachmentState pipeline_color_blend_attachment_state = {VK_TRUE, vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendOp::eAdd, vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd, vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA};
        vk::PipelineColorBlendStateCreateInfo pipeline_color_blend_state_create_info = {vk::PipelineColorBlendStateCreateFlags(), VK_FALSE, vk::LogicOp::eCopy, 1, &pipeline_color_blend_attachment_state, {0.0f, 0.0f, 0.0f, 0.0f}};

        vk::GraphicsPipelineCreateInfo graphics_pipeline_create_info = {vk::PipelineCreateFlags(), shader_module_count, shader_modules, &pipeline_vertex_input_state_create_info, &pipeline_assembly_state_create_info, nullptr, &pipeline_viewport_state_create_info, &pipeline_rasterization_state_create_info, &pipeline_multisample_state_create_info, nullptr, &pipeline_color_blend_state_create_info, nullptr, pipeline_layout, info_p->render_pass, 0, vk::Pipeline(), -1};
        pipeline = info_p->device.createGraphicsPipeline(vk::PipelineCache(), graphics_pipeline_create_info);
        return !!pipeline;
    }
    void destroy_pipeline(const vk::Pipeline& pipeline) {
        info_p->device.destroyPipeline(pipeline);
    }

    bool render_frame(void (*external_render)()) {
        info_p->device.waitForFences(1, &info_p->in_flight_fences[info_p->current_frame], VK_TRUE, std::numeric_limits<uint64_t >::max());
        vk::ResultValue<uint32_t> result_value = info_p->device.acquireNextImageKHR(info_p->swapchain, std::numeric_limits<uint64_t >::max(), info_p->image_available_semaphores[info_p->current_frame], vk::Fence());
        if (result_value.result == vk::Result::eErrorOutOfDateKHR) {
            if (reload_swapchain()) {
                return render_frame(external_render);
            }
            return false;
        }
        else if (result_value.result != vk::Result::eSuccess && result_value.result != vk::Result::eSuboptimalKHR) {
            return false;
        }
        uint32_t currentIndex = result_value.value;
        if (info_p->images_in_flight[currentIndex] != vk::Fence()) {
            info_p->device.waitForFences(1, &info_p->images_in_flight[currentIndex], VK_TRUE, std::numeric_limits<uint64_t >::max());
        }
        info_p->images_in_flight[currentIndex] = info_p->in_flight_fences[info_p->current_frame];

        info_p->device.resetCommandPool(info_p->commands[info_p->current_frame].pool, vk::CommandPoolResetFlagBits::eReleaseResources);

        vk::CommandBufferBeginInfo command_buffer_begin_info = {};
        info_p->commands[info_p->current_frame].buffers[0].begin(command_buffer_begin_info);

        std::array<float, 4> colour = {0.0f, 0.0f, 0.0f, 1.0f};
        vk::ClearValue clear_value = {{colour}};
        vk::RenderPassBeginInfo render_pass_begin_info = {info_p->render_pass, info_p->swapchain_framebuffers[currentIndex], {{0, 0}, info_p->swapchain_extent}, 1, &clear_value};
        info_p->commands[info_p->current_frame].buffers[0].beginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline);

        info_p->draw = true;
        external_render();
        info_p->draw = false;

        info_p->commands[info_p->current_frame].buffers[0].endRenderPass();
        info_p->commands[info_p->current_frame].buffers[0].end();

        vk::Semaphore wait_semaphores[] = {info_p->image_available_semaphores[info_p->current_frame]};
        vk::Semaphore signal_semaphores[] = {info_p->render_finished_semaphores[info_p->current_frame]};
        vk::PipelineStageFlags pipeline_stage_flags[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
        vk::SubmitInfo submit_info = {1, wait_semaphores, pipeline_stage_flags, 1, &info_p->commands[info_p->current_frame].buffers[0], 1, signal_semaphores};

        info_p->device.resetFences(1, &info_p->in_flight_fences[info_p->current_frame]);
        info_p->graphics_queue.submit(1, &submit_info, info_p->in_flight_fences[info_p->current_frame]);

        vk::PresentInfoKHR present_info = {1, signal_semaphores, 1, &info_p->swapchain, &currentIndex};
        info_p->present_queue.presentKHR(present_info);

        (info_p->current_frame += 1) %= MAX_FRAMES_IN_FLIGHT;
        return true;
    }
    void bind_pipeline(const vk::Pipeline& pipeline) {
        if (!info_p->draw) return;
        info_p->commands[info_p->current_frame].buffers[0].bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
    }
    void bind_vertex_buffers(uint32_t count, const vk::Buffer* buffers, const vk::DeviceSize* offsets) {
        if (!info_p->draw) return;
        info_p->commands[info_p->current_frame].buffers[0].bindVertexBuffers(0, count, buffers, offsets);
    }
    void push_constants(const vk::PipelineLayout& layout, const vk::ShaderStageFlags& stage, uint32_t offset, uint32_t size, const void* ptr) {
        if (!info_p->draw) return;
        info_p->commands[info_p->current_frame].buffers[0].pushConstants(layout, stage, offset, size, ptr);
    }
    void draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) {
        if (!info_p->draw) return;
        info_p->commands[info_p->current_frame].buffers[0].draw(vertex_count, instance_count, first_vertex, first_instance);
    }

    bool reload_swapchain() {
        destroy_swapchain();
        return create_swapchain();
    }

    void destroy_swapchain() {
        info_p->device.waitIdle();
        for (const vk::Framebuffer& framebuffer : info_p->swapchain_framebuffers) {
            info_p->device.destroyFramebuffer(framebuffer);
        }
        info_p->device.destroyRenderPass(info_p->render_pass);
        
        for (const vk::ImageView& image_view : info_p->swapchain_image_views) {
            info_p->device.destroyImageView(image_view);
        }
        info_p->device.destroySwapchainKHR(info_p->swapchain);
    }

    void wait_idle() {
        info_p->device.waitIdle();
    }

    void terminate() {
        destroy_swapchain();

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            info_p->device.destroySemaphore(info_p->image_available_semaphores[i]);
            info_p->device.destroySemaphore(info_p->render_finished_semaphores[i]);
            info_p->device.destroyFence(info_p->in_flight_fences[i]);
        }

        for (const Command& cmd : info_p->commands) {
            info_p->device.freeCommandBuffers(cmd.pool, cmd.buffers);
            info_p->device.destroyCommandPool(cmd.pool);
        }

        info_p->device.destroy();

#ifdef DEBUG_MODE
        destroyDebugUtilsMessengerEXT();
#endif
        info_p->instance.destroySurfaceKHR(info_p->surface);
        info_p->instance.destroy();
        info_p.reset(nullptr);
    }
}