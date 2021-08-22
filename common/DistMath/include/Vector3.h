
/* $Header: /common/distMath/include/Vector2.cpp	           8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Vector2.cpp												  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/

#pragma once
#include "../../Core/include/PCH.h"
#include "../../Core/include/Assert.h"
#include "Vector2.h"
#include "Matrix.h"
#include "Quaternion.h"
 
namespace math
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

		float Length() const;

		static float Dot(const Vector3& a, const Vector3& b);
		static Vector3 Cross(const Vector3& a, const Vector3& b);
		static Vector3 Normalize(const Vector3& a);
		static Vector3 Transform(const Vector3& v, const Float3x3& m);
		static Vector3 Transform(const Vector3& v, const Float4x4& m);
		static Vector3 TransformDirection(const Vector3& v, const Float4x4& m);
		static Vector3 Transform(const Vector3& v, const Quaternion& q);
		static Vector3 Clamp(const Vector3& val, const Vector3& min, const Vector3& max);
		static Vector3 Perpendicular(const Vector3& v);
		static float Distance(const Vector3& a, const Vector3& b);
		static float Length(const Vector3& v);

		float DistanceTo(const Vector3& other) const;
		float DistanceSquaredTo(const Vector3& other) const;

		float Pitch() const;
		float Yaw() const;

		Vector3 Abs() const;

		static Vector3 Min(const Vector3& a, const Vector3& b);
		static Vector3 Max(const Vector3& a, const Vector3& b);

		static bool Near3(const Vector3& a, const Vector3& b, float eps = 1e-3f);

		static Vector3 EX(); 
		static Vector3 EY();
		static Vector3 EZ();
		static Vector3 ONES();
		static Vector3 ZEROS();
		static Vector3 PLUS_MAX();
		static Vector3 MINUS_MAX();
		static Vector3 PLUS_INF();
		static Vector3 MINUS_INF();
	};

	// Non-member operators of Float3
	Vector3 operator*(float a, const Vector3& b);
}