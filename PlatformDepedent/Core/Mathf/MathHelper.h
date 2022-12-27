//***************************************************************************************
// MathHelper.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Helper math class.
//***************************************************************************************

#pragma once
#include <Windows.h>
#include <DirectXMath.h>
#include <cstdint>
#include <math.h>

// Standard int typedefs
#include <stdint.h>
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef intptr_t intptr;
typedef uintptr_t uintptr;
typedef wchar_t wchar;
typedef uint32_t bool32;

namespace Mathf
{
    static const float Infinity = FLT_MAX;
    static const float Pi = 3.1415926535f;
    static const float Pi2 = 6.283185307f;
    static const float Pi_2 = 1.570796327f;
    static const float Pi_4 = 0.7853981635f;
    static const float InvPi = 0.318309886f;
    static const float InvPi2 = 0.159154943f;
    static const double EPSILON = 0.0000000001;

    // Returns random float in [0, 1).
    static float RandF()
    {
        return (float)(rand()) / (float)RAND_MAX;
    }

    // Returns random float in [a, b).
    static float RandF(float a, float b)
    {
        return a + RandF() * (b - a);
    }

    static int Rand(int a, int b)
    {
        return a + rand() % ((b - a) + 1);
    }

    template<typename T>
    static T Min(const T& a, const T& b)
    {
        return a < b ? a : b;
    }

    template<typename T>
    static T Max(const T& a, const T& b)
    {
        return a > b ? a : b;
    }

    template<typename T>
    static T Lerp(const T& a, const T& b, float t)
    {
        return a + (b - a) * t;
    }

    template<typename T>
    static T Clamp(const T& x, const T& low, const T& high)
    {
        return x < low ? low : (x > high ? high : x);
    }

    // Satuate clamp 0  1
    template<typename T> T Saturate(T val)
    {
        return Clamp<T>(val, T(0.0f), T(1.0f));
    }

    // ƽ��
    template<typename T> T Square(T x)
    {
        return x * x;
    }

    //���� < - > ��
    inline float DEG2RAD(const float x)
    {
        return ((x / 180.f) * Pi);
    }

    inline float DEG2RAD(const double x)
    {
        return ((x / 180) * (double)Pi);
    }

    inline float RAD2DEG(const float x)
    {
        return ((x / Pi) * 180.0f);
    }

    inline float RAD2DEG(const double x)
    {
        return ((x / (double)Pi) * 180.0);
    }

    inline float DEG2RAD_HALF(const float x)
    {
        return ((x * Pi) / 360.0f);
    }

    inline float DEG2RAD_HALF(const double x)
    {
        return ((x * (double)Pi) / 360.0);
    }

    // Returns the polar angle of the point (x,y) in [0, 2*PI).
    static float AngleFromXY(float x, float y);

    static DirectX::XMVECTOR SphericalToCartesian(float radius, float theta, float phi)
    {
        return DirectX::XMVectorSet(
                radius * sinf(phi) * cosf(theta),
                radius * cosf(phi),
                radius * sinf(phi) * sinf(theta),
                1.0f);
    }

    static DirectX::XMMATRIX InverseTranspose(DirectX::CXMMATRIX M)
    {
        DirectX::XMMATRIX A = M;
        A.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

        DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(A);
        return DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, A));
    }

    static DirectX::XMFLOAT4X4 Identity4x4()
    {
        static DirectX::XMFLOAT4X4 I(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);

        return I;
    }

    static DirectX::XMVECTOR RandUnitVec3();
    static DirectX::XMVECTOR RandHemisphereUnitVec3(DirectX::XMVECTOR n);

    // Returns true if the distance between f0 and f1 is smaller than epsilon
    inline bool CompareApproximately(float f0, float f1, float epsilon = 0.000001F)
    {
        float dist = (f0 - f1);
        dist = fabs(dist);
        return dist <= epsilon;
    }

    inline bool CompareApproximatelyD(double d0, double d1, double epsilon = 0.000001)
    {
        double dist = (d0 - d1);
        dist = fabs(dist);
        return dist <= epsilon;
    }

    inline int CompareFloatRobustSignUtility(float A)
    {
        // The sign bit of a number is the high bit.
        union
        {
            float f;
            int i;
        } u;
        u.f = A;
        return (u.i) & 0x80000000;
    }

    inline float Sign(float f)
    {
        if (f < 0.0F)
            return -1.0F;
        else
            return 1.0;
    }
}
