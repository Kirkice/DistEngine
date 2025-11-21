#pragma once
#include "DX12GameApp.h"
#include "../Component/Camera.h"

class InputSystem : public DX12GameApp
{
public:
    InputSystem(HINSTANCE hInstance);
    InputSystem(const InputSystem& rhs) = delete;
    InputSystem& operator=(const InputSystem& rhs) = delete;
    ~InputSystem();

    void OnMouseDown(WPARAM btnState, int x, int y) override;
    void OnMouseUp(WPARAM btnState, int x, int y) override;
    void OnMouseMove(WPARAM btnState, int x, int y) override;

    void OnKeyboardInput(const GameTimer& gt);

public:
    POINT mLastMousePos;
    Camera mCamera;
};
