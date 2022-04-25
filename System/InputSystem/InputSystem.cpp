#include "InputSystem.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

namespace Dist
{
	InputSystem::InputSystem(HINSTANCE hInstance) : EngineApp(hInstance)
	{
	}

	InputSystem::~InputSystem()
	{
		if (md3dDevice != nullptr)
			FlushCommandQueue();
	}

	void InputSystem::OnKeyboardInput(const GameTimer& gt)
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

	void InputSystem::OnMouseDown(WPARAM btnState, int x, int y)
	{
		mLastMousePos.x = x;
		mLastMousePos.y = y;

		SetCapture(mhMainWnd);
	}


	void InputSystem::OnMouseUp(WPARAM btnState, int x, int y)
	{
		ReleaseCapture();
	}

	void InputSystem::OnMouseMove(WPARAM btnState, int x, int y)
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
}