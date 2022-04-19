#include "BoundingBox.h"

namespace Dist
{
	BoundingBox::BoundingBox()
	{
		aabb = BoundingAABB();
		bound_sphere = BoundingSphere();
		obb = BoundingOBB();
	}

	BoundingBox::~BoundingBox()
	{

	}
}