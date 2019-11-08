#include "vml/vec2.hpp"

#include <cmath>

namespace vml
{
    vec2::vec2() : data{0.0f, 0.0f} {}
    vec2::vec2(float x, float y) : data{x, y} {}
    vec2::vec2(const vec2& v) : data{v[0], v[1]} {}

    vec2& vec2::operator=(const vec2& v) {
        this->data[0] = v[0];
        this->data[1] = v[1];
        return *this;
    }
    vec2& vec2::operator+=(const vec2& v) {
        return (*this = (*this + v));
    }
    vec2& vec2::operator-=(const vec2& v) {
        return (*this = (*this - v));
    }
    vec2& vec2::operator*=(float s) {
        return (*this = (*this * s));
    }
    vec2& vec2::operator/=(float s) {
        return (*this = (*this / s));
    }

    float& vec2::operator[](int i) {
        return this->data[i];
    }
    float const& vec2::operator[](int i) const {
        return this->data[i];
    }

    float vec2::magnitude() const {
        return std::sqrt(this->data[0]*this->data[0] + this->data[1]*this->data[1]);
    }

    vec2 operator+(const vec2& v) {
        return vec2(+v[0], +v[1]);
    }
    vec2 operator-(const vec2& v) {
        return vec2(-v[0], -v[1]);
    }

    vec2 operator+(const vec2& v0, const vec2& v1) {
        return vec2(v0[0] + v1[0], v0[1] + v1[1]);
    }
    vec2 operator-(const vec2& v0, const vec2& v1) {
        return vec2(v0[0] - v1[0], v0[1] - v1[1]);
    }

    vec2 operator*(const vec2& v, float s) {
        return vec2(v[0] * s, v[1] * s);
    }
    vec2 operator*(float s, const vec2& v) {
        return vec2(v[0] * s, v[1] * s);
    }
    vec2 operator/(const vec2& v, float s) {
        return vec2(v[0] / s, v[1] / s);
    }
}