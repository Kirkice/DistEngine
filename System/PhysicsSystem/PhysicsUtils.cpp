#include "PhysicsUtils.h"
#include "../../Core/Mathf/Mathf.h"

namespace Dist
{
	void PhysicsUtils::InitFrustumPlane(float ScreenDepth, CXMMATRIX ProjMatrix, CXMMATRIX ViewMatrix)
	{
		float zMinimum, r;
		float4x4 matrix;
		float4x4 mProjMatrix = float4x4(ProjMatrix);
		float4x4 mViewMatrix = float4x4(ViewMatrix);

		//计算视截体近裁剪面的距离
		zMinimum = -mProjMatrix._43 / mProjMatrix._33;
		r = ScreenDepth / (ScreenDepth - zMinimum);

		mProjMatrix._33 = r;
		mProjMatrix._43 = -r * zMinimum;

		//从相机矩阵和投影矩阵计算视截体矩阵
		matrix = mViewMatrix * mProjMatrix;

		//计算视截体的近裁剪面
		XMFLOAT4 nearPlane;
		nearPlane.x = matrix._14 + matrix._13;
		nearPlane.y = matrix._24 + matrix._23;
		nearPlane.z = matrix._34 + matrix._33;
		nearPlane.w = matrix._44 + matrix._43;
		mPlane[0] = XMLoadFloat4(&nearPlane);
		mPlane[0] = XMPlaneNormalize(mPlane[0]);

		//计算视截体的远裁剪面
		XMFLOAT4 FarPlane;
		FarPlane.x = matrix._14 - matrix._13;
		FarPlane.y = matrix._24 - matrix._23;
		FarPlane.z = matrix._34 - matrix._33;
		FarPlane.w = matrix._44 - matrix._43;
		mPlane[1] = XMLoadFloat4(&FarPlane);
		mPlane[1] = XMPlaneNormalize(mPlane[1]);

		//计算视截体的左裁剪面(XZ面)
		XMFLOAT4 LeftPlane;
		LeftPlane.x = matrix._14 + matrix._11;
		LeftPlane.y = matrix._24 + matrix._21;
		LeftPlane.z = matrix._34 + matrix._31;
		LeftPlane.w = matrix._44 + matrix._41;
		mPlane[2] = XMLoadFloat4(&LeftPlane);
		mPlane[2] = XMPlaneNormalize(mPlane[2]);

		//计算视截体的右裁剪面(XZ面)
		XMFLOAT4 RightPlane;
		RightPlane.x = matrix._14 - matrix._11;
		RightPlane.y = matrix._24 - matrix._21; 
		RightPlane.z = matrix._34 - matrix._31;
		RightPlane.w = matrix._44 - matrix._41;
		mPlane[3] = XMLoadFloat4(&RightPlane);
		mPlane[3] = XMPlaneNormalize(mPlane[3]);

		//计算视截体的顶裁剪面(YZ面)
		XMFLOAT4 TopPlane;
		TopPlane.x = matrix._14 - matrix._12;
		TopPlane.y = matrix._24 - matrix._22;
		TopPlane.z = matrix._34 - matrix._32;
		TopPlane.w = matrix._44 - matrix._42;
		mPlane[4] = XMLoadFloat4(&TopPlane);
		mPlane[4] = XMPlaneNormalize(mPlane[4]);

		//计算视截体的底裁剪面(YZ面)
		XMFLOAT4 BottomPlane;
		BottomPlane.x = matrix._14 + matrix._12;
		BottomPlane.y = matrix._24 + matrix._22;
		BottomPlane.z = matrix._34 + matrix._32;
		BottomPlane.w = matrix._44 + matrix._42;
		mPlane[5] = XMLoadFloat4(&BottomPlane);
		mPlane[5] = XMPlaneNormalize(mPlane[5]);
	}

	//	判断Sphere与视锥体碰撞
	bool PhysicsUtils::CheckFrustumSphereIntersect(Camera camera, BoundingBox bound, Transform tranform)
	{
		InitFrustumPlane(camera.GetFarZ(),camera.GetView(),camera.GetProj());

		XMVECTOR Point = XMVectorSet(tranform.position.x, tranform.position.y, tranform.position.z, 1.0f);
		XMFLOAT4 DotEnd;
		for (int i = 0; i < 6; ++i)
		{
			XMStoreFloat4(&DotEnd, XMPlaneDotCoord(mPlane[i], Point));
			if (-bound.bound_sphere.GetRadius() > DotEnd.x)
			{
				return false;
			} 
		}
		return true;
	}

	//	判断视锥体与AABB碰撞
	bool PhysicsUtils::CheckFrustumAABBIntersect(Camera camera, BoundingBox bound)
	{
		bool ret = true;
		Vector3 vmin, vmax;

		for (int i = 0; i < 6; ++i)
		{
			Vector4 plane = Vector4(mPlane[i]);
			// X axis 
			if (plane.x > 0) {
				vmin.x = bound.aabb.GetMin().x;
				vmax.x = bound.aabb.GetMax().x;
			}
			else {
				vmin.x = bound.aabb.GetMax().x;
				vmax.x = bound.aabb.GetMin().x;
			}
			// Y axis 
			if (plane.y > 0) {
				vmin.y = bound.aabb.GetMin().y;
				vmax.y = bound.aabb.GetMax().y;
			}
			else {
				vmin.y = bound.aabb.GetMax().y;
				vmax.y = bound.aabb.GetMin().y;
			}
			// Z axis 
			if (plane.z > 0) {
				vmin.z = bound.aabb.GetMin().z;
				vmax.z = bound.aabb.GetMax().z;
			}
			else {
				vmin.z = bound.aabb.GetMax().z;
				vmax.z = bound.aabb.GetMin().z;
			}
			if (Vector3::Dot(plane.To3D(), vmin) + plane.w > 0)
				return false;
			if (Vector3::Dot(plane.To3D(), vmax) + plane.w >= 0)
				ret = true;
		}
		return ret;
	}
}