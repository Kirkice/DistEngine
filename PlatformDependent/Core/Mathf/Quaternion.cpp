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

	Quaternion Quaternion::LookRotation(const Vector3& forward, const Vector3& upwards)
	{
		Quaternion q = Quaternion::Identity();
		if (!LookRotationToQuaternion(forward, upwards, &q))
		{
			const float mag = Vector3::Magnitude(forward);
			if (mag > EPSILON)
			{
				float3x3 m; 
				m.SetFromToRotation(Vector3::zAxis, forward / mag);
				MatrixToQuaternion(m, q);
			}
		}
		return q;
	}

	void Quaternion::MatrixToQuaternion(const float3x3& kRot, Quaternion& q)
	{
		float fTrace = kRot._11 + kRot._22 + kRot._33;
		float fRoot;

		if (fTrace > 0.0f)
		{
			// |w| > 1/2, may as well choose w > 1/2
			fRoot = sqrt(fTrace + 1.0f);   // 2w
			q.w = 0.5f * fRoot;
			fRoot = 0.5f / fRoot;  // 1/(4w)
			q.x = (kRot._32 - kRot._23) * fRoot;
			q.y = (kRot._13 - kRot._31) * fRoot;
			q.z = (kRot._21 - kRot._12) * fRoot;
		}
		else
		{
			// |w| <= 1/2
			int s_iNext[3] = { 1, 2, 0 };
			int i = 0;
			if (kRot._22 > kRot._11)
				i = 1;
			if (kRot._33 > kRot.Get(i, i))
				i = 2;
			int j = s_iNext[i];
			int k = s_iNext[j];

			fRoot = sqrt(kRot.Get(i, i) - kRot.Get(j, j) - kRot.Get(k, k) + 1.0f);
			float* apkQuat[3] = { &q.x, &q.y, &q.z };
			
			*apkQuat[i] = 0.5f * fRoot;
			fRoot = 0.5f / fRoot;
			q.w = (kRot.Get(k, j) - kRot.Get(j, k)) * fRoot;
			*apkQuat[j] = (kRot.Get(j, i) + kRot.Get(i, j)) * fRoot;
			*apkQuat[k] = (kRot.Get(k, i) + kRot.Get(i, k)) * fRoot;
		}
		q = Normalize(q);
	}

	bool Quaternion::LookRotationToQuaternion(const Vector3& viewVec, const Vector3& upVec, Quaternion* res)
	{
		float3x3 m;
		if (!float3x3::LookRotationToMatrix(viewVec, upVec, &m))
			return false;
		MatrixToQuaternion(m, *res);
		return true;
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