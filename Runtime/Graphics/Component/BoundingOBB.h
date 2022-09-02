#pragma once
#include "../Core/Mathf/Mathf.h"
#include "BoundingAABB.h"


struct BoundingOBB
{
	Vector3 m_min, m_max;
	BoundingOBB();
	BoundingOBB(Vector3 min, Vector3 max);

	void AddPoint(Vector3 point);
	void AddBoundingOBB(const BoundingOBB& obb);

	Vector3 GetMin();
	Vector3 GetMax();
	Vector3 GetCenter();

	bool IsIntersect(const BoundingOBB& obb);
	bool IsIntersectWithRay(Vector3 rayOri, Vector3 rayDir, float length = 100);
};