#include "DescriptorHeap.h"

DescriptorHeap::DescriptorHeap()
{

}

DescriptorHeap::~DescriptorHeap()
{

}

void DescriptorHeap::Build(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, int NumDescriptors)
{
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = NumDescriptors;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mSrvDescriptorHeap)));

	mDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

/// <summary>
/// GetDescriptorHeap
/// </summary>
/// <returns></returns>
ComPtr<ID3D12DescriptorHeap> DescriptorHeap::GetDescriptorHeap()
{
	return mSrvDescriptorHeap;
}

/// <summary>
/// GetCpuSrv
/// </summary>
/// <returns></returns>
CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCpuSrv(int index)
{
	auto srv = CD3DX12_CPU_DESCRIPTOR_HANDLE(mSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	srv.Offset(index, mDescriptorSize);
	return srv;
}

/// <summary>
/// GetCpuSrv
/// </summary>
/// <returns></returns>
CD3DX12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGpuSrv(int index)
{
	auto srv = CD3DX12_GPU_DESCRIPTOR_HANDLE(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	srv.Offset(index, mDescriptorSize);
	return srv;
}