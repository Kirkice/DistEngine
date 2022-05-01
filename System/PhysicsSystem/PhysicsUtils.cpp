#include "PhysicsUtils.h"
#include "../../Core/Mathf/Mathf.h"

namespace Dist
{
	//void PhysicsUtils::InitFrustumPlane(float ScreenDepth, CXMMATRIX ProjMatrix, CXMMATRIX ViewMatrix)
	//{
	//	float zMinimum, r;
	//	float4x4 matrix;
	//	float4x4 mProjMatrix = float4x4(ProjMatrix);
	//	float4x4 mViewMatrix = float4x4(ViewMatrix);

	//	//�����ӽ�����ü���ľ���
	//	zMinimum = -mProjMatrix._43 / mProjMatrix._33;
	//	r = ScreenDepth / (ScreenDepth - zMinimum);

	//	mProjMatrix._33 = r;
	//	mProjMatrix._43 = -r * zMinimum;

	//	//����������ͶӰ��������ӽ������
	//	matrix = mViewMatrix * mProjMatrix;

	//	//�����ӽ���Ľ��ü���
	//	XMFLOAT4 nearPlane;
	//	nearPlane.x = matrix._14 + matrix._13;
	//	nearPlane.y = matrix._24 + matrix._23;
	//	nearPlane.z = matrix._34 + matrix._33;
	//	nearPlane.w = matrix._44 + matrix._43;
	//	mPlane[0] = XMLoadFloat4(&nearPlane);
	//	mPlane[0] = XMPlaneNormalize(mPlane[0]);

	//	//�����ӽ����Զ�ü���
	//	XMFLOAT4 FarPlane;
	//	FarPlane.x = matrix._14 - matrix._13;
	//	FarPlane.y = matrix._24 - matrix._23;
	//	FarPlane.z = matrix._34 - matrix._33;
	//	FarPlane.w = matrix._44 - matrix._43;
	//	mPlane[1] = XMLoadFloat4(&FarPlane);
	//	mPlane[1] = XMPlaneNormalize(mPlane[1]);

	//	//�����ӽ������ü���(XZ��)
	//	XMFLOAT4 LeftPlane;
	//	LeftPlane.x = matrix._14 + matrix._11;
	//	LeftPlane.y = matrix._24 + matrix._21;
	//	LeftPlane.z = matrix._34 + matrix._31;
	//	LeftPlane.w = matrix._44 + matrix._41;
	//	mPlane[2] = XMLoadFloat4(&LeftPlane);
	//	mPlane[2] = XMPlaneNormalize(mPlane[2]);

	//	//�����ӽ�����Ҳü���(XZ��)
	//	XMFLOAT4 RightPlane;
	//	RightPlane.x = matrix._14 - matrix._11;
	//	RightPlane.y = matrix._24 - matrix._21; 
	//	RightPlane.z = matrix._34 - matrix._31;
	//	RightPlane.w = matrix._44 - matrix._41;
	//	mPlane[3] = XMLoadFloat4(&RightPlane);
	//	mPlane[3] = XMPlaneNormalize(mPlane[3]);

	//	//�����ӽ���Ķ��ü���(YZ��)
	//	XMFLOAT4 TopPlane;
	//	TopPlane.x = matrix._14 - matrix._12;
	//	TopPlane.y = matrix._24 - matrix._22;
	//	TopPlane.z = matrix._34 - matrix._32;
	//	TopPlane.w = matrix._44 - matrix._42;
	//	mPlane[4] = XMLoadFloat4(&TopPlane);
	//	mPlane[4] = XMPlaneNormalize(mPlane[4]);

	//	//�����ӽ���ĵײü���(YZ��)
	//	XMFLOAT4 BottomPlane;
	//	BottomPlane.x = matrix._14 + matrix._12;
	//	BottomPlane.y = matrix._24 + matrix._22;
	//	BottomPlane.z = matrix._34 + matrix._32;
	//	BottomPlane.w = matrix._44 + matrix._42;
	//	mPlane[5] = XMLoadFloat4(&BottomPlane);
	//	mPlane[5] = XMPlaneNormalize(mPlane[5]);
	//}

	////	�ж�Sphere����׶����ײ
	//bool PhysicsUtils::CheckFrustumSphereIntersect(Camera camera, BoundingBox bound, Transform tranform)
	//{
	//	InitFrustumPlane(camera.GetFarZ(),camera.GetView(),camera.GetProj());

	//	XMVECTOR Point = XMVectorSet(tranform.position.x, tranform.position.y, tranform.position.z, 1.0f);
	//	XMFLOAT4 DotEnd;
	//	for (int i = 0; i < 6; ++i)
	//	{
	//		XMStoreFloat4(&DotEnd, XMPlaneDotCoord(mPlane[i], Point));
	//		if (-bound.bound_sphere.GetRadius() > DotEnd.x)
	//		{
	//			return false;
	//		} 
	//	}
	//	return true;
	//}

	//	�ж���׶����AABB��ײ
	bool PhysicsUtils::CheckFrustumAABBIntersect(Camera camera, BoundingBox bound)
	{
		float4x4 mFrustum = float4x4();
		mFrustum._11 = GetCOT(camera.GetFovY() / 2 * 0.01745329f) / camera.GetAspect();
		mFrustum._12 = 0;
		mFrustum._13 = 0;
		mFrustum._14 = 0;

		mFrustum._21 = 0;
		mFrustum._22 = GetCOT(camera.GetFovY() / 2 * 0.01745329f) / camera.GetAspect();
		mFrustum._23 = 0;
		mFrustum._24 = 0;

		mFrustum._31 = 0;
		mFrustum._32 = 0;
		mFrustum._33 = ((camera.GetFarZ() + camera.GetNearZ()) / (camera.GetFarZ() - camera.GetNearZ()) * -1);
		mFrustum._34 = -1;

		mFrustum._41 = 0;
		mFrustum._42 = 0;
		mFrustum._43 = ((2 * camera.GetNearZ() * camera.GetFarZ()) / (camera.GetFarZ() - camera.GetNearZ())) * -1;
		mFrustum._44 = 0;

		//	Min
		Vector4 cameraSpacePos = float4x4(camera.GetView()) * Vector4(bound.aabb.GetMin(),1);

		Vector4 res = mFrustum * Vector4(cameraSpacePos.x, cameraSpacePos.y, cameraSpacePos.z, 1);

		if ((res.x <= res.w) && (res.x >= res.w * -1) &&
			(res.y <= res.w) && (res.y >= res.w * -1) &&
			(res.z <= res.w) && (res.z >= res.w * -1))
		{
			return true;
		}

		cameraSpacePos = float4x4(camera.GetView()) * Vector4(bound.aabb.GetMax(), 1);

		//	Max
		res = mFrustum * Vector4(cameraSpacePos.x, cameraSpacePos.y, cameraSpacePos.z, 1);

		if ((res.x <= res.w) && (res.x >= res.w * -1) &&
			(res.y <= res.w) && (res.y >= res.w * -1) &&
			(res.z <= res.w) && (res.z >= res.w * -1))
		{
			return true;
		}

		return false;
	}
}