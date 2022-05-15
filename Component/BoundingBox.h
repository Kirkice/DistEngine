#pragma once
#include "../Core/Mathf/Mathf.h"
#include "BoundingOBB.h"
#include "BoundingAABB.h"
#include "BoundingSphere.h"

class BoundingBox
{
public:
	//	AABB°üÎ§ºÐ
	BoundingAABB aabb;
	//	Çò°üÎ§ºÐ
	DistBound::BoundingSphere bound_sphere;
	//	OBB°üÎ§ºÐ
	BoundingOBB obb;

public:
	BoundingBox();
	~BoundingBox();
};