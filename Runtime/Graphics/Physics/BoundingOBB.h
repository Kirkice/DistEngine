#pragma once
#include "../Core/Mathf/Mathf.h"
#include "BoundingAABB.h"

namespace DistBound
{
    struct BoundingOBB
    {
        Mathf::Vector3 m_min, m_max;
        BoundingOBB();
        BoundingOBB(Mathf::Vector3 min, Mathf::Vector3 max);

        void AddPoint(Mathf::Vector3 point);
        void AddBoundingOBB(const BoundingOBB& obb);

        Mathf::Vector3 GetMin();
        Mathf::Vector3 GetMax();
        Mathf::Vector3 GetCenter();

        bool IsIntersect(const BoundingOBB& obb);
        bool IsIntersectWithRay(Mathf::Vector3 rayOri, Mathf::Vector3 rayDir, float length = 100);
    };
}
