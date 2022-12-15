#include "Vector4.h"

using namespace DirectX;

namespace Mathf
{
    Vector4::Vector4()
    {
        x = y = z = w = 0.0f;
    }

    Vector4::Vector4(float x_)
    {
        x = y = z = w = x_;
    }

    Vector4::Vector4(float x_, float y_, float z_, float w_)
    {
        x = x_;
        y = y_;
        z = z_;
        w = w_;
    }

    Vector4::Vector4(const Vector3& xyz, float w_)
    {
        x = xyz.x;
        y = xyz.y;
        z = xyz.z;
        w = w_;
    }

    Vector4::Vector4(const XMFLOAT4& xyzw)
    {
        x = xyzw.x;
        y = xyzw.y;
        z = xyzw.z;
        w = xyzw.w;
    }

    Vector4::Vector4(FXMVECTOR xyzw)
{
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(this), xyzw);
}

Vector4& Vector4::operator+=(const Vector4& other)
{
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;
    return *this;
}

Vector4 Vector4::operator+(const Vector4& other) const
{
    Vector4 result;
    result.x = x + other.x;
    result.y = y + other.y;
    result.z = z + other.z;
    result.w = w + other.w;
    return result;
}

Vector4& Vector4::operator-=(const Vector4& other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;
    return *this;
}

Vector4 Vector4::operator-(const Vector4& other) const
{
    Vector4 result;
    result.x = x - other.x;
    result.y = y - other.y;
    result.z = z - other.z;
    result.w = w - other.w;
    return result;
}

Vector4& Vector4::operator*=(const Vector4& other)
{
    x *= other.x;
    y *= other.y;
    z *= other.z;
    w *= other.w;
    return *this;
}

Vector4 Vector4::operator*(const Vector4& other) const
{
    Vector4 result;
    result.x = x * other.x;
    result.y = y * other.y;
    result.z = z * other.z;
    result.w = w * other.w;
    return result;
}

Vector4& Vector4::operator/=(const Vector4& other)
{
    x /= other.x;
    y /= other.y;
    z /= other.z;
    w /= other.w;
    return *this;
}

Vector4 Vector4::operator/(const Vector4& other) const
{
    Vector4 result;
    result.x = x / other.x;
    result.y = y / other.y;
    result.z = z / other.z;
    result.w = w / other.w;
    return result;
}

bool Vector4::operator==(const Vector4& other) const
{
    return x == other.x && y == other.y && z == other.z && w == other.w;
}

bool Vector4::operator!=(const Vector4& other) const
{
    return x != other.x || y != other.y || z != other.z || w != other.w;
}


Vector4 Vector4::operator-() const
{
    Vector4 result;
    result.x = -x;
    result.y = -y;
    result.z = -z;
    result.w = -w;

    return result;
}

XMVECTOR Vector4::ToSIMD() const
{
    return XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(this));
}

Vector3 Vector4::To3D() const
{
    return Vector3(x, y, z);
}

Vector2 Vector4::To2D() const
{
    return Vector2(x, y);
}

Vector4 Vector4::Clamp(const Vector4& val, const Vector4& min, const Vector4& max)
{
    Vector4 retVal;
    retVal.x = Mathf::Clamp(val.x, min.x, max.x);
    retVal.y = Mathf::Clamp(val.y, min.y, max.y);
    retVal.z = Mathf::Clamp(val.z, min.z, max.z);
    retVal.w = Mathf::Clamp(val.w, min.w, max.w);
    return retVal;
}

Vector4 Vector4::Abs() const
{
    return { ::fabsf(x), ::fabsf(y), ::fabsf(z), ::fabsf(w) };
}

float Vector4::DistanceTo(const Vector4& other) const
{
    Vector4 delta = *this - other;
    return sqrtf(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
}

float Vector4::DistanceTo2D(const Vector4& other) const
{
    Vector4 delta = *this - other;
    return sqrtf(delta.x * delta.x + delta.y * delta.y);
}

float Vector4::DistanceSquaredTo2D(const Vector4& other) const
{
    Vector2 delta = To2D() - other.To2D();
    return delta.x * delta.x + delta.y * delta.y;
}

float Vector4::Normalize2()
{
    float len = sqrtf(To2D().x * To2D().x + To2D().y * To2D().y);
    if (len != 0)
    {
        *this /= Vector4(len, len, 1.0f, 1.0f);
    }
    return len;
}

float Vector4::Normalize3()
{
    float len = sqrtf(To3D().x * To3D().x + To3D().y * To3D().y + To3D().z * To3D().z);
    if (len != 0)
    {
        *this /= Vector4(len, len, len, 1.0f);
    }
    return len;
}

Vector4 Vector4::Normalized2() const
{
    float len = sqrtf(To2D().x * To2D().x + To2D().y * To2D().y);
    if (len == 0)
    {
        return *this;
    }
    return *this / Vector4(len, len, 1.0f, 1.0f);
}

Vector4 Vector4::Normalized3() const
{
    float len = sqrtf(To3D().x * To3D().x + To3D().y * To3D().y + To3D().z * To3D().z);
    if (len == 0)
    {
        return *this;
    }
    return *this / Vector4(len, len, len, 1.0f);
}

Vector4 Vector4::Add3(const Vector4& a, const Vector4& b)
{
    return { a.x + b.x, a.y + b.y, a.z + b.z ,a.w};
}

Vector4 Vector4::Sub3(const Vector4& a, const Vector4& b)
{
    return { a.x - b.x, a.y - b.y, a.z - b.z, a.w };
}

Vector4 Vector4::Mul3(const Vector4& a, const Vector4& b)
{
    return { a.x * b.x, a.y * b.y, a.z * b.z , a.w };
}

Vector4& Vector4::Add3(const Vector4& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

Vector4& Vector4::Sub3(const Vector4& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

Vector4& Vector4::Mul3(const float f)
{
    x *= f;
    y *= f;
    z *= f;
    return *this;
}

}