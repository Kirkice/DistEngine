#ifndef GBUFFER_H
#define GBUFFER_H
#pragma once

#include "d3dUtil.h"
#include "FrameResource.h"

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

	ID3D12Resource* GetGBuffer0();
	ID3D12Resource* GetGBuffer1();
	ID3D12Resource* GetGBuffer2();
	ID3D12Resource* GetGBuffer3();

	CD3DX12_GPU_DESCRIPTOR_HANDLE GBuffer0Srv()const;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GBuffer1Srv()const;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GBuffer2Srv()const;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GBuffer3Srv()const;

	void BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE& CPUDescriptor,
		CD3DX12_GPU_DESCRIPTOR_HANDLE& GPUDescriptor,
		UINT mCbvSrvUavDescriptorSize
	);

private:

	CD3DX12_CPU_DESCRIPTOR_HANDLE mhGBuffer0CpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhGBuffer1CpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhGBuffer2CpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhGBuffer3CpuSrv;

	CD3DX12_GPU_DESCRIPTOR_HANDLE mhGBuffer0GpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhGBuffer1GpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhGBuffer2GpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhGBuffer3GpuSrv;


	Microsoft::WRL::ComPtr<ID3D12Resource> GBuffer0;
	Microsoft::WRL::ComPtr<ID3D12Resource> GBuffer1;
	Microsoft::WRL::ComPtr<ID3D12Resource> GBuffer2;
	Microsoft::WRL::ComPtr<ID3D12Resource> GBuffer3;

	ID3D12Device* md3dDevice = nullptr;

	void BuildResource();

	DXGI_FORMAT mFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissorRect;

	UINT mWidth = 0;
	UINT mHeight = 0;
};

#endif