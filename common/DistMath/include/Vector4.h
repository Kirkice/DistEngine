
/* $Header: /common/distMath/include/Vector4.h	           8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Vector4.h												  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/

#pragma once
#include "../../Core/include/PCH.h"
#include "../../Core/include/Assert.h"
#include "Vector3.h"
#include "Vector2.h"

namespace math
{
	struct Vector4
	{
		float x, y, z, w;

		Vector4();
		Vector4(float x);
		Vector4(float x, float y, float z, float w);
		explicit Vector4(const Vector3& xyz, float w = 0.0f);
		explicit Vector4(const DirectX::XMFLOAT4& xyzw);
		explicit Vector4(DirectX::FXMVECTOR xyzw);

		Vector4& operator+=(const Vector4& other);
		Vector4 operator+(const Vector4& other) const;

		Vector4& operator-=(const Vector4& other);
		Vector4 operator-(const Vector4& other) const;

		Vector4& operator*=(const Vector4& other);
		Vector4 operator*(const Vector4& other) const;

		Vector4& operator/=(const Vector4& other);
		Vector4 operator/(const Vector4& other) const;

		bool operator==(const Vector4& other) const;
		bool operator!=(const Vector4& other) const;

		Vector4 operator-() const;

		DirectX::XMVECTOR ToSIMD() const;
		Vector3 To3D() const;
		Vector2 To2D() const;

		static Vector4 Clamp(const Vector4& val, const Vector4& min, const Vector4& max);
		static Vector4 Transform(const Vector4& v, const Float4x4& m);


		// Make Abs vector
		Vector4 Abs() const;

		// Calculate distance to other point
		float DistanceTo(const Vector4& other) const;

		// Calculate squared distance to other point
		float DistanceSquaredTo(const Vector4& other) const;

		// Calculate 2D distance to other point
		float DistanceTo2D(const Vector4& other) const;

		// Calculate 2D squared distance to other point
		float DistanceSquaredTo2D(const Vector4& other) const;

		// Normalization
		float Normalize2();
		float Normalize3();
		Vector4 Normalized2() const;
		Vector4 Normalized3() const;

		// Binary vector-vector operations
		static Vector4 Add3(const Vector4& a, const Vector4& b);
		static Vector4 Sub3(const Vector4& a, const Vector4& b);
		static Vector4 Mul3(const Vector4& a, const Vector4& b);

		// Inplace vector operators
		Vector4& Add3(const Vector4& a);
		Vector4& Sub3(const Vector4& a);

		// Inplace scalar operators
		Vector4& Mul3(float a);

		float Pitch() const;
		float Yaw() const;

		// Interpolate
		static Vector4 Interpolate(const Vector4& a, const Vector4& b, const float weight);
		void Interpolate(const Vector4& a, const float weight);

		// Some predefined vectors
		static Vector4 ZEROS();
		static Vector4 ZERO_3D_POINT();
		static Vector4 ONES();
		static Vector4 PLUS_MAX();
		static Vector4 MINUS_MAX();
		static Vector4 PLUS_INF();
		static Vector4 MINUS_INF();
		static Vector4 EX();
		static Vector4 EY();
		static Vector4 EZ();
		static Vector4 EW();
		static Vector4 FRONT();
		static Vector4 RIGHT();
		static Vector4 UP();
	};
}