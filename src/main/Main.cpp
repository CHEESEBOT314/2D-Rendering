#include "Game.hpp"
#include "GlfwWrapper.hpp"
#include "VulkanWrapper.hpp"

int main(int argc, char** args) {
    std::vector<const char*> extensions = GlfwWrapper::init();
    if (extensions.empty()) {
        return 0;
    }
    if (!VulkanWrapper::createInstance(extensions) ||
        !VulkanWrapper::createSurface(GlfwWrapper::createSurface, GlfwWrapper::getResolution) ||
        !VulkanWrapper::createOthers()) {
        return 0;
    }
    
    Game::init();

    while (!(GlfwWrapper::shouldQuit() || Game::shouldQuit())) {
        GlfwWrapper::pollEvents();
        Game::update();
        //VulkanWrapper::renderFrame(Game::render);
    }

    VulkanWrapper::terminate();
    GlfwWrapper::terminate();
    return 0;
}

