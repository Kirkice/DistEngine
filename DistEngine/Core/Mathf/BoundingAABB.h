#pragma once
#include "Mathf.h"

namespace Mathf
{
	struct BoundingAABB
	{
		Vector3 m_min, m_max;
		BoundingAABB();
		BoundingAABB(Vector3 min, Vector3 max);

		void AddPoint(Vector3 point);
		void AddBoundingAABB(const BoundingAABB& aabb);
		Vector3 GetMin();
		Vector3 GetMax();
		Vector3 GetCenter();

		void Update(float4x4 mat);

		bool IsIntersect(const BoundingAABB& aabb);
		bool IsIntersectWithRay(Vector3 rayOri, Vector3 rayDir, float length = 100);
	};
}