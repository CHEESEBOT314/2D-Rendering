#include "vml/mat3.hpp"

namespace vml {
    mat3::mat3() : cols{vec3(), vec3(), vec3()} {}
    mat3::mat3(float c00, float c01, float c02,
               float c10, float c11, float c12,
               float c20, float c21, float c22)
                : cols{vec3(c00, c01, c02), vec3(c10, c11, c12), vec3(c20, c21, c22)} {}
    mat3::mat3(const vec3& c0, const vec3& c1, const vec3& c2) : cols{c0, c1, c2} {}
    mat3::mat3(const mat3& m) : cols{m[0], m[1], m[2]} {}

    mat3& mat3::operator=(const mat3& m) {
        this->cols[0] = m[0];
        this->cols[1] = m[1];
        this->cols[2] = m[2];
        return *this;
    }
    mat3& mat3::operator*=(const mat3& m) {
        return (*this = (*this * m));
    }
    mat3& mat3::operator*=(float s) {
        return (*this = (*this * s));
    }
    mat3& mat3::operator/=(float s) {
        return (*this = (*this / s));
    }

    vec3& mat3::operator[](int i) {
        return this->cols[i];
    }
    vec3 const& mat3::operator[](int i) const {
        return this->cols[i];
    }

    mat3 mat3::identity() {
        return mat3(
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f);
    }
    mat3 mat3::extend(const mat2& m) {
        return mat3(
            m[0][0], m[0][1], 0.0f,
            m[1][0], m[1][1], 0.0f,
            0.0f,    0.0f,    1.0f);
    }

    mat3 operator+(const mat3& m) {
        return mat3(+m[0], +m[1], +m[2]);
    }
    mat3 operator-(const mat3& m) {
        return mat3(-m[0], -m[1], -m[2]);
    }

    mat3 operator*(const mat3& m0, const mat3& m1) {
        return mat3(
            m0[0][0]*m1[0][0] + m0[1][0]*m1[0][1] + m0[2][0]*m1[0][2],
            m0[0][1]*m1[0][0] + m0[1][1]*m1[0][1] + m0[2][1]*m1[0][2],
            m0[0][2]*m1[0][0] + m0[1][2]*m1[0][1] + m0[2][2]*m1[0][2],

            m0[0][0]*m1[1][0] + m0[1][0]*m1[1][1] + m0[2][0]*m1[1][2],
            m0[0][1]*m1[1][0] + m0[1][1]*m1[1][1] + m0[2][1]*m1[1][2],
            m0[0][2]*m1[1][0] + m0[1][2]*m1[1][1] + m0[2][2]*m1[1][2],

            m0[0][0]*m1[2][0] + m0[1][0]*m1[2][1] + m0[2][0]*m1[2][2],
            m0[0][1]*m1[2][0] + m0[1][1]*m1[2][1] + m0[2][1]*m1[2][2],
            m0[0][2]*m1[2][0] + m0[1][2]*m1[2][1] + m0[2][2]*m1[2][2]);
    }
    mat3 operator*(const mat3& m, float s) {
        return mat3(m[0] * s, m[1] * s, m[2] * s);
    }
    mat3 operator*(float s, const mat3& m) {
        return mat3(m[0] * s, m[1] * s, m[2] * s);
    }
    vec3 operator*(const mat3& m, const vec3& v) {
        return vec3(
            m[0][0]*v[0] + m[1][0]*v[1] + m[2][0]*v[2],
            m[0][1]*v[0] + m[1][1]*v[1] + m[2][1]*v[2],
            m[0][2]*v[0] + m[1][2]*v[1] + m[2][2]*v[2]);
    }
    mat3 operator/(const mat3& m, float s) {
        return mat3(m[0] / s, m[1] / s, m[2] / s);
    }
}