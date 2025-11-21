#include "BoundingSphere.h"

using namespace Mathf;

namespace DistBound
{
	BoundingSphere::BoundingSphere() : x(0), y(0), z(0), w(1.f) {}

	BoundingSphere::BoundingSphere(float x, float y, float z, float r) : x(x), y(y), z(z), w(r) {}

	BoundingSphere::BoundingSphere(const Vector3& xyz, float radius) : x(xyz.x), y(xyz.y), z(xyz.z), w(radius) {}

	BoundingSphere::BoundingSphere(const DirectX::XMFLOAT4& xyzw) : x(xyzw.x), y(xyzw.y), z(xyzw.z), w(xyzw.w) {}

	BoundingSphere::BoundingSphere(DirectX::FXMVECTOR xyzw)
	{
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(this), xyzw);
	}

	BoundingSphere::BoundingSphere(Vector4 sphere) : x(sphere.x), y(sphere.y), z(sphere.z), w(sphere.w) {}

	bool BoundingSphere::IsIntersect(const BoundingSphere& rhs)
	{
		float distSq = (x - rhs.x) * (x - rhs.x) + (y - rhs.y) * (y - rhs.y) + (z - rhs.z) * (z - rhs.z);
		float radSum = w + rhs.w;
		return distSq <= (radSum * radSum);
	}

	bool BoundingSphere::IsContain(const BoundingSphere& rhs)
	{
		float distSq = (x - rhs.x) * (x - rhs.x) + (y - rhs.y) * (y - rhs.y) + (z - rhs.z) * (z - rhs.z);
		float radDiff = w - rhs.w;
		return (radDiff >= 0) && (distSq <= (radDiff * radDiff));
	}

	bool BoundingSphere::IsIntersectWithRay(Vector3 rayOri, Vector3 rayDir, float length)
	{
		Vector3 m = rayOri - GetCenter();
		float b = Vector3::Dot(m, rayDir);
		float c = Vector3::Dot(m, m) - w * w;

		if (c > 0.0f && b > 0.0f) return false;

		float discr = b * b - c;
		if (discr < 0.0f) return false;

		float t = -b - sqrt(discr);
		if (t < 0.0f) t = 0.0f;

		if (t > length) return false;

		return true;
	}
}
