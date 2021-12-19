#pragma once
#include "../MathHelper.h"
#include "Matrix.h"
#include "Vector3.h"

namespace Mathf
{
	struct Quaternion
	{
		float x, y, z, w;

		Quaternion();
		Quaternion(float x, float y, float z, float w);
		Quaternion(const Vector3& axis, float angle);

		explicit Quaternion(const float3x3& m);
		explicit Quaternion(const DirectX::XMFLOAT4& q);
		explicit Quaternion(DirectX::FXMVECTOR q);

		Quaternion& operator*=(const Quaternion& other);
		Quaternion operator*(const Quaternion& other) const;

		bool operator==(const Quaternion& other) const;
		bool operator!=(const Quaternion& other) const;

		float3x3 ToFloat3x3() const;
		float4x4 ToFloat4x4() const;

		static Quaternion Identity();
		static Quaternion Invert(const Quaternion& q);
		static Quaternion FromAxisAngle(const Vector3& axis, float angle);
		static Quaternion FromEuler(float x, float y, float z);
		static Quaternion Normalize(const Quaternion& q);
		static float3x3 ToFloat3x3(const Quaternion& q);
		static float4x4 ToFloat4x4(const Quaternion& q);

		DirectX::XMVECTOR ToSIMD() const;
		DirectX::XMFLOAT4 ToXMFLOAT4() const;
	};
}