#include "vml/quaternion.hpp"

namespace vml {
    quaternion::quaternion() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    quaternion::quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    quaternion::quaternion(const quaternion& q) : x(q.x), y(q.y), z(q.z), w(q.w) {}

    quaternion& quaternion::operator=(const quaternion& q) {
        this->x = q.x;
        this->y = q.y;
        this->z = q.z;
        this->w = q.w;
        return *this;
    }
    quaternion& quaternion::operator*=(const quaternion& q) {
        return (*this = (*this * q));
    }
    quaternion& quaternion::operator*=(float s) {
        return (*this = (*this * s));
    }
    quaternion& quaternion::operator/=(float s) {
        return (*this = (*this / s));
    }

    float& quaternion::operator[](int i) {
        return this->data[i];
    }
    float const& quaternion::operator[](int i) const {
        return this->data[i];
    }

    static quaternion identity() {
        return quaternion(1.0f, 0.0f, 0.0f, 0.0f);
    }
    quaternion inverse() {
        return quaternion(q.x, -q.y, -q.z, -q.w);
    }
    vec4 rotate(const vec4& v) {
        quaternion temp = *this * quaternion(0.0f, v.x, v.y, v.z) * this->inverse();
        return vec4(temp.y, temp.z, temp.w, v.w);
    }

    quaternion operator+(const quaternion& q) {
        return quaternion(+q.x, +q.y, +q.z, +q.w);
    }
    quaternion operator-(const quaternion& q) {
        return quaternion(-q.x, -q.y, -q.z, -q.w);
    }

    quaternion operator*(const quaternion& q1, const quaternion& q2) {
        return quaternion(
            q1.x*q2.x - q1.y*q2.y - q1.z*q2.z - q1.w*q2.w,
            q1.x*q2.y + q1.y*q2.x + q1.z*q2.w - q1.w*q2.z,
            q1.x*q2.z - q1.y*q2.w + q1.z*q2.x + q1.w*q2.y,
            q1.x*q2.w + q1.y*q2.w - q1.y*q2.y + q1.w*q2.x);
    }
    quaternion operator*(const quaternion& q, float s) {
        return quaternion(q.x * s, q.y * s, q.z * s, q.w * s);
    }
    quaternion operator*(float s, const quaternion& q) {
        return quaternion(q.x * s, q.y * s, q.z * s, q.w * s);
    }
    quaternion operator/(const quaternion& q, float s) {
        return quaternion(q.x / s, q.y / s, q.z / s, q.w / s);
    }
}