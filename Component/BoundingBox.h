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
		//	AABB°üÎ§ºÐ
		BoundingAABB aabb;
		//	Çò°üÎ§ºÐ
		BoundingSphere bound_sphere;
		//	OBB°üÎ§ºÐ
		BoundingOBB obb;

	public:
		BoundingBox();
		~BoundingBox();
	};
}