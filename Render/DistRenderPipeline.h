#pragma once
#include "../Core/Mathf/Mathf.h"
#include "GraphicsCore.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;
 
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

	ID3D12Resource* GetMatBuffer();

	void RenderShadowPass(ID3D12Resource* matBuffer);

	void DepthPrePass();

	void RenderGBuffer();

	void RenderOpaquePass();

	void DeferredRenderPass();

	void RenderSkyBoxPass();

	void RenderTransparentPass();

	void RenderAxisPass();

	void RenderGizmosPass();

	void RenderPostProcessPass(ID3D12Resource* matBuffer);

	void DrawVolumeFog(ID3D12Resource* matBuffer);




	void CopyColorPass();
};