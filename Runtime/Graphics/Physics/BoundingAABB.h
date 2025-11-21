#pragma once
#include "../Core/Mathf/Mathf.h"
#include "../Component/MeshFliter.h"
#include <DirectXCollision.h>

namespace DistBound
{
    struct BoundingAABB
    {
        Mathf::Vector3 m_min, m_max;
        BoundingAABB();
        BoundingAABB(Mathf::Vector3 min, Mathf::Vector3 max);
        BoundingAABB(MeshData data);

        void AddPoint(Mathf::Vector3 point);
        void AddBoundingAABB(const BoundingAABB& aabb);
        Mathf::Vector3 GetMin();
        Mathf::Vector3 GetMax();
        Mathf::Vector3 GetCenter();
        Mathf::Vector3 GetExtents();
        DirectX::BoundingBox ToBoundBox();

        void Update(Mathf::float4x4 mat);

        bool IsIntersect(const BoundingAABB& aabb);
        bool IsIntersectWithRay(Mathf::Vector3 rayOri, Mathf::Vector3 rayDir, float length = 100);
    };
}
