
/* $Header: /common/distMath/src/Vector3.cpp	           8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Vector3.cpp												  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/

#include "../include/Vector3.h"
#include "../include/DistMathPublic.h"

using namespace DirectX;
using namespace DirectX::PackedVector;

namespace math
{
	Vector3::Vector3()
	{
		x = y = z = 0.0f;
	}

	Vector3::Vector3(float x_)
	{
		x = y = z = x_;
	}

	Vector3::Vector3(float x_, float y_, float z_)
	{
		x = x_;
		y = y_;
		z = z_;
	}

	Vector3::Vector3(Vector2 xy, float z_)
	{
		x = xy.x;
		y = xy.y;
		z = z_;
	}

	Vector3::Vector3(const XMFLOAT3& xyz)
	{
		x = xyz.x;
		y = xyz.y;
		z = xyz.z;
	}

	Vector3::Vector3(FXMVECTOR xyz)
	{
		XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(this), xyz);
	}

	float Vector3::operator[](unsigned int idx) const
	{
		assert(idx < 3);
		return *(&x + idx);
	}

	Vector3& Vector3::operator+=(const Vector3& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	Vector3 Vector3::operator+(const Vector3& other) const
	{
		Vector3 result;
		result.x = x + other.x;
		result.y = y + other.y;
		result.z = z + other.z;
		return result;
	}

	Vector3& Vector3::operator+=(float s)
	{
		x += s;
		y += s;
		z += s;
		return *this;
	}

	Vector3 Vector3::operator+(float s) const
	{
		Vector3 result;
		result.x = x + s;
		result.y = y + s;
		result.z = z + s;
		return result;
	}

	Vector3& Vector3::operator-=(const Vector3& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	Vector3 Vector3::operator-(const Vector3& other) const
	{
		Vector3 result;
		result.x = x - other.x;
		result.y = y - other.y;
		result.z = z - other.z;
		return result;
	}

	Vector3& Vector3::operator-=(float s)
	{
		x -= s;
		y -= s;
		z -= s;
		return *this;
	}

	Vector3 Vector3::operator-(float s) const
	{
		Vector3 result;
		result.x = x - s;
		result.y = y - s;
		result.z = z - s;
		return result;
	}


	Vector3& Vector3::operator*=(const Vector3& other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}

	Vector3 Vector3::operator*(const Vector3& other) const
	{
		Vector3 result;
		result.x = x * other.x;
		result.y = y * other.y;
		result.z = z * other.z;
		return result;
	}

	Vector3& Vector3::operator*=(float s)
	{
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	Vector3 Vector3::operator*(float s) const
	{
		Vector3 result;
		result.x = x * s;
		result.y = y * s;
		result.z = z * s;
		return result;
	}

	Vector3& Vector3::operator/=(const Vector3& other)
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;
		return *this;
	}

	Vector3 Vector3::operator/(const Vector3& other) const
	{
		Vector3 result;
		result.x = x / other.x;
		result.y = y / other.y;
		result.z = z / other.z;
		return result;
	}

	Vector3& Vector3::operator/=(float s)
	{
		x /= s;
		y /= s;
		z /= s;
		return *this;
	}

	Vector3 Vector3::operator/(float s) const
	{
		Vector3 result;
		result.x = x / s;
		result.y = y / s;
		result.z = z / s;
		return result;
	}

	bool Vector3::operator==(const Vector3& other) const
	{
		return x == other.x && y == other.y && z == other.z;
	}

	bool Vector3::operator!=(const Vector3& other) const
	{
		return x != other.x || y != other.y || z != other.z;
	}

	Vector3 Vector3::operator-() const
	{
		Vector3 result;
		result.x = -x;
		result.y = -y;
		result.z = -z;

		return result;
	}

	Vector3 operator*(float a, const Vector3& b)
	{
		return Vector3(a * b.x, a * b.y, a * b.z);
	}

	XMVECTOR Vector3::ToSIMD() const
	{
		return XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(this));
	}

	DirectX::XMFLOAT3 Vector3::ToXMFLOAT3() const
	{
		return XMFLOAT3(x, y, z);
	}

	Vector2 Vector3::To2D() const
	{
		return Vector2(x, y);
	}

	float Vector3::Length() const
	{
		return Vector3::Length(*this);
	}

	float Vector3::Dot(const Vector3& a, const Vector3& b)
	{
		return XMVectorGetX(XMVector3Dot(a.ToSIMD(), b.ToSIMD()));
	}

	Vector3 Vector3::Cross(const Vector3& a, const Vector3& b)
	{
		Vector3 result;
		XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&result), XMVector3Cross(a.ToSIMD(), b.ToSIMD()));
		return result;
	}

	Vector3 Vector3::Normalize(const Vector3& a)
	{
		Vector3 result;
		XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&result), XMVector3Normalize(a.ToSIMD()));
		return result;
	}

	Vector3 Vector3::Transform(const Vector3& v, const Float3x3& m)
	{
		XMVECTOR vec = v.ToSIMD();
		vec = XMVector3TransformCoord(vec, m.ToSIMD());
		return Vector3(vec);
	}

	Vector3 Vector3::Transform(const Vector3& v, const Float4x4& m)
	{
		XMVECTOR vec = v.ToSIMD();
		vec = XMVector3TransformCoord(vec, m.ToSIMD());
		return Vector3(vec);
	}

	Vector3 Vector3::TransformDirection(const Vector3& v, const Float4x4& m)
	{
		XMVECTOR vec = v.ToSIMD();
		vec = XMVector3TransformNormal(vec, m.ToSIMD());
		return Vector3(vec);
	}

	Vector3 Vector3::Transform(const Vector3& v, const Quaternion& q)
	{
		return Vector3::Transform(v, q.ToFloat3x3());
	}

	Vector3 Vector3::Clamp(const Vector3& val, const Vector3& min, const Vector3& max)
	{
		Vector3 retVal;
		retVal.x = math::Clamp(val.x, min.x, max.x);
		retVal.y = math::Clamp(val.y, min.y, max.y);
		retVal.z = math::Clamp(val.z, min.z, max.z);
		return retVal;
	}

	Vector3 Vector3::Perpendicular(const Vector3& vec)
	{
		Assert_(vec.Length() >= 0.00001f);

		Vector3 perp;

		float x = std::abs(vec.x);
		float y = std::abs(vec.y);
		float z = std::abs(vec.z);
		float minVal = std::min(x, y);
		minVal = std::min(minVal, z);

		if (minVal == x)
			perp = Vector3::Cross(vec, Vector3(1.0f, 0.0f, 0.0f));
		else if (minVal == y)
			perp = Vector3::Cross(vec, Vector3(0.0f, 1.0f, 0.0f));
		else
			perp = Vector3::Cross(vec, Vector3(0.0f, 0.0f, 1.0f));

		return Vector3::Normalize(perp);
	}

	float Vector3::Distance(const Vector3& a, const Vector3& b)
	{
		XMVECTOR x = a.ToSIMD();
		XMVECTOR y = b.ToSIMD();
		XMVECTOR length = XMVector3Length(XMVectorSubtract(x, y));
		return XMVectorGetX(length);
	}

	float Vector3::Length(const Vector3& v)
	{
		XMVECTOR x = v.ToSIMD();
		XMVECTOR length = XMVector3Length(x);
		return XMVectorGetX(length);
	}

	float Vector3::DistanceTo(const Vector3& other) const
	{
		Vector3 delta = *this - other;
		return sqrtf(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
	}

	float Vector3::DistanceSquaredTo(const Vector3& other) const
	{
		Vector3 delta = *this - other;
		return delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
	}

	float Vector3::Pitch() const
	{
		return (!x && !y) ? ((z > 0) ? 90.0f : -90.0f) : RAD2DEG(atan2f(-z, sqrtf(x * x + y * y)));
	}

	float Vector3::Yaw() const
	{
		if (fabsf(x) < 0.0001f && fabsf(y) < 0.0001f)
			return 0.0f;

		return RAD2DEG(-atan2f(x, y));
	}

	Vector3 Vector3::Abs() const
	{
		return{ ::fabsf(x), ::fabsf(y), ::fabsf(z) };
	}

	Vector3 Vector3::Min(const Vector3& v1, const Vector3& v2)
	{
		return { math::Min(v1.x, v2.x), math::Min(v1.y, v2.y), math::Min(v1.z, v2.z) };
	}

	Vector3 Vector3::Max(const Vector3& v1, const Vector3& v2)
	{
		return { math::Max(v1.x, v2.x), math::Max(v1.y, v2.y), math::Max(v1.z, v2.z) };
	}

	bool Vector3::Near3(const Vector3& a, const Vector3& b, float eps/*=1e-3f*/)
	{
		return fabsf(a.x - b.x) < eps && fabsf(a.y - b.y) < eps && fabsf(a.z - b.z) < eps;
	}

	Vector3 Vector3::EX()
	{
		return { 1.f, 0.f, 0.f };
	}

	Vector3 Vector3::EY()
	{
		return { 0.f, 1.f, 0.f };
	}

	Vector3 Vector3::EZ()
	{
		return { 0.f, 0.f, 1.f };
	}

	Vector3 Vector3::ONES()
	{
		return { 1.f, 1.f, 1.f };
	}

	Vector3 Vector3::ZEROS()
	{
		return { 0.f, 0.f, 0.f };
	}

	Vector3 Vector3::PLUS_MAX()
	{
		return { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
	}

	Vector3 Vector3::MINUS_MAX()
	{
		return { -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max() };
	}

	Vector3 Vector3::PLUS_INF()
	{
		return { std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity() };
	}

	Vector3 Vector3::MINUS_INF()
	{
		return { -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity() };
	}
}