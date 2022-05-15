#pragma once
#include "../Core/Mathf/Mathf.h"
#include "BoundingOBB.h"
#include "BoundingAABB.h"
#include "BoundingSphere.h"

class BoundingBox
{
public:
	//	AABB��Χ��
	BoundingAABB aabb;
	//	���Χ��
	DistBound::BoundingSphere bound_sphere;
	//	OBB��Χ��
	BoundingOBB obb;

public:
	BoundingBox();
	~BoundingBox();
};