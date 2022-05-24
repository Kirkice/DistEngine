#pragma once
#include "ResourcesManager.h"
#include "../Core/Mathf/Mathf.h"
#include "d3dApp.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class DescriptorHeap
{
public:
	DescriptorHeap();
	~DescriptorHeap();

public:
	void Build(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, int NumDescriptors);
	ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap();
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetCpuSrv(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, int index);
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetGpuSrv(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, int index);

private:
	ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;
};