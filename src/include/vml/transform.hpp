#ifndef MSCFINALPROJECT_VML_TRANSFORM_HPP
#define MSCFINALPROJECT_VML_TRANSFORM_HPP

#include "vml/mat3.hpp"
#include "vml/mat4.hpp"
#include "vml/quaternion.hpp"
#include "vml/vec3.hpp"

namespace vml {
    mat3 scaleM3(float s);
    mat3 scaleM3(const vec3& v);

    mat4 translateM4(const vec3& v);
    mat4 translateM4(const mat3& m, const vec3& v);

    mat4 rotateXM4(float rad);
    mat4 rotateYM4(float rad);
    mat4 rotateZM4(float rad);

    mat4 orthoM4(float left, float right, float bottom, float top, float near, float far);
    mat4 perspectiveM4(float aspectRatio, float fov, float near, float far);

    quaternion rotateQ(float rad, const vec3& axis);
    mat4 rotateM4(const quaternion& q);
}

#endif//MSCFINALPROJECT_VML_TRANSFORM_HPP