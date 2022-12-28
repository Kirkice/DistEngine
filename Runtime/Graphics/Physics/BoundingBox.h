#pragma once
#include "../Component/Component.h"
#include "../Core/Mathf/Mathf.h"
#include "BoundingOBB.h"
#include "BoundingAABB.h"
#include "BoundingSphere.h"

namespace DistBound
{
	class BoundingBox : public Component
	{
	public:
		//	AABB��Χ��
		BoundingAABB aabb;
		//	���Χ��
		DistBound::BoundingSphere bound_sphere;
		//	OBB��Χ��
		BoundingOBB obb;

	public:
		BoundingBox(std::string _name);
		~BoundingBox();
	};
}