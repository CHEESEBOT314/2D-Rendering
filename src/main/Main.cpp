#include "Game.hpp"
#include "GlfwWrapper.hpp"
#include "VulkanWrapper.hpp"
#include "platform/Platform.hpp"
#include "resource/ResourceManager.hpp"

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
    ResourceManager::init(Platform::Files::getResourceFolder(), Platform::Files::FILE_SEPARATOR);
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

