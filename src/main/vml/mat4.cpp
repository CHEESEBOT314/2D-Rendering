#include "vml/mat4.hpp"

namespace vml {
    mat4::mat4() : c0(), c1(), c2(), c3() {}
    mat4::mat4(float c00, float c01, float c02, float c03,
               float c10, float c11, float c12, float c13,
               float c20, float c21, float c22, float c23,
               float c30, float c31, float c32, float c33)
                : c0(c00, c01, c02, c03), c1(c10, c11, c12, c13), c2(c20, c21, c22, c23), c3(c30, c31, c32, c33) {}
    mat4::mat4(const vec4& c0, const vec4& c1, const vec4& c2, const vec4& c3) : c0(c0), c1(c1), c2(c2), c3(c3) {}
    mat4::mat4(const mat4& m) : c0(m.c0), c1(m.c1), c2(m.c2), c3(m.c3) {}

    mat4& mat4::operator=(const mat4& m) {
        this->c0 = m.c0;
        this->c1 = m.c1;
        this->c2 = m.c2;
        this->c3 = m.c3;
        return *this;
    }
    mat4& mat4::operator*=(const mat4& m) {
        return (*this = (*this * m));
    }
    mat4& mat4::operator*=(float s) {
        return (*this = (*this * s));
    }
    mat4& mat4::operator/=(float s) {
        return (*this = (*this / s));
    }

    vec4& mat4::operator[](int i) {
        return this->cols[i];
    }
    vec4 const& mat4::operator[](int i) const {
        return this->cols[i];
    }

    mat4 mat4::identity() {
        return mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);
    }
    mat4 mat4::extend(const mat2& m) {
        return mat4(
            m.c0.x, m.c0.y, 0.0f, 0.0f,
            m.c1.x, m.c1.y, 0.0f, 0.0f,
            0.0f,   0.0f,   1.0f, 0.0f,
            0.0f,   0.0f,   0.0f, 1.0f);
    }
    mat4 mat4::extend(const mat3& m) {
        return mat4(
            m.c0.x, m.c0.y, m.c0.z, 0.0f,
            m.c1.x, m.c1.y, m.c1.z, 0.0f,
            m.c2.x, m.c2.y, m.c2.z, 0.0f,
            0.0f,   0.0f,   0.0f,   1.0f);
    }

    mat4 operator+(const mat4& m) {
        return mat4(+m.c0, +m.c1, +m.c2, +m.c3);
    }
    mat4 operator-(const mat4& m) {
        return mat4(-m.c0, -m.c1, -m.c2, -m.c3);
    }

    mat4 operator*(const mat4& m0, const mat4& m1) {
        return mat4(
            m0[0][0]*m1[0][0] + m0[1][0]*m1[0][1] + m0[2][0]*m1[0][2] + m0[3][0]*m1[0][3],
            m0[0][1]*m1[0][0] + m0[1][1]*m1[0][1] + m0[2][1]*m1[0][2] + m0[3][1]*m1[0][3],
            m0[0][2]*m1[0][0] + m0[1][2]*m1[0][1] + m0[2][2]*m1[0][2] + m0[3][2]*m1[0][3],
            m0[0][3]*m1[0][0] + m0[1][3]*m1[0][1] + m0[2][3]*m1[0][2] + m0[3][3]*m1[0][3],

            m0[0][0]*m1[1][0] + m0[1][0]*m1[1][1] + m0[2][0]*m1[1][2] + m0[3][0]*m1[1][3],
            m0[0][1]*m1[1][0] + m0[1][1]*m1[1][1] + m0[2][1]*m1[1][2] + m0[3][1]*m1[1][3],
            m0[0][2]*m1[1][0] + m0[1][2]*m1[1][1] + m0[2][2]*m1[1][2] + m0[3][2]*m1[1][3],
            m0[0][3]*m1[1][0] + m0[1][3]*m1[1][1] + m0[2][3]*m1[1][2] + m0[3][3]*m1[1][3],

            m0[0][0]*m1[2][0] + m0[1][0]*m1[2][1] + m0[2][0]*m1[2][2] + m0[3][0]*m1[2][3],
            m0[0][1]*m1[2][0] + m0[1][1]*m1[2][1] + m0[2][1]*m1[2][2] + m0[3][1]*m1[2][3],
            m0[0][2]*m1[2][0] + m0[1][2]*m1[2][1] + m0[2][2]*m1[2][2] + m0[3][2]*m1[2][3],
            m0[0][3]*m1[2][0] + m0[1][3]*m1[2][1] + m0[2][3]*m1[2][2] + m0[3][3]*m1[2][3],

            m0[0][0]*m1[3][0] + m0[1][0]*m1[3][1] + m0[2][0]*m1[3][2] + m0[3][0]*m1[3][3],
            m0[0][1]*m1[3][0] + m0[1][1]*m1[3][1] + m0[2][1]*m1[3][2] + m0[3][1]*m1[3][3],
            m0[0][2]*m1[3][0] + m0[1][2]*m1[3][1] + m0[2][2]*m1[3][2] + m0[3][2]*m1[3][3],
            m0[0][3]*m1[3][0] + m0[1][3]*m1[3][1] + m0[2][3]*m1[3][2] + m0[3][3]*m1[3][3]);
    }
    mat4 operator*(const mat4& m, float s) {
        return mat4(m.c0 * s, m.c1 * s, m.c2 * s, m.c3 * s);
    }
    mat4 operator*(float s, const mat4& m) {
        return mat4(m.c0 * s, m.c1 * s, m.c2 * s, m.c3 * s);
    }
    vec4 operator*(const mat4& m, const vec4& v) {
        return vec4(
            m[0][0]*v[0] + m[1][0]*v[1] + m[2][0]*v[2] + m[3][0]*v[3],
            m[0][1]*v[0] + m[1][1]*v[1] + m[2][1]*v[2] + m[3][1]*v[3],
            m[0][2]*v[0] + m[1][2]*v[1] + m[2][2]*v[2] + m[3][2]*v[3],
            m[0][3]*v[0] + m[1][3]*v[1] + m[2][3]*v[2] + m[3][3]*v[3]);
    }
    mat4 operator/(const mat4& m, float s) {
        return mat4(m.c0 / s, m.c1 / s, m.c2 / s, m.c3 / s);
    }
}