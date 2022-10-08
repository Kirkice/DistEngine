#pragma once

#include "DX12Utils.h"

class RenderTexture
{
public:
	RenderTexture(ID3D12Device* device, UINT width, UINT height);

	RenderTexture(const RenderTexture& rhs) = delete;
	RenderTexture& operator=(const RenderTexture& rhs) = delete;
	~RenderTexture() = default;

	UINT Width()const;
	UINT Height()const;

	ID3D12Resource* Resource();
	CD3DX12_GPU_DESCRIPTOR_HANDLE GpuSrv()const;
	CD3DX12_CPU_DESCRIPTOR_HANDLE CpuSrv()const;

	D3D12_VIEWPORT Viewport()const;
	D3D12_RECT ScissorRect()const;

	CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhGpuSrv;

	void BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE& CPUDescriptor,
		CD3DX12_GPU_DESCRIPTOR_HANDLE& GPUDescriptor,
		UINT mCbvSrvUavDescriptorSize
	);

private:
	void BuildResource();

private:
	ID3D12Device* md3dDevice = nullptr;

	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissorRect;

	UINT mWidth = 0;
	UINT mHeight = 0;

	DXGI_FORMAT mTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	Microsoft::WRL::ComPtr<ID3D12Resource> mRenderTexture;
};