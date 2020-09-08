#ifndef MSCFINALPROJECT_RENDER_SPRITEMANAGER_HPP
#define MSCFINALPROJECT_RENDER_SPRITEMANAGER_HPP

#include <string>

namespace render::sprite_manager {
    bool init();
    uint32_t get_sprite(const std::string& name);
    void bind_sprite(uint32_t sprite);
}

#endif//MSCFINALPROJECT_RENDER_SPRITEMANAGER_HPP