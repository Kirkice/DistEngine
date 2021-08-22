
/* $Header: /common/distMath/src/Vector2.cpp	           8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Vector2.cpp												  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#include "../include/Vector2.h"
#include "../include/DistMathPublic.h"

using namespace DirectX;
using namespace DirectX::PackedVector;

namespace math
{
	Vector2::Vector2()
	{
		x = y = 0.0f;
	}

	Vector2::Vector2(float x_)
	{
		x = y = x_;
	}

	Vector2::Vector2(float x_, float y_)
	{
		x = x_;
		y = y_;
	}

	Vector2::Vector2(const XMFLOAT2& xy)
	{
		x = xy.x;
		y = xy.y;
	}

	Vector2::Vector2(FXMVECTOR xy)
	{
		XMStoreFloat2(reinterpret_cast<XMFLOAT2*>(this), xy);
	}

	Vector2& Vector2::operator+=(const Vector2& other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	Vector2 Vector2::operator+(const Vector2& other) const
	{
		Vector2 result;
		result.x = x + other.x;
		result.y = y + other.y;
		return result;
	}

	Vector2& Vector2::operator-=(const Vector2& other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	Vector2 Vector2::operator-(const Vector2& other) const
	{
		Vector2 result;
		result.x = x - other.x;
		result.y = y - other.y;
		return result;
	}

	Vector2& Vector2::operator*=(const Vector2& other)
	{
		x *= other.x;
		y *= other.y;
		return *this;
	}

	Vector2 Vector2::operator*(const Vector2& other) const
	{
		Vector2 result;
		result.x = x * other.x;
		result.y = y * other.y;
		return result;
	}

	Vector2& Vector2::operator*=(float s)
	{
		x *= s;
		y *= s;
		return *this;
	}

	Vector2 Vector2::operator*(float s) const
	{
		Vector2 result;
		result.x = x * s;
		result.y = y * s;
		return result;
	}

	Vector2& Vector2::operator/=(const Vector2& other)
	{
		x /= other.x;
		y /= other.y;
		return *this;
	}

	Vector2 Vector2::operator/(const Vector2& other) const
	{
		Vector2 result;
		result.x = x / other.x;
		result.y = y / other.y;
		return result;
	}

	Vector2& Vector2::operator/=(float s)
	{
		x /= s;
		y /= s;
		return *this;
	}

	Vector2 Vector2::operator/(float s) const
	{
		Vector2 result;
		result.x = x / s;
		result.y = y / s;
		return result;
	}

	bool Vector2::operator==(const Vector2& other) const
	{
		return x == other.x && y == other.y;
	}

	bool Vector2::operator!=(const Vector2& other) const
	{
		return x != other.x || y != other.y;
	}

	Vector2 Vector2::operator-() const
	{
		Vector2 result;
		result.x = -x;
		result.y = -y;

		return result;
	}

	XMVECTOR Vector2::ToSIMD() const
	{
		return XMLoadFloat2(reinterpret_cast<const XMFLOAT2*>(this));
	}

	Vector2 Vector2::Clamp(const Vector2& val, const Vector2& min, const Vector2& max)
	{
		Vector2 retVal;
		retVal.x = math::Clamp(val.x, min.x, max.x);
		retVal.y = math::Clamp(val.y, min.y, max.y);
		return retVal;
	}

	float Vector2::Length(const Vector2& val)
	{
		return std::sqrtf(val.x * val.x + val.y * val.y);
	}

	float Vector2::Dot(const Vector2& v) const
	{
		return x * v.x + y * v.y;
	}

	float Vector2::CrossZ(const Vector2& v) const
	{
		return x * v.y - y * v.x;
	}

	float Vector2::Normalize()
	{
		const float length = sqrtf(x * x + y * y);

		if (length > 0)
			*this /= length;

		return length;
	}

	Vector2 Vector2::Normalized() const
	{
		const float length = sqrtf(x * x + y * y);

		if (length > 0)
			return { x / length, y / length };

		return *this;
	}

	float Vector2::DistanceTo(const Vector2& v) const
	{
		return sqrtf((*this - v).x * (*this - v).x + (*this - v).y + (*this - v).y);
	}

	float Vector2::DistanceSquaredTo(const Vector2& v) const
	{
		return (*this - v).x * (*this - v).x + (*this - v).y * (*this - v).y;
	}

	float Vector2::Yaw() const
	{
		if (fabsf(x) < 0.0001f && fabsf(y) < 0.0001f)
			return 0.0f;

		return RAD2DEG(-atan2f(x, y));
	}

	Vector2 Vector2::Rotate(float radians) const
	{
		const float s = sin(radians);
		const float c = cos(radians);
		return Vector2(c * x - s * y, s * x + c * y);
	}

	Vector2 Vector2::Perpendicular()const
	{
		return Vector2(-y, x);
	}

	Vector2 Vector2::Min(const Vector2& v1, const Vector2& v2)
	{
		return { math::Min(v1.x, v2.x), math::Min(v1.y, v2.y) };
	}

	Vector2 Vector2::Max(const Vector2& v1, const Vector2& v2)
	{
		return { math::Max(v1.x, v2.x), math::Max(v1.y, v2.y) };
	}

	Vector2 Vector2::EX()
	{
		return { 1.f, 0.f };
	}

	Vector2 Vector2::EY()
	{
		return { 0.f, 1.f };
	}

	Vector2 Vector2::ONES()
	{
		return { 1.f, 1.f };
	}

	Vector2 Vector2::ZEROS()
	{
		return { 0.f, 0.f };
	}

	Vector2 Vector2::HALVES()
	{
		return{ 0.5f, 0.5f };
	}

	 Vector2 Vector2::PLUS_MAX()
	{
		return { std::numeric_limits<float>::max() };
	}

	Vector2 Vector2::MINUS_MAX()
	{
		return { -std::numeric_limits<float>::max() };
	}

	Vector2 Vector2::PLUS_INF()
	{
		return{ std::numeric_limits<float>::infinity() };
	}

	Vector2 Vector2::MINUS_INF()
	{
		return{ -std::numeric_limits<float>::infinity() };
	}
}