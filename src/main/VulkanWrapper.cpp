#include "VulkanWrapper.hpp"

#include <memory>
#include <vulkan/vulkan.h>
#include <optional>
#include <set>

namespace VulkanWrapper {
    namespace {
        const int MAX_FRAMES_IN_FLIGHT = 3;
        void (*resolutionFunction)(int*, int*);
        struct Command {
            vk::CommandPool pool;
            std::vector<vk::CommandBuffer> buffers;
        };
        struct Info {
            vk::Instance instance;
            vk::SurfaceKHR surface;

            vk::PhysicalDevice physicalDevice;
            vk::Device device;
            vk::Queue graphicsQueue;
            vk::Queue presentQueue;

            vk::SwapchainKHR swapchain;
            std::vector<vk::Image> swapchainImages;
            vk::Format swapchainImageFormat = vk::Format::eB8G8R8A8Unorm;
            vk::Extent2D swapchainExtent;
            std::vector<vk::ImageView> swapchainImageViews;
            std::vector<vk::Framebuffer> swapchainFramebuffers;
            vk::RenderPass renderPass;

            std::vector<Command> commands;
            std::vector<vk::Semaphore> imageAvailableSemaphores;
            std::vector<vk::Semaphore> renderFinishedSemaphores;
            std::vector<vk::Fence> inFlightFences;
            std::vector<vk::Fence> imagesInFlight;

            size_t currentFrame = 0;
            bool draw = false;

