#include "vulkan_wrapper.hpp"

#include <memory>
#include <vulkan/vulkan.h>
#include <optional>
#include <set>

namespace vulkan_wrapper {
    namespace {
        struct Info {
            vk::Instance instance;
            vk::SurfaceKHR surface;

            vk::PhysicalDevice physicalDevice;
            vk::Device device;
            vk::Queue graphicsQueue;
            vk::Queue presentQueue;

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
        struct SwapChainSupportDetails {
            vk::SurfaceCapabilitiesKHR capabilities;
            std::vector<vk::SurfaceFormatKHR> formats;
            std::vector<vk::PresentModeKHR> presentModes;
        };

        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
            SwapChainSupportDetails details;

            details.capabilities = device.getSurfaceCapabilitiesKHR(info->surface);
            details.formats = device.getSurfaceFormatsKHR(info->surface);
            details.presentModes = device.getSurfacePresentModesKHR(info->surface);

            return details;
        }
        vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
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
    bool createSurface(bool(*fn)(vk::Instance, vk::SurfaceKHR*)) {
        if (info) {
            return fn(info->instance, &info->surface);
        }
        return false;
    }

    void terminate() {
        info->instance.destroySurfaceKHR(info->surface);
        info->instance.destroy();
        info.reset(nullptr);
    }
}