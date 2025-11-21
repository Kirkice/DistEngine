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
        // AABB
        BoundingAABB aabb;
        // Sphere
        BoundingSphere bound_sphere;
        // OBB
        BoundingOBB obb;

    public:
        BoundingBox(std::string _name);
        ~BoundingBox();
    };
}
