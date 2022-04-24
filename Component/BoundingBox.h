#pragma once
#include "../../Core/Mathf/Mathf.h"
#include "BoundingOBB.h"
#include "BoundingAABB.h"
#include "BoundingSphere.h"

namespace Dist
{
	class BoundingBox
	{
	public:
		//	AABB��Χ��
		BoundingAABB aabb;
		//	���Χ��
		BoundingSphere bound_sphere;
		//	OBB��Χ��
		BoundingOBB obb;

	public:
		BoundingBox();
		~BoundingBox();
	};
}