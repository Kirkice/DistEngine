#ifndef CAMERA_H
#define CAMERA_H

#include "d3dUtil.h"
#include "Mathf.h"

using namespace Mathf;

class Camera
{
public:

	Camera();
	~Camera();

	// Get/Set world camera position.
	XMVECTOR GetPosition()const;
	Vector3 GetPosition3f()const;
	void SetPosition(float x, float y, float z);
	void SetPosition(const DirectX::XMFLOAT3& v);
	void UpdateCamPosArray();
	void UpdateCamRotArray();
	// Get camera basis vectors.
	XMVECTOR GetRight()const;
	XMFLOAT3 GetRight3f()const;
	XMVECTOR GetUp()const;
	XMFLOAT3 GetUp3f()const;
	XMVECTOR GetLook()const;
	XMFLOAT3 GetLook3f()const;
	Vector3 ScreenToWorldPoint(Vector2 viewPortSize);

	// Get frustum properties.
	float GetNearZ()const;
	float GetFarZ()const;
	float GetAspect()const;
	float GetFovY()const;
	float& GetFovYPtr();
	float GetFovX()const;

	// Get near and far plane dimensions in view space coordinates.
	float GetNearWindowWidth()const;
	float GetNearWindowHeight()const;
	float GetFarWindowWidth()const;
	float GetFarWindowHeight()const;
	
	// Set frustum.
	void SetLens(float fovY, float aspect, float zn, float zf);

	// Define camera space via LookAt parameters.
	void LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);
	void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);

	// Get View/Proj matrices.
	XMMATRIX GetView()const;
	XMMATRIX GetProj()const;

	XMFLOAT4X4 GetView4x4f()const;
	XMFLOAT4X4 GetProj4x4f()const;

	// Strafe/Walk the camera a distance d.
	void Strafe(float d);
	void Walk(float d);

	// Rotate the camera.
	void Pitch(float angle);
	void RotateY(float angle);

	// After modifying camera position/orientation, call to rebuild the view matrix.
	void UpdateViewMatrix();

	float mPosArray[3] = { 0.0f, 0.0f, 0.0f };
	float mRotArray[3] = { 0.0f, 0.0f, 0.0f };
	float mScaleArray[3] = { 1.0f, 1.0f, 1.0f };

private:

	// Camera coordinate system with coordinates relative to world space.
	XMFLOAT3 mPosition = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 mRight = { 1.0f, 0.0f, 0.0f };
	XMFLOAT3 mUp = { 0.0f, 1.0f, 0.0f };
	XMFLOAT3 mLook = { 0.0f, 0.0f, 1.0f };

	// Cache frustum properties.
	float mNearZ = 0.0f;
	float mFarZ = 0.0f;
	float mAspect = 0.0f;
	float mFovY = 0.0f;
	float mNearWindowHeight = 0.0f;
	float mFarWindowHeight = 0.0f;

	bool mViewDirty = true;

	// Cache View/Proj matrices.
	XMFLOAT4X4 mView = Mathf::Identity4x4();
	XMFLOAT4X4 mProj = Mathf::Identity4x4();
};

#endif // CAMERA_H