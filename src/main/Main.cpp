#include "Game.hpp"
#include "GlfwWrapper.hpp"
#include "VulkanWrapper.hpp"
#include "platform/Platform.hpp"
#include "render/RenderManager.hpp"
#include "render/SpriteManager.hpp"
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
    resource::ResourceManager::init(Platform::Files::getResourceFolder(), Platform::Files::FILE_SEPARATOR);

    render::RenderManager::init();
    render::RenderManager::loadShaders();

    render::SpriteManager::init();

    Game::init();

    while (!(GlfwWrapper::shouldQuit() || Game::shouldQuit())) {
        GlfwWrapper::pollEvents();
        Game::update();
        if (!VulkanWrapper::renderFrame(Game::render)) {
            break;
        }
    }
    VulkanWrapper::waitIdle();

    render::RenderManager::terminate();
    VulkanWrapper::terminate();
    GlfwWrapper::terminate();
    return 0;
}

