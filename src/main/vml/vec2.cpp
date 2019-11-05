#include "vml/vec2.hpp"

namespace vml
{
    vec2::vec2() : x(0), y(0) {}
    vec2::vec2(float x, float y) : x(x), y(y) {}
    vec2::vec2(const vec2& v) : x(v.x), y(v.y) {}

    vec2& vec2::operator=(const vec2& v) {
        this->x = v.x;
        this->y = v.y;
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
        return sqrt(x*x + y*y);
    }

    vec2 operator+(const vec2& v) {
        return vec2(+v.x, +v.y);
    }
    vec2 operator-(const vec2& v) {
        return vec2(-v.x, -v.y);
    }

    vec2 operator+(const vec2& v0, const vec2& v1) {
        return vec2(v0.x + v1.x, v0.y + v1.y);
    }
    vec2 operator-(const vec2& v0, const vec2& v1) {
        return vec2(v0.x - v1.x, v0.y - v1.y);
    }

    vec2 operator*(const vec2& v, float s) {
        return vec2(v.x * s, v.y * s);
    }
    vec2 operator*(float s, const vec2& v) {
        return vec2(v.x * s, v.y * s);
    }
    vec2 operator/(const vec2& v, float s) {
        return vec2(v.x / s, v.y / s);
    }
};