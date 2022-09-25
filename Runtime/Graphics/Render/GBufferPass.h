#ifndef GBUFFER_H
#define GBUFFER_H
#pragma once

#include "DX12Utils.h"
#include "../Graphics/FrameResource.h"

class GBuffer
{
public:

	GBuffer(
		ID3D12Device* device,
		UINT width,
		UINT height
	);

	GBuffer(const GBuffer& rhs) = delete;
	GBuffer& operator=(const GBuffer& rhs) = delete;
	~GBuffer() = default;

	ID3D12Resource* GetGBuffer(int index);

	CD3DX12_CPU_DESCRIPTOR_HANDLE GET_GBUFFER_CPU_SRV(int index)const;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GET_GBUFFER_GPU_SRV(int index)const;
	CD3DX12_CPU_DESCRIPTOR_HANDLE GET_GBUFFER_CPU_RTV(int index)const;

	void BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE& CPUDescriptor,
		CD3DX12_GPU_DESCRIPTOR_HANDLE& GPUDescriptor,
		CD3DX12_CPU_DESCRIPTOR_HANDLE& CPURtv,
		UINT mCbvSrvUavDescriptorSize,
		UINT rtvDescriptorSize
	);

	void RebuildDescriptors();

	void SetPSOs(ID3D12PipelineState* gbufferPSO);

	void OnResize(UINT newWidth, UINT newHeight);

private:
	void BuildResources();

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> mGBufferRootSig;
	Microsoft::WRL::ComPtr<ID3D12Resource> GBufferResources[4];

	ID3D12Device* md3dDevice = nullptr;
	ID3D12PipelineState* mGBufferPSO;

	CD3DX12_CPU_DESCRIPTOR_HANDLE mhGBufferCpuSrv[4];
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhGBufferGpuSrv[4];
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhGBufferCpuRtv[4];

	DXGI_FORMAT mFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissorRect;

	UINT mWidth = 0;
	UINT mHeight = 0;
};

#endif