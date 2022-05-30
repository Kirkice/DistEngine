#ifndef GBUFFER_H
#define GBUFFER_H
#pragma once

#include "d3dUtil.h"
#include "FrameResource.h"

class GBuffer
{
public:

	GBuffer(
		UINT width, 
		UINT height
	);

	GBuffer(const GBuffer& rhs) = delete;
	GBuffer& operator=(const GBuffer& rhs) = delete;
	~GBuffer() = default;

	static const DXGI_FORMAT GBuffer0Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	static const DXGI_FORMAT GBuffer1Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	static const DXGI_FORMAT GBuffer2Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	static const DXGI_FORMAT GBuffer3Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

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
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuRtv,
		UINT mCbvSrvUavDescriptorSize
	);

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> GBuffer0;
	Microsoft::WRL::ComPtr<ID3D12Resource> GBuffer1;
	Microsoft::WRL::ComPtr<ID3D12Resource> GBuffer2;
	Microsoft::WRL::ComPtr<ID3D12Resource> GBuffer3;

	void BuildResource();

	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissorRect;

	UINT mWidth = 0;
	UINT mHeight = 0;
};

#endif