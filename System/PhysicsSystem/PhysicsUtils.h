#pragma once
#include "../../Core/Mathf/Mathf.h"
#include "../../Component/MeshRender.h"
#include "../../Component/DirectionLight.h"
#include "../../Component/Camera.h"

namespace Dist
{
	class PhysicsUtils
	{
	private:
		static XMVECTOR mPlane[6];

	public:
		//	ÊÓ×¶		Çò		¼ì²â
		static bool CheckFrustumSphereIntersect(Camera camera, BoundingBox bound, Transform transform);

		//	ÊÓ×¶		AABB	¼ì²â
		static bool CheckFrustumAABBIntersect(Camera camera, BoundingBox bound);

		//	³õÊ¼»¯ FrustumPlane
		static void InitFrustumPlane(float ScreenDepth, CXMMATRIX ProjMatrix, CXMMATRIX ViewMatrix);
	};
}