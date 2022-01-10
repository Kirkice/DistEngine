#include "BoundingSphere.h"

namespace Mathf
{
	BoundingSphere::BoundingSphere()
	{
		x = y = z = w = 0.0f;
	}

	BoundingSphere::BoundingSphere(float x_, float y_, float z_, float w_)
	{
		x = x_;
		y = y_;
		z = z_;
		w = w_;
	}

	BoundingSphere::BoundingSphere(const Vector3& xyz, float radius_)
	{
		x = xyz.x;
		y = xyz.y;
		z = xyz.z;
		w = radius_;
	}

	BoundingSphere::BoundingSphere(const XMFLOAT4& xyzw)
	{
		x = xyzw.x;
		y = xyzw.y;
		z = xyzw.z;
		w = xyzw.w;
	}

	BoundingSphere::BoundingSphere(FXMVECTOR xyzw)
	{
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(this), xyzw);
	}

	BoundingSphere::BoundingSphere(Vector4 vec)
	{
		x = vec.x;
		y = vec.y;
		z = vec.z;
		w = vec.w;
	}

	BoundingSphere BoundingSphere::Union(const BoundingSphere& rhs)
	{
		float radA = GetRadius();
		if (radA == 0.0f)
			return rhs;

		float radB = rhs.GetRadius();
		if (radB == 0.0f)
			return *this;

		Vector3 diff = GetCenter() - rhs.GetCenter();
		float dist = Vector3::Length(diff);

		// Safe normalize vector between sphere centers
		diff = dist < 1e-6f ? Vector3(1,0,0) : diff * (1 / dist);

		Vector3 extremeA = GetCenter() + diff * Max(radA, radB - dist);
		Vector3 extremeB = rhs.GetCenter() - diff * Max(radB, radA - dist);

		return BoundingSphere((extremeA + extremeB) * 0.5f, Vector3::Length(extremeA - extremeB) * 0.5f);
	}
}