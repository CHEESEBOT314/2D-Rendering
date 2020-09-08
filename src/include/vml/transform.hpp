#ifndef MSCFINALPROJECT_VML_TRANSFORM_HPP
#define MSCFINALPROJECT_VML_TRANSFORM_HPP

#include <vml/mat4.hpp>
#include <vml/quaternion.hpp>

namespace vml {
    mat3 scale(float s);
    mat3 scale(const vec3 &v);

    mat4 translate(const vec3 &v);
    mat4 translate(const mat3 &m, const vec3 &v);

    mat4 rotate_x(float rad);
    mat4 rotate_y(float rad);
    mat4 rotate_z(float rad);

    mat4 ortho(float left, float right, float bottom, float top, float near, float far);
    mat4 perspective(float aspectRatio, float fov, float near, float far);

    mat4 rotate(float rad, const vec3 &axis);
    mat4 rotate(const quaternion &q);
}

#endif//MSCFINALPROJECT_VML_TRANSFORM_HPP