#pragma once
#include "../Core/Mathf/Mathf.h"
#include "GraphicsCore.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class RenderCore : public GraphicsCore
{
public:
	RenderCore(HINSTANCE hInstance);
	RenderCore(const RenderCore& rhs) = delete;
	RenderCore& operator=(const RenderCore& rhs) = delete;
	~RenderCore();

	virtual void Draw(const GameTimer& gt)override;

private:

	void CopyColorPass();

	//	DrawShadowMap
	void DrawShadowMap(ID3D12Resource* matBuffer);

	//	DrawDepthNormal
	void DrawDepthNormal();

	//	Draw SSAO
	void DrawSSAO(ID3D12Resource* matBuffer);

	void BindGBuffer();

	//DrawOpaque
	void DrawOpaque();

	//DrawSkyBox
	void DrawSkyBox();
};