            vk::DispatchLoaderDynamic dldi;
#ifdef DEBUG_MODE
            vk::DebugUtilsMessengerEXT debugMessenger;
#endif
        };
        std::unique_ptr<Info> info;

#ifdef DEBUG_MODE
        VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
            printf("%s", pCallbackData->pMessage);
            return VK_FALSE;
        }
        vk::Result createDebugUtilsMessengerEXT() {
            vk::DebugUtilsMessengerCreateInfoEXT createInfo = {vk::DebugUtilsMessengerCreateFlagsEXT(),
                                                               vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
                                                               vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
                                                               debugCallback};
            return info->instance.createDebugUtilsMessengerEXT(&createInfo, nullptr, &info->debugMessenger, info->dldi);
        }
        void destroyDebugUtilsMessengerEXT() {
            info->instance.destroyDebugUtilsMessengerEXT(info->debugMessenger, nullptr, info->dldi);
        }
#endif
        struct QueueFamilyIndices {
            std::optional<uint32_t> graphicsFamily;
            std::optional<uint32_t> presentFamily;
            bool isComplete() {
                return graphicsFamily.has_value() && presentFamily.has_value();
            }
        };
        QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice physicalDevice) {
            QueueFamilyIndices indices;
            std::vector<vk::QueueFamilyProperties> queueFamilies = physicalDevice.getQueueFamilyProperties();

            int i = 0;
            for (const vk::QueueFamilyProperties& properties : queueFamilies) {
                if (properties.queueCount > 0) {
                    if (properties.queueFlags & vk::QueueFlagBits::eGraphics) {
                        indices.graphicsFamily = i;
                    }
                    if (physicalDevice.getSurfaceSupportKHR(i, info->surface)) {
                        indices.presentFamily = i;
                    }
                    if (indices.isComplete()) {
                        break;
                    }
                }
                i++;
            }
            return indices;
        }
        bool checkDeviceExtensionSupport(vk::PhysicalDevice pd) {
            std::vector<vk::ExtensionProperties> extensionProperties = pd.enumerateDeviceExtensionProperties(nullptr);

            std::set<std::string> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

            for (vk::ExtensionProperties properties : extensionProperties) {
                deviceExtensions.erase(properties.extensionName);
            }
            return deviceExtensions.empty();
        }
        struct SwapchainSupportDetails {
            vk::SurfaceCapabilitiesKHR capabilities;
            std::vector<vk::SurfaceFormatKHR> formats;
            std::vector<vk::PresentModeKHR> presentModes;
        };

        SwapchainSupportDetails querySwapchainSupport(vk::PhysicalDevice device) {
            SwapchainSupportDetails details;

            details.capabilities = device.getSurfaceCapabilitiesKHR(info->surface);
            details.formats = device.getSurfaceFormatsKHR(info->surface);
            details.presentModes = device.getSurfacePresentModesKHR(info->surface);

            return details;
        }
        vk::SurfaceFormatKHR chooseSwapchainSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
            if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined) {
                return {vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eVkColorspaceSrgbNonlinear};
            }

            for (const auto& availableFormat : availableFormats) {
                if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eVkColorspaceSrgbNonlinear) {
                    return availableFormat;
                }
            }
            return availableFormats[0];
        }
        vk::PresentModeKHR chooseSwapchainPresentMode(const std::vector<vk::PresentModeKHR>& availableModes) {
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
        vk::Extent2D chooseSwapchainExtent(const vk::SurfaceCapabilitiesKHR& capabilities) {
            if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
                return capabilities.currentExtent;
            }
            int width, height;
            resolutionFunction(&width, &height);

            vk::Extent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

            actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
        bool isDeviceSuitable(vk::PhysicalDevice physicalDevice) {
            QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

            bool extensionsSupported = checkDeviceExtensionSupport(physicalDevice);

            bool swapchainAdequate = false;
            if (extensionsSupported) {
                SwapchainSupportDetails swapchainSupport = querySwapchainSupport(physicalDevice);
                swapchainAdequate = !swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
            }

            return indices.isComplete() && extensionsSupported && swapchainAdequate;
        }
    }
    bool createInstance(std::vector<const char*> extensions) {
        if (info) {
            return false;
        }
        info = std::make_unique<Info>();

        const std::vector<const char*> validationLayers = {"VK_LAYER_LUNARG_standard_validation"};

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
        vk::ApplicationInfo appInfo = {"InViculum", 0x00400000/* 0000000000,0000000000,000000000000 */, "No Engine", 0x00400000, 0x00401000};
        vk::InstanceCreateInfo createInfo = {vk::InstanceCreateFlags(), &appInfo,
#ifdef DEBUG_MODE
        static_cast<uint32_t>(validationLayers.size()), validationLayers.data(),
#else
            0, nullptr,
#endif
            static_cast<uint32_t>(extensions.size()), extensions.data()};

        info->instance = vk::createInstance(createInfo);
        info->dldi = vk::DispatchLoaderDynamic(info->instance);
#ifdef DEBUG_MODE
        createDebugUtilsMessengerEXT();
#endif
        return true;
    }
    bool createSurface(bool(*fn)(const vk::Instance&, vk::SurfaceKHR&), void (*r)(int*, int*)) {
        if (info) {
            resolutionFunction = r;
            return fn(info->instance, info->surface);
        }
        return false;
    }

    bool createOthers() {
        /////////////////////////
        //// PHYSICAL DEVICE ////
        /////////////////////////
        std::vector<vk::PhysicalDevice> physicalDevices = info->instance.enumeratePhysicalDevices();
        for (const vk::PhysicalDevice& d : physicalDevices) {
            if (isDeviceSuitable(d)) {
                info->physicalDevice = d;
                break;
            }
        }
        if (!info->physicalDevice) {
            return false;
        }

        ////////////////
        //// DEVICE ////
        ////////////////
        QueueFamilyIndices indices = findQueueFamilies(info->physicalDevice);

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            vk::DeviceQueueCreateInfo queueCreateInfo = {vk::DeviceQueueCreateFlags(), queueFamily, 1, &queuePriority};
                queueCreateInfos.push_back(queueCreateInfo);
        }

        vk::PhysicalDeviceFeatures deviceFeatures = {};
        const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
#ifdef DEBUG_MODE
        const std::vector<const char*> validationLayers = {"VK_LAYER_LUNARG_standard_validation"};
