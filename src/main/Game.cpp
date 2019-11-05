#include "Game.hpp"

#include "render/RenderManager.hpp"

#include <memory>

namespace Game {
    namespace {
        struct Info {
            uint32_t shaderID;
            uint32_t counter;
        };
        std::unique_ptr<Info> info;
    }
    void init() {
        info = std::make_unique<Info>();
        if (!render::RenderManager::createGraphicsPipeline("default")) {

            return;
        }
        info->shaderID = render::RenderManager::getPipelineID("default");
        info->counter = 0;
    }

    void render() {
        render::RenderManager::bindPipeline(info->shaderID);
        render::RenderManager::drawRect2D();
    }
    void handleEvent() {

    }
    void update() {
        info->counter++;
    }

    bool shouldQuit() {
        return info->counter > 99;
    }

    void terminate() {
        info.reset(nullptr);
    }
}