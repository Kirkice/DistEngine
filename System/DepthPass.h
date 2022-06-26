#pragma once

#include "d3dUtil.h"

class DepthPass
{
public:
	DepthPass(ID3D12Device* device);
	DepthPass(const DepthPass& rhs) = delete;
	DepthPass& operator=(const DepthPass& rhs) = delete;
	~DepthPass() = default;

	CD3DX12_GPU_DESCRIPTOR_HANDLE Srv()const;

	void BuildDescriptors(
		ID3D12Resource* depthStencilBuffer,
		CD3DX12_CPU_DESCRIPTOR_HANDLE& CPUDescriptor,
		CD3DX12_GPU_DESCRIPTOR_HANDLE& GPUDescriptor,
		UINT mCbvSrvUavDescriptorSize
	);

private:
	void BuildDescriptors(ID3D12Resource* depthStencilBuffer);

	ID3D12Device* md3dDevice = nullptr;

	CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhGpuSrv;
};