#include "glfw_wrapper.hpp"

#include <memory>

namespace glfw_wrapper {
    namespace {
        // Place "private members" in here
        struct Info {
            GLFWwindow *window;
        };
        std::unique_ptr<Info> info;
    }
    const char** init(uint32_t* ext_count) {
        if (!info) {
            if (glfwInit() && glfwVulkanSupported()) {
                const char **ext_list = glfwGetRequiredInstanceExtensions(ext_count);
                glfwDefaultWindowHints();
                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                GLFWwindow* w = glfwCreateWindow(640, 480, "2D", nullptr, nullptr);
                if (w) {
                    info = std::make_unique<Info>();
                    info->window = w;
                    return ext_list;
                }
            }
        }
        *ext_count = 0;
        return nullptr;
    }

    bool createSurface(vk::Instance instance, vk::SurfaceKHR* surfaceKhr) {
        if (info && info->window) {
            VkSurfaceKHR tSurf;
            VkResult result = glfwCreateWindowSurface(VkInstance(instance), info->window, nullptr, &tSurf);
            *surfaceKhr = vk::SurfaceKHR(tSurf);
            return result == VK_SUCCESS;
        }
        return false;
    }

    void terminate() {
        if (info && info->window) {
            glfwDestroyWindow(info->window);
        }
        glfwTerminate();
    }
}

