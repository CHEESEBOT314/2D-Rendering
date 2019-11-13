#include "vml/vec3.hpp"

#include <cmath>

namespace vml
{
    vec3::vec3() : data{0.0f, 0.0f, 0.0f} {}
    vec3::vec3(float x, float y, float z) : data{x, y, z} {}
    vec3::vec3(float x, const vec2& v) : data{x, v[0], v[1]} {}
    vec3::vec3(const vec2& v, float z) : data{v[0], v[1], z} {}
    vec3::vec3(const vec3& v) : data{v[0], v[1], v[2]} {}

    vec3& vec3::operator=(const vec3& v) {
        this->data[0] = v[0];
        this->data[1] = v[1];
        this->data[2] = v[2];
        return *this;
    }
    vec3& vec3::operator+=(const vec3& v) {
        return (*this = (*this + v));
    }
    vec3& vec3::operator-=(const vec3& v) {
        return (*this = (*this - v));
    }
    vec3& vec3::operator*=(float s) {
        return (*this = (*this * s));
    }
    vec3& vec3::operator/=(float s) {
        return (*this = (*this / s));
    }

    float& vec3::operator[](int i) {
        return this->data[i];
    }
    float const& vec3::operator[](int i) const {
        return this->data[i];
    }

    float vec3::magnitude() const {
        return std::sqrt(this->data[0]*this->data[0] + this->data[1]*this->data[1] + this->data[2]*this->data[2]);
    }

    vec3 operator+(const vec3& v) {
        return vec3(+v[0], +v[1], +v[2]);
    }
    vec3 operator-(const vec3& v) {
        return vec3(-v[0], -v[1], -v[2]);
    }

    vec3 operator+(const vec3& v0, const vec3& v1) {
        return vec3(v0[0] + v1[0], v0[1] + v1[1], v0[2] + v1[2]);
    }
    vec3 operator-(const vec3& v0, const vec3& v1) {
        return vec3(v0[0] - v1[0], v0[1] - v1[1], v0[2] - v1[2]);
    }

    vec3 operator*(const vec3& v, float s) {
        return vec3(v[0] * s, v[1] * s, v[2] * s);
    }
    vec3 operator*(float s, const vec3& v) {
        return vec3(v[0] * s, v[1] * s, v[2] * s);
    }
    vec3 operator/(const vec3& v, float s) {
        return vec3(v[0] / s, v[1] / s, v[2] / s);
    }
}