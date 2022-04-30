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
		//	��׶		��		���
		static bool CheckFrustumSphereIntersect(Camera camera, BoundingBox bound, Transform transform);

		//	��׶		AABB	���
		static bool CheckFrustumAABBIntersect(Camera camera, BoundingBox bound);

		//	��ʼ�� FrustumPlane
		static void InitFrustumPlane(float ScreenDepth, CXMMATRIX ProjMatrix, CXMMATRIX ViewMatrix);
	};
}