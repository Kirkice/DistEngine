#include "PhysicsCore.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

PhysicsCore::PhysicsCore(HINSTANCE hInstance) : D3DApp(hInstance)
{
}

PhysicsCore::~PhysicsCore()
{
	if (md3dDevice != nullptr)
		FlushCommandQueue();
}

void PhysicsCore::OnKeyboardInput(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	if (GetAsyncKeyState('W') & 0x8000)
		mCamera.Walk(10.0f * dt);

	if (GetAsyncKeyState('S') & 0x8000)
		mCamera.Walk(-10.0f * dt);

	if (GetAsyncKeyState('A') & 0x8000)
		mCamera.Strafe(-10.0f * dt);

	if (GetAsyncKeyState('D') & 0x8000)
		mCamera.Strafe(10.0f * dt);

	mCamera.UpdateViewMatrix();
}

void PhysicsCore::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;
	isMouseDown = true;
	SetCapture(mhMainWnd);
}


void PhysicsCore::OnMouseUp(WPARAM btnState, int x, int y)
{
	isMouseDown = false;
	ReleaseCapture();
}

void PhysicsCore::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

		mCamera.Pitch(dy);
		mCamera.RotateY(dx);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

bool PhysicsCore::ScreenPointToRay(BoundingAABB aabb)
{
	float cx = (mLastMousePos.x - (mClientWidth * 0.5)) / (mClientWidth * 0.5);
	float cy = ((mClientHeight * 0.5) - mLastMousePos.y) / (mClientHeight * 0.5);
	Vector3 DirPos = mCamera.ScreenToWorldPoint(Vector2(cx, cy));
	Vector3 RayDir = DirPos - Vector3(mCamera.GetPosition());
	return aabb.IsIntersectWithRay(Vector3(mCamera.GetPosition()), RayDir.Normalize(RayDir), 200);
}