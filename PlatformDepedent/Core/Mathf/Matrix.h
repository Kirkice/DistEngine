#pragma once
#include "Vector3.h"
#include "Vector4.h"

namespace Mathf
{
    struct float3x3
    {
        float _11, _12, _13;
        float _21, _22, _23;
        float _31, _32, _33;

        float3x3();
        explicit float3x3(const DirectX::XMFLOAT3X3& m);
        explicit float3x3(DirectX::CXMMATRIX m);
        float3x3(const Vector3& r0, const Vector3& r1, const Vector3& r2);

        float3x3& operator*=(const float3x3& other);
        float3x3 operator*(const float3x3& other) const;
        Vector3 operator* (Vector3 vec);

        Vector3 Up() const;
        Vector3 Down() const;
        Vector3 Left() const;
        Vector3 Right() const;
        Vector3 Forward() const;
        Vector3 Back() const;

        float Get(int row, int column) const;
        void SetXBasis(const Vector3& x);
        void SetYBasis(const Vector3& y);
        void SetZBasis(const Vector3& z);

        static float3x3 Transpose(const float3x3& m);
        static float3x3 Invert(const float3x3& m);
        static float3x3 ScaleMatrix(float s);
        static float3x3 ScaleMatrix(const Vector3& s);
        static float3x3 RotationAxisAngle(const Vector3& axis, float angle);
        static float3x3 RotationEuler(float x, float y, float z);
        static float3x3 SetFromToRotation(const Vector3& from, const Vector3& to);
        static bool LookRotationToMatrix(const Vector3& viewVec, const Vector3& upVec, float3x3* m);
        DirectX::XMMATRIX ToSIMD() const;
    };

    struct float4x4
    {
        float _11, _12, _13, _14;
        float _21, _22, _23, _24;
        float _31, _32, _33, _34;
        float _41, _42, _43, _44;

        float4x4();
        explicit float4x4(const DirectX::XMFLOAT4X4& m);
        explicit float4x4(DirectX::CXMMATRIX m);
        float4x4(const Vector4& r0, const Vector4& r1, const Vector4& r2, const Vector4& r3);

        float4x4& operator*=(const float4x4& other);
        float4x4 operator*(const float4x4& other) const;
        Vector3 operator* (Vector3 vec);
        Vector4 operator* (Vector4 vec);

        Vector3 Up() const;
        Vector3 Down() const;
        Vector3 Left() const;
        Vector3 Right() const;
        Vector3 Forward() const;
        Vector3 Back() const;

        Vector3 Translation() const;
        void SetTranslation(const Vector3& t);

        void SetXBasis(const Vector3& x);
        void SetYBasis(const Vector3& y);
        void SetZBasis(const Vector3& z);

        void Scale(const Vector3& scale);

        static float4x4 Transpose(const float4x4& m);
        static float4x4 Invert(const float4x4& m);
        static float4x4 RotationAxisAngle(const Vector3& axis, float angle);
        static float4x4 RotationEuler(float x, float y, float z);
        static float4x4 ScaleMatrix(float s);
        static float4x4 ScaleMatrix(const Vector3& s);
        static float4x4 TranslationMatrix(const Vector3& t);

        bool operator==(const float4x4& other) const;
        bool operator!=(const float4x4& other) const;

        DirectX::XMMATRIX ToSIMD() const;
        float3x3 To3x3() const;
    };

}