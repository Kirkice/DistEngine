#include "Vector3.h"


namespace Dist
{
	const Vector3 Vector3::zero = Vector3(0, 0, 0);
	const Vector3 Vector3::one = Vector3(1.0F, 1.0F, 1.0F);
	const Vector3 Vector3::xAxis = Vector3(1, 0, 0);
	const Vector3 Vector3::yAxis = Vector3(0, 1, 0);
	const Vector3 Vector3::zAxis = Vector3(0, 0, 1);

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
	 
	Vector3 Vector3::Clamp(const Vector3& val, const Vector3& min, const Vector3& max)
	{
		Vector3 retVal;
		retVal.x = Dist::Clamp(val.x, min.x, max.x);
		retVal.y = Dist::Clamp(val.y, min.y, max.y);
		retVal.z = Dist::Clamp(val.z, min.z, max.z);
		return retVal;
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

	Vector3 Vector3::Abs() const
	{
		return{ ::fabsf(x), ::fabsf(y), ::fabsf(z) };
	}

	Vector3 Vector3::Min(const Vector3& v1, const Vector3& v2)
	{
		return { Dist::Min(v1.x, v2.x), Dist::Min(v1.y, v2.y), Dist::Min(v1.z, v2.z) };
	}

	Vector3 Vector3::Max(const Vector3& v1, const Vector3& v2)
	{
		return { Dist::Max(v1.x, v2.x), Dist::Max(v1.y, v2.y), Dist::Max(v1.z, v2.z) };
	}

	bool Vector3::Near3(const Vector3& a, const Vector3& b, float eps/*=1e-3f*/)
	{
		return fabsf(a.x - b.x) < eps && fabsf(a.y - b.y) < eps && fabsf(a.z - b.z) < eps;
	}

	void Vector3::StoreVector3(Vector3* pDestination, Vector3 V)
	{
		XMStoreFloat3(&(pDestination->ToXMFLOAT3()), V.ToSIMD());
	}

	void Vector3::StoreVector3(Vector3* pDestination, XMVECTOR V)
	{
		XMStoreFloat3(&(pDestination->ToXMFLOAT3()), V); 
	}
}