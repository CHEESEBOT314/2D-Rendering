#ifndef MSCFINALPROJECT_RENDER_PUSHCONSTANTS_HPP
#define MSCFINALPROJECT_RENDER_PUSHCONSTANTS_HPP

#include "vml/mat4.hpp"

namespace render {
    struct PushConstants {
        vml::mat4 p;
        vml::mat4 v;
        vml::mat4 m;
        vml::mat3 textureTransform;
    };
}

#endif//MSCFINALPROJECT_RENDER_PUSHCONSTANTS_HPP
