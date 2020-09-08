#ifndef MSCFINALPROJECT_RENDER_PUSHCONSTANTS_HPP
#define MSCFINALPROJECT_RENDER_PUSHCONSTANTS_HPP

#include <vml/mat4.hpp>

namespace render {
    struct push_constants {
        vml::mat4 p;
        vml::mat4 v;
        vml::mat4 m;
        vml::mat3 tt;
        vml::mat4 cm;
    };
}

#endif//MSCFINALPROJECT_RENDER_PUSHCONSTANTS_HPP
