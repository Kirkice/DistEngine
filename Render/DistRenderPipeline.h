#pragma once
#include "../Core/Mathf/Mathf.h"
#include "GraphicsCore.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;
 

//Render Layer
enum class MatBufferType : int
{
	PBR = 0,
	SkyBox = 1,
	PostProcess = 2,
};

class DistRenderPipeline: public GraphicsCore
{
public:
	DistRenderPipeline(HINSTANCE hInstance);
	DistRenderPipeline(const DistRenderPipeline& rhs) = delete;
	DistRenderPipeline& operator=(const DistRenderPipeline& rhs) = delete;
	~DistRenderPipeline();

public:
	void Render();

private:

	//	RenderPass
	void RenderShadowPass(ID3D12Resource* matBuffer);

	void DepthPrePass();

	void RenderGBuffer();

	void RenderOpaquePass();

	void DeferredRenderPass();

	void RenderSkyBoxPass();

	void RenderTransparentPass();

	void RenderAxisPass();

	void RenderGizmosPass();

	void RenderPostProcessPass();

	void DrawVolumeFog();


	//	RenderUtils

	ID3D12Resource* mMatBuffer = nullptr;

	void SetDescriptorHeap();

	void SetRootSignature();

	void SetMatBuffer(MatBufferType type);

	void CopyColorPass();
};