#endif

        vk::DeviceCreateInfo deviceCreateInfo = {vk::DeviceCreateFlags(), static_cast<uint32_t>(queueCreateInfos.size()), queueCreateInfos.data(),
            
#ifdef  DEBUG_MODE
            static_cast<uint32_t>(validationLayers.size()), validationLayers.data(),
#else
            0, nullptr,
#endif
                                                 static_cast<uint32_t>(deviceExtensions.size()), deviceExtensions.data(), &deviceFeatures};

        info->device = info->physicalDevice.createDevice(deviceCreateInfo);

        info->graphicsQueue = info->device.getQueue(indices.graphicsFamily.value(), 0);
        info->presentQueue = info->device.getQueue(indices.presentFamily.value(), 0);

        ///////////////////////
        //// COMMAND POOLS ////
        ///////////////////////
        info->commands.resize(MAX_FRAMES_IN_FLIGHT);
        vk::CommandPoolCreateInfo commandPoolCreateInfo = {vk::CommandPoolCreateFlags(), indices.graphicsFamily.value()};
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            info->commands[i].pool = info->device.createCommandPool(commandPoolCreateInfo);
        }

        /////////////////////////
        //// COMMAND BUFFERS ////
        /////////////////////////
        for (Command& cmd : info->commands) {
            cmd.buffers.resize(1);
            vk::CommandBufferAllocateInfo commandBufferAllocateInfo = {cmd.pool, vk::CommandBufferLevel::ePrimary, 1};
            cmd.buffers = info->device.allocateCommandBuffers(commandBufferAllocateInfo);
        }

        //////////////////////
        //// SYNC OBJECTS ////
        //////////////////////
        info->imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        info->renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        info->inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        vk::SemaphoreCreateInfo semaphoreCreateInfo = {vk::SemaphoreCreateFlags()};
        vk::FenceCreateInfo fenceCreateInfo = {vk::FenceCreateFlagBits::eSignaled};
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            info->imageAvailableSemaphores[i] = info->device.createSemaphore(semaphoreCreateInfo);
            info->renderFinishedSemaphores[i] = info->device.createSemaphore(semaphoreCreateInfo);
            info->inFlightFences[i] = info->device.createFence(fenceCreateInfo);
        }
        return createSwapchain();
    }

    bool createSwapchain() {
        info->device.waitIdle();
        ///////////////////
        //// SWAPCHAIN ////
        ///////////////////
        SwapchainSupportDetails swapchainSupport = querySwapchainSupport(info->physicalDevice);

        vk::SurfaceFormatKHR surfaceFormat = chooseSwapchainSurfaceFormat(swapchainSupport.formats);
        vk::PresentModeKHR presentMode = chooseSwapchainPresentMode(swapchainSupport.presentModes);
        vk::Extent2D extent = chooseSwapchainExtent(swapchainSupport.capabilities);

        uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;
        if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount) {
            imageCount = swapchainSupport.capabilities.maxImageCount;
        }

        QueueFamilyIndices indices = findQueueFamilies(info->physicalDevice);
        uint32_t  queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};
        bool queueDifferent = indices.graphicsFamily != indices.presentFamily;

        vk::SwapchainCreateInfoKHR swapchainCreateInfo = {vk::SwapchainCreateFlagsKHR(), info->surface, imageCount, surfaceFormat.format,
                                                            surfaceFormat.colorSpace, extent, 1, vk::ImageUsageFlagBits::eColorAttachment,
                                                            queueDifferent ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive,
                                                            queueDifferent ? 2U : 0U, queueDifferent ? queueFamilyIndices : nullptr,
                                                            swapchainSupport.capabilities.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque,
                                                            presentMode, true};

        info->swapchain = info->device.createSwapchainKHR(swapchainCreateInfo);

        info->swapchainImages = info->device.getSwapchainImagesKHR(info->swapchain);
        info->swapchainImageFormat = surfaceFormat.format;
        info->swapchainExtent = extent;

        /////////////////////
        //// IMAGE VIEWS ////
        /////////////////////
        info->swapchainImageViews.resize(info->swapchainImages.size());
        for (uint32_t i = 0; i < info->swapchainImages.size(); i++) {
            vk::ImageViewCreateInfo imageViewCreateInfo = {vk::ImageViewCreateFlags(), info->swapchainImages[i], vk::ImageViewType::e2D, info->swapchainImageFormat,
                                                            {vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity},
                                                            {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}};


            info->swapchainImageViews[i] = info->device.createImageView(imageViewCreateInfo);
        }

        /////////////////////
        //// RENDER PASS ////
        /////////////////////
        vk::AttachmentDescription attachmentDescription = {vk::AttachmentDescriptionFlags(), info->swapchainImageFormat, vk::SampleCountFlagBits::e1,
                                                            vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare,
                                                            vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR};

        vk::AttachmentReference attachmentReference = {0, vk::ImageLayout::eColorAttachmentOptimal};

        vk::SubpassDescription subpassDescription = {vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics, 0, nullptr, 1, &attachmentReference, nullptr, nullptr, 0, nullptr};

        vk::SubpassDependency subpassDependency = {~0U, 0, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::AccessFlags(),
                                                    vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite, vk::DependencyFlags()};

        vk::RenderPassCreateInfo renderPassCreateInfo = {vk::RenderPassCreateFlags(), 1, &attachmentDescription, 1, &subpassDescription, 1, &subpassDependency};

        info->renderPass = info->device.createRenderPass(renderPassCreateInfo);

        //////////////////////
        //// FRAMEBUFFERS ////
        //////////////////////
        info->swapchainFramebuffers.resize(info->swapchainImageViews.size());

        for (uint32_t i = 0; i < info->swapchainFramebuffers.size(); i++) {
            vk::FramebufferCreateInfo framebufferCreateInfo = {vk::FramebufferCreateFlags(), info->renderPass, 1, &info->swapchainImageViews[i],
                                                                info->swapchainExtent.width, info->swapchainExtent.height, 1};

            info->swapchainFramebuffers[i] = info->device.createFramebuffer(framebufferCreateInfo);
        }
        info->imagesInFlight.resize(info->swapchainImages.size(), VK_NULL_HANDLE);
        return true;
    }

    bool createShaderModule(vk::ShaderModule& shaderModule, const std::vector<uint8_t>& src) {
        vk::ShaderModuleCreateInfo shaderModuleCreateInfo = {vk::ShaderModuleCreateFlags(), src.size(), reinterpret_cast<const uint32_t*>(src.data())};
        shaderModule = info->device.createShaderModule(shaderModuleCreateInfo);
        return true;
    }
    void destroyShaderModule(const vk::ShaderModule& shaderModule) {
        info->device.destroyShaderModule(shaderModule);
    }

    bool createPipelineLayout(vk::PipelineLayout& pipelineLayout, const vk::PipelineLayoutCreateInfo& pipelineLayoutCreateInfo) {
        pipelineLayout = info->device.createPipelineLayout(pipelineLayoutCreateInfo);
        return !!pipelineLayout;
    }
    void destroyPipelineLayout(const vk::PipelineLayout& pipelineLayout) {
        info->device.destroyPipelineLayout(pipelineLayout);
    }
    bool createPipeline(vk::Pipeline& pipeline, const vk::PipelineLayout& pipelineLayout, uint32_t shaderModuleCount, const vk::PipelineShaderStageCreateInfo* shaderModules, uint32_t vertexBindingDescriptionCount, const vk::VertexInputBindingDescription* vertexBindingDescriptions, uint32_t vertexAttributeDescriptionCount, const vk::VertexInputAttributeDescription* vertexAttributeDescriptions) {
        vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {vk::PipelineVertexInputStateCreateFlags(), vertexBindingDescriptionCount, vertexBindingDescriptions, vertexAttributeDescriptionCount, vertexAttributeDescriptions};
        vk::PipelineInputAssemblyStateCreateInfo pipelineAssemblyStateCreateInfo = {vk::PipelineInputAssemblyStateCreateFlags(), vk::PrimitiveTopology::eTriangleList, VK_FALSE};
        vk::Viewport viewport = {0.0f, 0.0f, (float)info->swapchainExtent.width, (float)info->swapchainExtent.height, 0.0f, 1.0f};
        vk::Rect2D scissor = {{0, 0}, info->swapchainExtent};
        vk::PipelineViewportStateCreateInfo pipelineViewportStateCreateInfo = {vk::PipelineViewportStateCreateFlags(), 1, &viewport, 1, &scissor};
        vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo = {vk::PipelineRasterizationStateCreateFlags(), VK_FALSE, VK_FALSE, vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack, vk::FrontFace::eClockwise, VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f};
        vk::PipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo = {vk::PipelineMultisampleStateCreateFlags(), vk::SampleCountFlagBits::e1, VK_FALSE, 1.0f, nullptr, VK_FALSE, VK_FALSE};
        vk::PipelineColorBlendAttachmentState pipelineColorBlendAttachmentState = {VK_TRUE, vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendOp::eAdd, vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd, vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA};
        vk::PipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = {vk::PipelineColorBlendStateCreateFlags(), VK_FALSE, vk::LogicOp::eCopy, 1, &pipelineColorBlendAttachmentState, {0.0f, 0.0f, 0.0f, 0.0f}};

        vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {vk::PipelineCreateFlags(), shaderModuleCount, shaderModules, &pipelineVertexInputStateCreateInfo, &pipelineAssemblyStateCreateInfo, nullptr, &pipelineViewportStateCreateInfo, &pipelineRasterizationStateCreateInfo, &pipelineMultisampleStateCreateInfo, nullptr, &pipelineColorBlendStateCreateInfo, nullptr, pipelineLayout, info->renderPass, 0, vk::Pipeline(), -1};
        pipeline = info->device.createGraphicsPipeline(vk::PipelineCache(), graphicsPipelineCreateInfo);
        return !!pipeline;
    }
    void destroyPipeline(const vk::Pipeline& pipeline) {
        info->device.destroyPipeline(pipeline);
    }

    bool renderFrame(void (*externalRender)()) {
        info->device.waitForFences(1, &info->inFlightFences[info->currentFrame], VK_TRUE, std::numeric_limits<uint64_t >::max());
        vk::ResultValue<uint32_t> resultValue = info->device.acquireNextImageKHR(info->swapchain, std::numeric_limits<uint64_t >::max(), info->imageAvailableSemaphores[info->currentFrame], vk::Fence());
        if (resultValue.result == vk::Result::eErrorOutOfDateKHR) {
            if (reloadSwapchain()) {
                return renderFrame(externalRender);
            }
            return false;
        }
        else if (resultValue.result != vk::Result::eSuccess && resultValue.result != vk::Result::eSuboptimalKHR) {
            return false;
        }
        uint32_t currentIndex = resultValue.value;
        if (info->imagesInFlight[currentIndex] != VK_NULL_HANDLE) {
            info->device.waitForFences(1, &info->imagesInFlight[currentIndex], VK_TRUE, std::numeric_limits<uint64_t >::max());
        }
        info->imagesInFlight[currentIndex] = info->inFlightFences[info->currentFrame];

        info->device.resetCommandPool(info->commands[info->currentFrame].pool, vk::CommandPoolResetFlagBits::eReleaseResources);

        vk::CommandBufferBeginInfo commandBufferBeginInfo = {};
        info->commands[info->currentFrame].buffers[0].begin(commandBufferBeginInfo);

        std::array<float, 4> colour = {0.0f, 0.0f, 0.0f, 1.0f};
        vk::ClearValue clearValue = {{colour}};
        vk::RenderPassBeginInfo renderPassBeginInfo = {info->renderPass, info->swapchainFramebuffers[currentIndex], {{0, 0}, info->swapchainExtent}, 1, &clearValue};
        info->commands[info->currentFrame].buffers[0].beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

        info->draw = true;
        externalRender();
        info->draw = false;

        info->commands[info->currentFrame].buffers[0].endRenderPass();
        info->commands[info->currentFrame].buffers[0].end();

        vk::Semaphore waitSemaphores[] = {info->imageAvailableSemaphores[info->currentFrame]};
        vk::Semaphore signalSemaphores[] = {info->renderFinishedSemaphores[info->currentFrame]};
        vk::PipelineStageFlags pipelineStageFlags[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
        vk::SubmitInfo submitInfo = {1, waitSemaphores, pipelineStageFlags, 1, &info->commands[info->currentFrame].buffers[0], 1, signalSemaphores};

        info->device.resetFences(1, &info->inFlightFences[info->currentFrame]);
        info->graphicsQueue.submit(1, &submitInfo, info->inFlightFences[info->currentFrame]);

        vk::PresentInfoKHR presentInfo = {1, signalSemaphores, 1, &info->swapchain, &currentIndex};
        info->presentQueue.presentKHR(presentInfo);

        (info->currentFrame += 1) %= MAX_FRAMES_IN_FLIGHT;
        return true;
    }
    void bindPipeline(const vk::Pipeline& pipeline) {
        if (!info->draw) return;
        info->commands[info->currentFrame].buffers[0].bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
    }
    void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
        info->commands[info->currentFrame].buffers[0].draw(vertexCount, instanceCount, firstVertex, firstInstance);
    }

    bool reloadSwapchain() {
        destroySwapchain();
        return createSwapchain();
    }

    void destroySwapchain() {
        info->device.waitIdle();
        for (const vk::Framebuffer& framebuffer : info->swapchainFramebuffers) {
            info->device.destroyFramebuffer(framebuffer);
        }
        info->device.destroyRenderPass(info->renderPass);
        
        for (const vk::ImageView& imageView : info->swapchainImageViews) {
            info->device.destroyImageView(imageView);
        }
        info->device.destroySwapchainKHR(info->swapchain);
    }

    void waitIdle() {
        info->device.waitIdle();
    }

    void terminate() {
        destroySwapchain();

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            info->device.destroySemaphore(info->imageAvailableSemaphores[i]);
            info->device.destroySemaphore(info->renderFinishedSemaphores[i]);
            info->device.destroyFence(info->inFlightFences[i]);
        }

        for (const Command& cmd : info->commands) {
            info->device.freeCommandBuffers(cmd.pool, cmd.buffers);
            info->device.destroyCommandPool(cmd.pool);
        }

        info->device.destroy();

#ifdef DEBUG_MODE
        destroyDebugUtilsMessengerEXT();
#endif
        info->instance.destroySurfaceKHR(info->surface);
        info->instance.destroy();
        info.reset(nullptr);
    }
}