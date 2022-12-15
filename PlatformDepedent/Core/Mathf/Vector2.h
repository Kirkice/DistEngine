#pragma once
#include "MathHelper.h"

using namespace DirectX;

namespace Mathf
{
    struct Vector2
    {
        /// <summary>
        /// ????Vector2 ???Щ??????
        /// </summary>
        float x, y;

        Vector2();
        Vector2(float x);
        Vector2(float x, float y);
        explicit Vector2(const DirectX::XMFLOAT2& xy);
        explicit Vector2(DirectX::FXMVECTOR xy);

        Vector2& operator+=(const Vector2& other);
        Vector2 operator+(const Vector2& other) const;

        Vector2& operator-=(const Vector2& other);
        Vector2 operator-(const Vector2& other) const;

        Vector2& operator*=(const Vector2& other);
        Vector2 operator*(const Vector2& other) const;

        Vector2& operator*=(float s);
        Vector2 operator*(float s) const;

        Vector2& operator/=(const Vector2& other);
        Vector2 operator/(const Vector2& other) const;

        Vector2& operator/=(float s);
        Vector2 operator/(float s) const;

        bool operator==(const Vector2& other) const;
        bool operator!=(const Vector2& other) const;

        Vector2 operator-() const;

        DirectX::XMVECTOR ToSIMD() const;

        DirectX::XMFLOAT2 ToXMFLOAT2() const;
        //???
        static Vector2 Clamp(const Vector2& val, const Vector2& min, const Vector2& max);
        //????
        static float Length(const Vector2& val);
        //???
        float Dot(const Vector2& v) const;
        //???
        float CrossZ(const Vector2& vec) const;
        //??????
        float Normalize();
        //???????????
        Vector2 Normalized() const;
        //???????????????
        float DistanceTo(const Vector2& v) const;
        //???????
        float DistanceSquaredTo(const Vector2& v) const;
        //???
        Vector2 Rotate(float radians) const;
        //?????
        float* Vector2Array(const Vector2& v);
        //???? ??С?
        static Vector2 Min(const Vector2& a, const Vector2& b);
        static Vector2 Max(const Vector2& a, const Vector2& b);

        static void StoreVector2(Vector2* pDestination, Vector2 V);

        static void StoreVector2(Vector2* pDestination, XMVECTOR V);
    };
}