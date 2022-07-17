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

	//	DrawOpaque
	void DrawOpaque();

	//	DrawUnlit
	void DrawUnlit();

	//	Draw GBuffer
	void DrawGBuffer0();
	void DrawGBuffer1();
	void DrawGBuffer2();
	void DrawGBuffer3();

	//	DrawSkyBox
	void DrawSkyBox();


	//	DrawGizmos
	void DrawGizmos();

	//	DrawPostProcess
	void DrawPostProcess(ID3D12Resource* matBuffer);

	void DrawVolumeFog(ID3D12Resource* matBuffer);
};