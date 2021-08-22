
/* $Header: /common/distMath/include/Vector2.h	           8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Vector2.h												  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/

#pragma once
#include "../../Core/include/PCH.h"
#include "../../Core/include/Assert.h"

namespace math
{
	struct Vector2
	{
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

		static Vector2 Clamp(const Vector2& val, const Vector2& min, const Vector2& max);

		static float Length(const Vector2& val);

		float Dot(const Vector2& v) const;

		float CrossZ(const Vector2& vec) const;

		float Normalize();

		Vector2 Normalized() const;

		float DistanceTo(const Vector2& v) const;

		float DistanceSquaredTo(const Vector2& v) const;

		float Yaw() const;

		Vector2 Rotate(float radians) const;

		Vector2 Perpendicular()const;


		static Vector2 Min(const Vector2& a, const Vector2& b);
		static Vector2 Max(const Vector2& a, const Vector2& b);

		static Vector2 EX();
		static Vector2 EY();
		static Vector2 ONES();
		static Vector2 ZEROS();
		static Vector2 HALVES();
		static Vector2 PLUS_MAX();
		static Vector2 MINUS_MAX();
		static Vector2 PLUS_INF();
		static Vector2 MINUS_INF();
	};
}