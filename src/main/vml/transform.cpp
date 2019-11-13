#include "vml/transform.hpp"

#define M_PI 3.1415926535897932384f
#include <cmath>

namespace vml {
    mat3 scaleM3(float s) {
        return mat3(
                 s,    0.0f, 0.0f,
            0.0f,      s,    0.0f,
            0.0f, 0.0f,      s   );
    }
    mat3 scaleM3(const vec3& v) {
        return mat3(
            v[0], 0.0f, 0.0f,
            0.0f, v[1], 0.0f,
            0.0f, 0.0f, v[2]);
    }

    mat4 translateM4(const vec3& v) {
        return mat4(
            1.0f,  0.0f,  0.0f,  0.0f,
            0.0f,  1.0f,  0.0f,  0.0f,
            0.0f,  0.0f,  1.0f,  0.0f,
            v[0],  v[1],  v[2],  1.0f);
    }
    mat4 translateM4(const mat3& m, const vec3& v) {
        mat4 out = mat4::extend(m);
        out[3][0] = v[0];
        out[3][1] = v[1];
        out[3][2] = v[2];
        return out;
    }

    mat4 rotateXM4(float rad) {
        float c = std::cos(rad);
        float s = std::sin(rad);
        return mat4(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, c, s, 0.0f,
                0.0f, -s, c, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);
    }
    mat4 rotateYM4(float rad) {
        float c = std::cos(rad);
        float s = std::sin(rad);
        return mat4(
                c, 0.0f, -s, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                s, 0.0f, c, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);
    }
    mat4 rotateZM4(float rad) {
        float c = std::cos(rad);
        float s = std::sin(rad);
        return mat4(
            c, s, 0.0f, 0.0f,
            -s, c, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);
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
        float s = 1.0f / std::tan(0.5f * M_PI * fovPiRad);
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
        float s = std::sin(rad / 2);
        return quaternion(std::cos(rad / 2), s * unit[0], s * unit[1], s * unit[2]);
    }
    mat4 rotateM4(const quaternion& q) {
        return mat4(
                1.0f-2.0f*(q[2]*q[2]+q[3]*q[3]), 2.0f*(q[1]*q[2]+q[3]*q[0]),      2.0f*(q[1]*q[3]-q[2]*q[0]),      0.0f,
                2.0f*(q[1]*q[2]-q[3]*q[0]),      1.0f-2.0f*(q[1]*q[1]+q[3]*q[3]), 2.0f*(q[2]*q[3]+q[1]*q[0]),      0.0f,
                2.0f*(q[1]*q[3]+q[2]*q[0]),      2.0f*(q[2]*q[3]-q[1]*q[0]),      1.0f-2.0f*(q[1]*q[1]+q[2]*q[2]), 0.0f,
                0.0f,                            0.0f,                            0.0f,                            1.0f);
    }
}