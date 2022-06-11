#include "Camera.h"
#include <string>


Camera::Camera()
{
	SetLens(0.25f * Mathf::Pi, 1.0f, 1.0f, 1000.0f);
}

Camera::~Camera()
{
}

//	更新相机
void Camera::tickCamera()
{
	position = { position3f.x, position3f.y, position3f.z };
}

//	获取相机坐标
XMVECTOR Camera::GetPosition()const
{
	return position.ToSIMD();
}

XMFLOAT3 Camera::GetPosition3f()const
{
	return XMFLOAT3(position3f.x, position3f.y, position3f.z);
}

//	设置相机坐标
void Camera::SetPosition(float x, float y, float z)
{
	position = Vector3(x, y, z);
	mViewDirty = true;
}

void Camera::SetPosition(const Vector3& v)
{
	position = v;
	mViewDirty = true;
}

//	获取右向量
XMVECTOR Camera::GetRight()const
{
	return XMLoadFloat3(&mRight);
}

XMFLOAT3 Camera::GetRight3f()const
{
	return mRight;
}

//	获取上向量
XMVECTOR Camera::GetUp()const
{
	return XMLoadFloat3(&mUp);
}

XMFLOAT3 Camera::GetUp3f()const
{
	return mUp;
}

//	获取前向量
XMVECTOR Camera::GetLook()const
{
	return XMLoadFloat3(&mLook);
}

	XMFLOAT3 Camera::GetLook3f()const
	{
		return mLook;
	}

	//	获取NearZ
	float Camera::GetNearZ()const
	{
		return mNearZ;
	}

	//	获取FarZ
	float Camera::GetFarZ()const
	{
		return mFarZ;
	}

	//	获取宽高比
	float Camera::GetAspect()const
	{
		return mAspect;
	}

	//	获取FovY
	float Camera::GetFovY()const
	{
		return mFovY;
	}

	float& Camera::GetFovYPtr()
	{
		return mFovY;
	}

	//	获取FovX
	float Camera::GetFovX()const
	{
		float halfWidth = 0.5f * GetNearWindowWidth();
		return 2.0f * atan(halfWidth / mNearZ);
	}

	float Camera::GetNearWindowWidth()const
	{
		return mAspect * mNearWindowHeight;
	}

	float Camera::GetNearWindowHeight()const
	{
		return mNearWindowHeight;
	}

	float Camera::GetFarWindowWidth()const
	{
		return mAspect * mFarWindowHeight;
	}

	float Camera::GetFarWindowHeight()const
	{
		return mFarWindowHeight;
	}

	void Camera::SetLens(float fovY, float aspect, float zn, float zf)
	{
		// cache properties
		mFovY = fovY;
		mAspect = aspect;
		mNearZ = zn;
		mFarZ = zf;

		mNearWindowHeight = 2.0f * mNearZ * tanf(0.5f * mFovY);
		mFarWindowHeight = 2.0f * mFarZ * tanf(0.5f * mFovY);

		XMMATRIX P = XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
		XMStoreFloat4x4(&mProj, P);
	}

	void Camera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
	{
		XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
		XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
		XMVECTOR U = XMVector3Cross(L, R);

		XMStoreFloat3(&position3f, pos);
		XMStoreFloat3(&mLook, L);
		XMStoreFloat3(&mRight, R);
		XMStoreFloat3(&mUp, U);

		mViewDirty = true;
	}

	void Camera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
	{
		XMVECTOR P = XMLoadFloat3(&pos);
		XMVECTOR T = XMLoadFloat3(&target);
		XMVECTOR U = XMLoadFloat3(&up);

		LookAt(P, T, U);

		mViewDirty = true;
	}

	XMMATRIX Camera::GetView()const
	{
		assert(!mViewDirty);
		return XMLoadFloat4x4(&mView);
	}

	XMMATRIX Camera::GetProj()const
	{
		return XMLoadFloat4x4(&mProj);
	}


	XMFLOAT4X4 Camera::GetView4x4f()const
	{
		assert(!mViewDirty);
		return mView;
	}

	XMFLOAT4X4 Camera::GetProj4x4f()const
	{
		return mProj;
	}

	void Camera::Strafe(float d)
	{
		// mPosition += d*mRight
		XMVECTOR s = XMVectorReplicate(d);
		XMVECTOR r = XMLoadFloat3(&mRight);
		XMVECTOR p = XMLoadFloat3(&position3f);
		XMStoreFloat3(&position3f, XMVectorMultiplyAdd(s, r, p));

		mViewDirty = true;
	}

	void Camera::Walk(float d)
	{
		// mPosition += d*mLook
		XMVECTOR s = XMVectorReplicate(d);
		XMVECTOR l = XMLoadFloat3(&mLook);
		XMVECTOR p = XMLoadFloat3(&position3f);
		XMStoreFloat3(&position3f, XMVectorMultiplyAdd(s, l, p));

		mViewDirty = true;
	}

	void Camera::Pitch(float angle)
	{
		// Rotate up and look vector about the right vector.

		XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&mRight), angle);

		XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
		XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));

		mViewDirty = true;
	}

	void Camera::RotateY(float angle)
	{
		// Rotate the basis vectors about the world y-axis.

		XMMATRIX R = XMMatrixRotationY(angle);

		XMStoreFloat3(&mRight, XMVector3TransformNormal(XMLoadFloat3(&mRight), R));
		XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
		XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));

		mViewDirty = true;
	}

	void Camera::UpdateViewMatrix()
	{
		if (mViewDirty)
		{
			XMVECTOR R = XMLoadFloat3(&mRight);
			XMVECTOR U = XMLoadFloat3(&mUp);
			XMVECTOR L = XMLoadFloat3(&mLook);
			XMVECTOR P = XMLoadFloat3(&position3f);

			// Keep camera's axes orthogonal to each other and of unit length.
			L = XMVector3Normalize(L);
			U = XMVector3Normalize(XMVector3Cross(L, R));

			// U, L already ortho-normal, so no need to normalize cross product.
			R = XMVector3Cross(U, L);

			// Fill in the view matrix entries.
			float x = -XMVectorGetX(XMVector3Dot(P, R));
			float y = -XMVectorGetX(XMVector3Dot(P, U));
			float z = -XMVectorGetX(XMVector3Dot(P, L));

			XMStoreFloat3(&mRight, R);
			XMStoreFloat3(&mUp, U);
			XMStoreFloat3(&mLook, L);

			mView(0, 0) = mRight.x;
			mView(1, 0) = mRight.y;
			mView(2, 0) = mRight.z;
			mView(3, 0) = x;

			mView(0, 1) = mUp.x;
			mView(1, 1) = mUp.y;
			mView(2, 1) = mUp.z;
			mView(3, 1) = y;

			mView(0, 2) = mLook.x;
			mView(1, 2) = mLook.y;
			mView(2, 2) = mLook.z;
			mView(3, 2) = z;

			mView(0, 3) = 0.0f;
			mView(1, 3) = 0.0f;
			mView(2, 3) = 0.0f;
			mView(3, 3) = 1.0f;

			mViewDirty = false;
		}
	}