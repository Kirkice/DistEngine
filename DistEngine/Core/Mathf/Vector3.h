#pragma once
#include "../MathHelper.h"
#include "Vector2.h"

using namespace DirectX;

namespace Mathf
{
	struct Vector3
	{
		float x, y, z;

		Vector3();
		Vector3(float x);
		Vector3(float x, float y, float z);
		Vector3(Vector2 xy, float z);
		explicit Vector3(const DirectX::XMFLOAT3& xyz);
		explicit Vector3(DirectX::FXMVECTOR xyz);

		float operator[](unsigned int idx) const;
		Vector3& operator+=(const Vector3& other);
		Vector3 operator+(const Vector3& other) const;

		Vector3& operator+=(float other);
		Vector3 operator+(float other) const;

		Vector3& operator-=(const Vector3& other);
		Vector3 operator-(const Vector3& other) const;

		Vector3& operator-=(float s);
		Vector3 operator-(float s) const;

		Vector3& operator*=(const Vector3& other);
		Vector3 operator*(const Vector3& other) const;

		Vector3& operator*=(float s);
		Vector3 operator*(float s) const;

		Vector3& operator/=(const Vector3& other);
		Vector3 operator/(const Vector3& other) const;

		Vector3& operator/=(float s);
		Vector3 operator/(float s) const;

		bool operator==(const Vector3& other) const;
		bool operator!=(const Vector3& other) const;

		Vector3 operator-() const;

		DirectX::XMVECTOR ToSIMD() const;

		DirectX::XMFLOAT3 ToXMFLOAT3() const;

		Vector2 To2D() const;

		static float Dot(const Vector3& a, const Vector3& b);
		static Vector3 Cross(const Vector3& a, const Vector3& b);
		static Vector3 Normalize(const Vector3& a);
		static Vector3 Clamp(const Vector3& val, const Vector3& min, const Vector3& max);

		static float Distance(const Vector3& a, const Vector3& b);
		static float Length(const Vector3& v);

		float DistanceTo(const Vector3& other) const;
		float DistanceSquaredTo(const Vector3& other) const;

		Vector3 Abs() const;

		static Vector3 Min(const Vector3& a, const Vector3& b);
		static Vector3 Max(const Vector3& a, const Vector3& b);

		static bool Near3(const Vector3& a, const Vector3& b, float eps = 1e-3f);

		static void StoreVector3(Vector3* pDestination, Vector3 V);
		static void StoreVector3(Vector3* pDestination, XMVECTOR V); 
	};
}