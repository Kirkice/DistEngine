#pragma once
#include "../Core/Mathf/Mathf.h"
#include "GraphicsCore.h"

#include "../ThirdParty/ImGUI/imgui.h"
#include "../ThirdParty/ImGUI/imgui_impl_win32.h"
#include "../ThirdParty/ImGUI/imgui_impl_dx12.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class ForwardRenderer : public GraphicsCore
{
public:
	ForwardRenderer(HINSTANCE hInstance);
	ForwardRenderer(const ForwardRenderer& rhs) = delete;
	ForwardRenderer& operator=(const ForwardRenderer& rhs) = delete;
	~ForwardRenderer();

private:

	virtual void Draw(const GameTimer& gt)override;


	//	DrawShadowMap
	void DrawShadowMap(ID3D12Resource* matBuffer);

	//	DrawDepthNormal
	void DrawDepthNormal();

	//	Draw SSAO
	void DrawSSAO(ID3D12Resource* matBuffer);

	//DrawOpaque
	void DrawOpaque();

	//DrawSkyBox
	void DrawSkyBox();
};