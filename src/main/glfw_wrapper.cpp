#include "glfw_wrapper.hpp"

#include <memory>

namespace glfw_wrapper {
    namespace {
        //--Place "private members" in here--//
        struct info {
            GLFWwindow *window;
        };
        std::unique_ptr<info> info_p;

        void error_callback(int error, const char* description) {
            printf("GLFW error %d: %s\n", error, description);
        }
        void key_callback(GLFWwindow* w, int key, int scancode, int action, int mods) {
            if (key == GLFW_KEY_ESCAPE) {
                glfwSetWindowShouldClose(w, true);
            }
        }
    }
    std::vector<const char*> init() {
        std::vector<const char*> extensions;
        if (!info_p) {
            glfwSetErrorCallback(error_callback);
            if (glfwInit() && glfwVulkanSupported()) {
                uint32_t ext_count;
                const char **ext_list = glfwGetRequiredInstanceExtensions(&ext_count);
                extensions = std::vector<const char*>(ext_list, ext_list + ext_count);
                glfwDefaultWindowHints();
                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

                GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);

                GLFWwindow* w = glfwCreateWindow(vidmode->width, vidmode->height, "2D", monitor, nullptr);
                if (w) {
                    info_p = std::make_unique<info>();
                    info_p->window = w;
                    glfwSetKeyCallback(w, key_callback);

                    return extensions;
                }
            }
        }
        extensions.clear();
        return extensions;
    }

    bool create_surface(const vk::Instance& instance, vk::SurfaceKHR& surface_khr) {
        if (info_p && info_p->window) {
            VkSurfaceKHR t_surf;
            VkResult result = glfwCreateWindowSurface(VkInstance(instance), info_p->window, nullptr, &t_surf);
            surface_khr = vk::SurfaceKHR(t_surf);
            return result == VK_SUCCESS;
        }
        return false;
    }

    void get_resolution(int* width, int* height) {
       glfwGetFramebufferSize(info_p->window, width, height);
    }

    void poll_events() {
        glfwPollEvents();
    }

    bool should_quit() {
        return glfwWindowShouldClose(info_p->window) == GLFW_TRUE;
    }
    void terminate() {
        if (info_p && info_p->window) {
            glfwDestroyWindow(info_p->window);
        }
        glfwTerminate();
        info_p.reset(nullptr);
    }
}

