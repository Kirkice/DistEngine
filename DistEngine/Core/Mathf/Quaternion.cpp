#include "Quaternion.h"

using namespace DirectX;

namespace Mathf
{
	Quaternion::Quaternion()
	{
		*this = Quaternion::Identity();
	}

	Quaternion::Quaternion(float x_, float y_, float z_, float w_)
	{
		x = x_;
		y = y_;
		z = z_;
		w = w_;
	}

	Quaternion::Quaternion(const Vector3& axis, float angle)
	{
		*this = Quaternion::FromAxisAngle(axis, angle);
	}

	Quaternion::Quaternion(const float3x3& m)
	{
		*this = Quaternion(XMQuaternionRotationMatrix(m.ToSIMD()));
	}

	Quaternion::Quaternion(const XMFLOAT4& q)
	{
		x = q.x;
		y = q.y;
		z = q.z;
		w = q.w;
	}

	Quaternion::Quaternion(FXMVECTOR q)
	{
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(this), q);
	}

	Quaternion& Quaternion::operator*=(const Quaternion& other)
	{
		XMVECTOR q = ToSIMD();
		q = XMQuaternionMultiply(q, other.ToSIMD());
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(this), q);

		return *this;
	}

	Quaternion Quaternion::operator*(const Quaternion& other) const
	{
		Quaternion q = *this;
		q *= other;
		return q;
	}

	bool Quaternion::operator==(const Quaternion& other) const
	{
		return x == other.x && y == other.y && z == other.z && w == other.w;
	}

	bool Quaternion::operator!=(const Quaternion& other) const
	{
		return x != other.x || y != other.y || z != other.z || w != other.w;
	}

	float3x3 Quaternion::ToFloat3x3() const
	{
		return float3x3(XMMatrixRotationQuaternion(ToSIMD()));
	}

	float4x4 Quaternion::ToFloat4x4() const
	{
		return float4x4(XMMatrixRotationQuaternion(ToSIMD()));
	}

	Quaternion Quaternion::Identity()
	{
		return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
	}

	Quaternion Quaternion::Invert(const Quaternion& q)
	{
		return Quaternion(XMQuaternionInverse(q.ToSIMD()));
	}

	Quaternion Quaternion::FromAxisAngle(const Vector3& axis, float angle)
	{
		XMVECTOR q = XMQuaternionRotationAxis(axis.ToSIMD(), angle);
		return Quaternion(q);
	}

	Quaternion Quaternion::FromEuler(float x, float y, float z)
	{
		XMVECTOR q = XMQuaternionRotationRollPitchYaw(x, y, z);
		return Quaternion(q);
	}

	Quaternion Quaternion::Normalize(const Quaternion& q)
	{
		return Quaternion(XMQuaternionNormalize(q.ToSIMD()));
	}

	Quaternion Quaternion::Slerp(Quaternion a, Quaternion b, float t)
	{
		return Normalize(Quaternion(XMQuaternionSlerp(a.ToSIMD(), b.ToSIMD(), t)));
	}

	Quaternion Quaternion::Lerp(Quaternion a, Quaternion b, float t)
	{
		return Normalize(Quaternion(XMVectorLerp(a.ToSIMD(), b.ToSIMD(), t)));
	}

	float3x3 Quaternion::ToFloat3x3(const Quaternion& q)
	{
		return q.ToFloat3x3();
	}

	float4x4 Quaternion::ToFloat4x4(const Quaternion& q)
	{
		return q.ToFloat4x4();
	}

	XMVECTOR Quaternion::ToSIMD() const
	{
		return XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(this));
	}

	XMFLOAT4 Quaternion::ToXMFLOAT4() const
	{
		return XMFLOAT4(x, y, z, w);
	}
}