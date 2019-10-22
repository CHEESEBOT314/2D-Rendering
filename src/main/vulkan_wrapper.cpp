#include "vulkan_wrapper.hpp"

#include <memory>
#include <vulkan/vulkan.h>
#include <optional>
#include <set>

namespace vulkan_wrapper {
    namespace {
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
            vk::Format swapchainImageFormat;
            vk::Extent2D swapchainExtent;
            std::vector<vk::ImageView> swapchainImageViews;
            std::vector<vk::Framebuffer> swapchainFramebuffers;
            vk::RenderPass renderPass;

            vk::DispatchLoaderDynamic dldi;
#ifdef DEBUG_MODE
            vk::DebugUtilsMessengerEXT debugMessenger;
#endif
        };
        std::unique_ptr<Info> info;

#ifdef DEBUG_MODE
        VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
            return VK_FALSE;
        }
        vk::Result createDebugUtilsMessengerEXT() {
            vk::DebugUtilsMessengerCreateInfoEXT createInfo = {vk::DebugUtilsMessengerCreateFlagsEXT(),
                                                               vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
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
        QueueFamilyIndices findQueueFamilies() {
            QueueFamilyIndices indices;
            std::vector<vk::QueueFamilyProperties> queueFamilies = info->physicalDevice.getQueueFamilyProperties();

            int i = 0;
            for (vk::QueueFamilyProperties properties : queueFamilies) {
                if (properties.queueCount > 0) {
                    if (properties.queueFlags & vk::QueueFlagBits::eGraphics) {
                        indices.graphicsFamily = i;
                    }
                    if (info->physicalDevice.getSurfaceSupportKHR(i, info->surface)) {
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
        vk::PresentModeKHR chooseSwapchainPresentMode(const std::vector<vk::PresentModeKHR>& availabeModes) {
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
        bool isDeviceSuitable(vk::PhysicalDevice device, vk::SurfaceKHR surface) {
            QueueFamilyIndices indices = findQueueFamilies(device, surface);

            bool extensionsSupported = checkDeviceExtensionSupport(device);

            bool swapchainAdequate = false;
            if (extensionsSupported) {
                SwapchainSupportDetails swapchainSupport = querySwapchainSupport(device, surface);
                swapchainAdequate = !swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
            }

            return indices.isComplete() && extensionsSupported && swapchainAdequate;
        }
    }
    bool createInstance(const char** ext_list, uint32_t ext_count) {
        if (info) {
            return false;
        }
        info = std::make_unique<Info>();
        vk::InstanceCreateInfo createInfo = {vk::InstanceCreateFlags(), nullptr, 0, nullptr, ext_count, ext_list};
        info->instance = vk::createInstance(createInfo);
        info->dldi = vk::DispatchLoaderDynamic(info->instance);
        return true;
    }
    bool createSurface(bool(*fn)(vk::Instance, vk::SurfaceKHR*), void (*r)(int*, int*)) {
        if (info) {
            resolutionFunction = r;
            return fn(info->instance, &info->surface);
        }
        return false;
    }

    bool createOthers() {
        /////////////////////////
        //// PHYSICAL DEVICE ////
        /////////////////////////
        std::vector<vk::PhysicalDevice> physicalDevices = vk::enumeratePhysicalDevices();
        for (const vk::PhysicalDevice& d : physicalDevices) {
            if (isDeviceSuitable(d, info->surface)) {
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
        QueueFamilyIndices indices = findQueueFamilies(info->physicalDevice, info->surface);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            vk::DeviceQueueCreateInfo queueCreateInfo = {vk::DeviceQueueCreateFlags(), queueFamily, 1, &queuePriority};
                queueCreateInfos.push_back(queueCreateInfo);
        }

        vk::PhysicalDeviceFeatures deviceFeatures = {};

        vk::DeviceCreateInfo deviceCreateInfo = {vk::DeviceCreateFlags(), static_cast<uint32_t>(queueCreateInfos.size()), queueCreateInfos.data(),
                                                    &deviceFeatures, static_cast<uint32_t>(deviceExtensions.size()), deviceExtensions.data(),
            
#ifdef  DEBUG_MODE
            static_cast<uint32_t>(validationLayers.size()), validationLayers.data()
#else
            0, nullptr
#endif
        };

        info->device = info->physicalDevice.createDevice(deviceCreateInfo);
        if (!info->device) {
            return false;
        }

        info->graphicsQueue = info->device.getQueue(indices.graphicsFamily.value(), 0);
        info->presentQueue = info->device.getQueue(indices.presentFamily.value(), 0);

        return createSwapchain();
    }

    bool createSwapchain() {
        ///////////////////
        //// SWAPCHAIN ////
        ///////////////////
        SwapchainSupportDetails swapchainSupport = querySwapchainSupport(info->physicalDevice, info->surface);

        vk::SurfaceFormatKHR surfaceFormat = chooseSwapchainSurfaceFormat(swapchainSupport.formats);
        vk::PresentModeKHR presentMode = chooseSwapchainPresentMode(swapchainSupport.presentModes);
        vk::Extent2D extent = chooseSwapchainExtent(swapchainSupport.capabilities);

        uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;
        if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount) {
            imageCount = swapchainSupport.capabilities.maxImageCount;
        }

        bool queueDifferent = indices.graphicsFamily != indices.presentFamily
        vk::SwapchainCreateInfoKHR swapchainCreateInfo = {vk::SwapchainCreateFlagsKHR(), info->surface, imageCount, surfaceFormat.format,
                                                            surfaceFormat.colorSpace, extent, 1, vk::ImageUsageFlagBits::eColorAttachment,
                                                            queueDifferent ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive,
                                                            queueDifferent ? 2 : 0, queueDifferent ? queueFamilyIndices : nullptr,
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
        for (int i = 0; i < info->swapchainImages.size(); i++) {
            vk::ImageViewCreateInfo imageViewCreateInfo = {vk::ImageViewCreateFlags(), info->swapchainImages[i], vk::ImageViewType::e2D, info->swapchainImageFormat,
                                                            {vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity},
                                                            {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}};


            info->swapchainImageViews[i] = info->device.createImageView(&ImageViewCreateInfo);
        }

        /////////////////////
        //// RENDER PASS ////
        /////////////////////
        vk::AttachmentDescription attachmentDescription = {vk::AttachmentDescriptionFlags(), info->swapchainImageFormat, vk::SampleCountFlagBits::e1,
                                                            vk::AttachmentLoadOp::eLoad, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eLoad,
                                                            vk::AttachmentStoreOp::eStore, vk::ImageLayout::eUndefined, vk::ImageLayout::eSharedPresentKHR};

        vk::AttachmentReference attachmentReference = {0, vk::ImageLayout::eColorAttachmentOptimal};

        vk::SubpassDescription subpassDescription = {vk::PipelineBindPoint::eGraphics, 0, nullptr, 1, &attachmentReference, nullptr, nullptr, 0, nullptr};

        vk::SubpassDependency subpassDependency = {~0U, 0, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput, 0,
                                                    vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite};

        vk::RenderPassCreateInfo renderPassCreateInfo = {vk::RenderPassCreateFlags(), 1, &attachmentDescription, 1, &subpassDescription, 1, &subpassDependency};

        info->renderPass = info->device.createRenderPass(&renderPassCreateInfo);

        //////////////////////
        //// FRAMEBUFFERS ////
        //////////////////////
        info->swapchainFramebuffers.resize(info->swapchainImageViews.size());

        for (int i = 0; i < info->swapchainFramebuffers.size(); i++) {
            vk::FramebufferCreateInfo framebufferCreateInfo = {vk::FramebufferCreateFlags(), info->renderPass, 1, &info->swapchainImageViews[i],
                                                                info->swapchainExtent.width, info->swapchainExtent.height, 1};

            info->swapchainFramebuffers[i] = info->device.createFramebuffer(&framebufferCreateInfo);
        }
    }

    bool reloadSwapchain() {
        destroySwapchain();
        return createSwapchain();
    }

    void destroySwapchain() {
        for (vk::Framebuffer framebuffer : info->swapchainFramebuffers) {
            info->device.destroyFramebuffer(framebuffer);
        }
        info->device.destroyRenderPass(info->renderPass);
        
        for (vk::ImageView imageView : info->swapchainImageViews) {
            info->device.destroyImageView(imageView);
        }
    }

    void terminate() {
        destroySwapchain();

        info->device.destroy();

#ifdef DEBUG_MODE
        destroyDebugUtilsMessengerEXT();
#endif
        info->instance.destroySurfaceKHR(info->surface);
        info->instance.destroy();
        info.reset(nullptr);
    }
}