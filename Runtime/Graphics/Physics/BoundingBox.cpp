#include "BoundingBox.h"

namespace DistBound
{
	BoundingBox::BoundingBox(std::string _name = "BoundingBox") : Component(_name)
	{
		aabb = BoundingAABB();
		bound_sphere = DistBound::BoundingSphere();
		obb = BoundingOBB();
	}

	BoundingBox::~BoundingBox()
	{

	}
}