#pragma once
#include "../../Core/Mathf/Mathf.h"
#include "../../Core/Mathf/BoundingOBB.h"
#include "../../Core/Mathf/BoundingAABB.h"
#include "../../Core/Mathf/BoundingSphere.h"

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