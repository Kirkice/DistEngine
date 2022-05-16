#include "BoundingBox.h"

namespace DistBound
{
	BoundingBox::BoundingBox()
	{
		aabb = BoundingAABB();
		bound_sphere = DistBound::BoundingSphere();
		obb = BoundingOBB();
	}

	BoundingBox::~BoundingBox()
	{

	}
}