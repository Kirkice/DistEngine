#pragma once
#include "../Core/Mathf/Mathf.h"
#include "MeshFliter.h"
#include <DirectXCollision.h>

struct BoundingAABB
{
	Vector3 m_min, m_max;
	BoundingAABB();
	BoundingAABB(Vector3 min, Vector3 max);
	BoundingAABB(MeshData data);

	void AddPoint(Vector3 point);
	void AddBoundingAABB(const BoundingAABB& aabb);
	Vector3 GetMin();
	Vector3 GetMax();
	Vector3 GetCenter();
	Vector3 GetExtents();
	BoundingBox ToBoundBox();

	void Update(float4x4 mat);

	bool IsIntersect(const BoundingAABB& aabb);
	bool IsIntersectWithRay(Vector3 rayOri, Vector3 rayDir, float length = 100);
};