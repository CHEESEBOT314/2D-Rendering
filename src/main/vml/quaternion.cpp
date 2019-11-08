#include "vml/quaternion.hpp"

namespace vml {
    quaternion::quaternion() : data{0.0f, 0.0f, 0.0f, 0.0f} {}
    quaternion::quaternion(float x, float y, float z, float w) : data{x, y, z, w} {}
    quaternion::quaternion(const quaternion& q) : data{q[0], q[1], q[2], q[3]} {}

    quaternion& quaternion::operator=(const quaternion& q) {
        this->data[0] = q[0];
        this->data[1] = q[1];
        this->data[2] = q[2];
        this->data[3] = q[3];
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

    quaternion quaternion::identity() {
        return quaternion(1.0f, 0.0f, 0.0f, 0.0f);
    }
    quaternion quaternion::inverse() {
        return quaternion(this->data[0], -this->data[1], -this->data[2], -this->data[3]);
    }
    vec4 quaternion::rotate(const vec4& v) {
        quaternion temp = *this * quaternion(0.0f, v[0], v[1], v[2]) * this->inverse();
        return vec4(temp[1], temp[2], temp[3], v[3]);
    }

    quaternion operator+(const quaternion& q) {
        return quaternion(+q[0], +q[1], +q[2], +q[3]);
    }
    quaternion operator-(const quaternion& q) {
        return quaternion(-q[0], -q[1], -q[2], -q[3]);
    }

    quaternion operator*(const quaternion& q1, const quaternion& q2) {
        return quaternion(
            q1[0]*q2[0] - q1[1]*q2[1] - q1[2]*q2[2] - q1[3]*q2[3],
            q1[0]*q2[1] + q1[1]*q2[0] + q1[2]*q2[3] - q1[3]*q2[2],
            q1[0]*q2[2] - q1[1]*q2[3] + q1[2]*q2[0] + q1[3]*q2[1],
            q1[0]*q2[3] + q1[1]*q2[3] - q1[1]*q2[1] + q1[3]*q2[0]);
    }
    quaternion operator*(const quaternion& q, float s) {
        return quaternion(q[0] * s, q[1] * s, q[2] * s, q[3] * s);
    }
    quaternion operator*(float s, const quaternion& q) {
        return quaternion(q[0] * s, q[1] * s, q[2] * s, q[3] * s);
    }
    quaternion operator/(const quaternion& q, float s) {
        return quaternion(q[0] / s, q[1] / s, q[2] / s, q[3] / s);
    }
}