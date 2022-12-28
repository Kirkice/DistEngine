#pragma once
#pragma once
#include "DX12GameApp.h"
#include "../Component/Camera.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class InputSystem : public DX12GameApp
{

public:
	InputSystem(HINSTANCE hInstance);
	InputSystem(const InputSystem& rhs) = delete;
	InputSystem& operator=(const InputSystem& rhs) = delete;
	~InputSystem();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

	void OnKeyboardInput(const GameTimer& gt);

public:

	POINT mLastMousePos;
	Camera mCamera;
};
