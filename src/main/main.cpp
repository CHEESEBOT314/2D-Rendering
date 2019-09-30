#include "glfw_wrapper.hpp"
#include "vulkan_wrapper.hpp"

int main(int argc, char** args) {
    uint32_t ext_count;
    const char** ext_list = glfw_wrapper::init(&ext_count);
    if (ext_count == 0) {
        return 0;
    }
    if (!vulkan_wrapper::createInstance(ext_list, ext_count) ||
        !vulkan_wrapper::createSurface(glfw_wrapper::createSurface)) {
        return 0;
    }


    vulkan_wrapper::terminate();
    glfw_wrapper::terminate();
    return 0;
}

