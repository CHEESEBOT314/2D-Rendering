#include "GlfwWrapper.hpp"

#include <memory>

namespace GlfwWrapper {
    namespace {
        //--Place "private members" in here--//
        struct Info {
            GLFWwindow *window;
        };
        std::unique_ptr<Info> info;
    }
    std::vector<const char*> init() {
        std::vector<const char*> extensions;
        if (!info) {
            if (glfwInit() && glfwVulkanSupported()) {
                uint32_t extCount;
                const char **extList = glfwGetRequiredInstanceExtensions(&extCount);
                extensions = std::vector<const char*>(extList, extList + extCount);
                glfwDefaultWindowHints();
                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

                GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);

                GLFWwindow* w = glfwCreateWindow(vidmode->width, vidmode->height, "2D", monitor, nullptr);
                if (w) {
                    info = std::make_unique<Info>();
                    info->window = w;
                    return extensions;
                }
            }
        }
        extensions.clear();
        return extensions;
    }

    bool createSurface(const vk::Instance& instance, vk::SurfaceKHR& surfaceKhr) {
        if (info && info->window) {
            VkSurfaceKHR tSurf;
            VkResult result = glfwCreateWindowSurface(VkInstance(instance), info->window, nullptr, &tSurf);
            surfaceKhr = vk::SurfaceKHR(tSurf);
            return result == VK_SUCCESS;
        }
        return false;
    }

    void getResolution(int* width, int* height) {
       glfwGetFramebufferSize(info->window, width, height);
    }

    void pollEvents() {
        glfwPollEvents();
    }

    bool shouldQuit() {
        return glfwWindowShouldClose(info->window);
    }
    void terminate() {
        if (info && info->window) {
            glfwDestroyWindow(info->window);
        }
        glfwTerminate();
        info.reset(nullptr);
    }
}

