#include "Game.hpp"

#include <memory>

namespace Game {
    namespace {
        struct Info {
            uint32_t counter;
        };
        std::unique_ptr<Info> info;
    }
    void init() {
        info = std::make_unique<Info>();
        info->counter = 0;
    }

    void render() {

    }
    void handleEvent() {

    }
    void update() {
        info->counter++;
    }

    bool shouldQuit() {
        return info->counter > 100;
    }
}