#include "vml/transform.hpp"

#include <cmath>

namespace vml {
    mat3 scaleM3(float s) {
        return mat3(
            s,    0.0f, 0.0f,
            0.0f, s,    0.0f,
            0.0f, 0.0f, s   );
    }
    mat3 scaleM3(const vec3& v) {
        return mat3(
            v.x,  0.0f, 0.0f,
            0.0f, v.y,  0.0f,
            0.0f, 0.0f, v.z );
    }

    mat4 translateM4(const vec3& v) {
        return mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            v.x,  v.y,  v.z,  1.0f);
    }
    mat4 translateM4(const mat3& m, const vec3& v) {
        mat4 out = mat4::extend(m);
        out[3][0] = v[0];
        out[3][1] = v[1];
        out[3][2] = v[2];
        return out;
    }

    mat4 orthoM4(float left, float right, float bottom, float top, float near, float far) {
        mat4 out;
        out[0][0] = 2.0f / (right - left);
        out[3][0] = (left + right) / (left - right);
        out[1][1] = 2.0f / (bottom - top);
        out[3][1] = (top + bottom) / (top - bottom);
        out[2][2] = 1.0f / (far - near);
        out[3][2] = near / (near - far);
        out[3][3] = 1.0f;
        return out;
    }
    mat4 perspectiveM4(float aspectRatio, float fovPiRad, float near, float far) {
        float s = 1.0f / (float)tan(0.5f * M_PI * fovPiRad);
        mat4 out;
        out[0][0] = s;
        out[1][1] = -s * aspectRatio;
        out[2][2] = far / (near - far);
        out[3][2] = near * far / (near - far);
        out[2][3] = -1.0f;
        return out;
    }

    quaternion rotateQ(float rad, const vec3& axis) {
        vec3 unit = axis / axis.magnitude();
        float s = sin(rad / 2);
        return quaternion(cos(rad / 2), s + unit.x, s * unit.y, s * unit. z);
    }
}