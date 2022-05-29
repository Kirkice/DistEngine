#pragma once

#include "d3dUtil.h"

class RenderTarget
{
public:
	RenderTarget(ID3D12Device* device, UINT width, UINT height);

	RenderTarget(const RenderTarget& rhs) = delete;
	RenderTarget& operator=(const RenderTarget& rhs) = delete;
	~RenderTarget() = default;

	UINT Width()const;
	UINT Height()const;

	ID3D12Resource* Resource();
	CD3DX12_GPU_DESCRIPTOR_HANDLE GpuSrv()const;
	CD3DX12_CPU_DESCRIPTOR_HANDLE CpuSrv()const;
	CD3DX12_CPU_DESCRIPTOR_HANDLE Rtv()const;

	D3D12_VIEWPORT Viewport()const;
	D3D12_RECT ScissorRect()const;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;

	CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhGpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuRtv;

	void BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE& CPUDescriptor,
		CD3DX12_GPU_DESCRIPTOR_HANDLE& GPUDescriptor,
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuRtv,
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
	Microsoft::WRL::ComPtr<ID3D12Resource> mRenderTarget;
};