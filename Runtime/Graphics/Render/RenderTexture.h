#pragma once

#include "DX12Utils.h"

namespace DistRender
{
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

	void BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE& CPUDescriptor,
		CD3DX12_GPU_DESCRIPTOR_HANDLE& GPUDescriptor,
		UINT mCbvSrvUavDescriptorSize
	);

	void OnResize(UINT newWidth, UINT newHeight);

private:
	void BuildDescriptors();
	void BuildResource();

private:

	ID3D12Device* md3dDevice = nullptr;

	UINT mWidth = 0;
	UINT mHeight = 0;
	DXGI_FORMAT mTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhGpuSrv;

	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissorRect;

	Microsoft::WRL::ComPtr<ID3D12Resource> mRenderTexture = nullptr;
	};
}