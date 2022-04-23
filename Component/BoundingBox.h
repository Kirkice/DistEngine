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