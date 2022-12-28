#pragma once
#include "../Core/Mathf/Mathf.h"
using namespace Mathf;

namespace DistBound
{
	struct BoundingSphere
	{
		float x, y, z, w;
		BoundingSphere();
		BoundingSphere(float x, float y, float z, float r);
		explicit BoundingSphere(const Vector3& xyz, float radius = 1.0f);
		explicit BoundingSphere(const DirectX::XMFLOAT4& xyzw);
		explicit BoundingSphere(DirectX::FXMVECTOR xyzw);
		explicit BoundingSphere(Vector4 sphere);

		Vector3 GetCenter(void) const { return Vector3(x, y, z); }
		float GetRadius(void) const { return w; }

		bool IsIntersect(const BoundingSphere& rhs);
		bool IsContain(const BoundingSphere& rhs);

		bool IsIntersectWithRay(Vector3 rayOri, Vector3 rayDir, float length = 100);
	};
}
