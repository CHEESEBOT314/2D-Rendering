#include <bits/unique_ptr.h>

#include <memory>
#include <vulkan/vulkan.h>
#include <optional>
#include "vulkan_wrapper.hpp"

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