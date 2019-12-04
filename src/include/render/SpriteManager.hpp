#ifndef MSCFINALPROJECT_RENDER_SPRITEMANAGER_HPP
#define MSCFINALPROJECT_RENDER_SPRITEMANAGER_HPP

#include <cstdint>
#include <string>

namespace render::SpriteManager {
    bool init();
    uint32_t getID(const std::string& name);
    void bindImage(uint32_t id);
}

#endif//MSCFINALPROJECT_RENDER_SPRITEMANAGER_HPP