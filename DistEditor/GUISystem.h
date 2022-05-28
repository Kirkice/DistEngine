#pragma once
#include "../Core/Mathf/Mathf.h"
#include "RenderCore.h"

#include "../ThirdParty/ImGUI/imgui.h"
#include "../ThirdParty/ImGUI/imgui_impl_win32.h"
#include "../ThirdParty/ImGUI/imgui_impl_dx12.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class GUISystem : public RenderCore
{
public:
	GUISystem(HINSTANCE hInstance);
	GUISystem(const GUISystem& rhs) = delete;
	GUISystem& operator=(const GUISystem& rhs) = delete;
	~GUISystem();

private:

	virtual void Draw(const GameTimer& gt)override;
};
