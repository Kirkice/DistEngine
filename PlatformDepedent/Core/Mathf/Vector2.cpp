#include "Vector2.h"

using namespace DirectX;

namespace Mathf {
    Vector2::Vector2() {
        x = y = 0.0f;
    }

    Vector2::Vector2(float x_) {
        x = y = x_;
    }

    Vector2::Vector2(float x_, float y_) {
        x = x_;
        y = y_;
    }

    Vector2::Vector2(const XMFLOAT2 &xy) {
        x = xy.x;
        y = xy.y;
    }

    Vector2::Vector2(FXMVECTOR
    xy) {
    XMStoreFloat2(reinterpret_cast<XMFLOAT2*>(this), xy);
}

Vector2 &Vector2::operator+=(const Vector2 &other) {
    x += other.x;
    y += other.y;
    return *this;
}

Vector2 Vector2::operator+(const Vector2 &other) const {
    Vector2 result;
    result.x = x + other.x;
    result.y = y + other.y;
    return result;
}

Vector2 &Vector2::operator-=(const Vector2 &other) {
    x -= other.x;
    y -= other.y;
    return *this;
}

Vector2 Vector2::operator-(const Vector2 &other) const {
    Vector2 result;
    result.x = x - other.x;
    result.y = y - other.y;
    return result;
}

Vector2 &Vector2::operator*=(const Vector2 &other) {
    x *= other.x;
    y *= other.y;
    return *this;
}

Vector2 Vector2::operator*(const Vector2 &other) const {
    Vector2 result;
    result.x = x * other.x;
    result.y = y * other.y;
    return result;
}

Vector2 &Vector2::operator*=(float s) {
    x *= s;
    y *= s;
    return *this;
}

Vector2 Vector2::operator*(float s) const {
    Vector2 result;
    result.x = x * s;
    result.y = y * s;
    return result;
}

Vector2 &Vector2::operator/=(const Vector2 &other) {
    x /= other.x;
    y /= other.y;
    return *this;
}

Vector2 Vector2::operator/(const Vector2 &other) const {
    Vector2 result;
    result.x = x / other.x;
    result.y = y / other.y;
    return result;
}

Vector2 &Vector2::operator/=(float s) {
    x /= s;
    y /= s;
    return *this;
}

Vector2 Vector2::operator/(float s) const {
    Vector2 result;
    result.x = x / s;
    result.y = y / s;
    return result;
}

bool Vector2::operator==(const Vector2 &other) const {
    return x == other.x && y == other.y;
}

bool Vector2::operator!=(const Vector2 &other) const {
    return x != other.x || y != other.y;
}

Vector2 Vector2::operator-() const {
    Vector2 result;
    result.x = -x;
    result.y = -y;

    return result;
}

XMVECTOR Vector2::ToSIMD() const {
    return XMLoadFloat2(reinterpret_cast<const XMFLOAT2 *>(this));
}

DirectX::XMFLOAT2 Vector2::ToXMFLOAT2() const {
    return XMFLOAT2(x, y);
}

Vector2 Vector2::Clamp(const Vector2 &val, const Vector2 &min, const Vector2 &max) {
    Vector2 retVal;
    retVal.x = Mathf::Clamp(val.x, min.x, max.x);
    retVal.y = Mathf::Clamp(val.y, min.y, max.y);
    return retVal;
}

float Vector2::Length(const Vector2 &val) {
    return sqrtf(val.x * val.x + val.y * val.y);
}

float Vector2::Dot(const Vector2 &v) const {
    return x * v.x + y * v.y;
}

float Vector2::CrossZ(const Vector2 &v) const {
    return x * v.y - y * v.x;
}

float Vector2::Normalize() {
    const float length = sqrtf(x * x + y * y);

    if (length > 0)
        *this /= length;

    return length;
}

Vector2 Vector2::Normalized() const {
    const float length = sqrtf(x * x + y * y);

    if (length > 0)
        return {x / length, y / length};

    return *this;
}

float Vector2::DistanceTo(const Vector2 &v) const {
    return sqrtf((*this - v).x * (*this - v).x + (*this - v).y + (*this - v).y);
}

float Vector2::DistanceSquaredTo(const Vector2 &v) const {
    return (*this - v).x * (*this - v).x + (*this - v).y * (*this - v).y;
}

Vector2 Vector2::Rotate(float radians) const {
    const float s = sin(radians);
    const float c = cos(radians);
    return Vector2(c * x - s * y, s * x + c * y);
}

float *Vector2::Vector2Array(const Vector2 &v) {
    float *vec2Array;
    vec2Array = new float[2];
    vec2Array[0] = v.x;
    vec2Array[1] = v.y;
    return vec2Array;
}

Vector2 Vector2::Min(const Vector2 &v1, const Vector2 &v2) {
    return {Mathf::Min(v1.x, v2.x), Mathf::Min(v1.y, v2.y)};
}

Vector2 Vector2::Max(const Vector2 &v1, const Vector2 &v2) {
    return {Mathf::Max(v1.x, v2.x), Mathf::Max(v1.y, v2.y)};
}

void Vector2::StoreVector2(Vector2 *pDestination, Vector2 V) {
    XMStoreFloat2(&(pDestination->ToXMFLOAT2()), V.ToSIMD());
}

void Vector2::StoreVector2(Vector2 *pDestination, XMVECTOR V) {
    XMStoreFloat2(&(pDestination->ToXMFLOAT2()), V);
}

}