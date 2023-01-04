#include "DepthPass.h"

DepthPass::DepthPass(ID3D12Device* device)
{
	md3dDevice = device;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE DepthPass::Srv()const
{
	return mhGpuSrv;
}

void DepthPass::BuildDescriptors(
	ID3D12Resource* depthStencilBuffer,
	CD3DX12_CPU_DESCRIPTOR_HANDLE& CPUDescriptor,
	CD3DX12_GPU_DESCRIPTOR_HANDLE& GPUDescriptor,
	UINT mCbvSrvUavDescriptorSize
)
{
	// Save references to the descriptors. 
	mhCpuSrv = CPUDescriptor;
	mhGpuSrv = GPUDescriptor;

	CPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
	GPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);

	//  Create the descriptors
	BuildDescriptors(depthStencilBuffer);
}

void DepthPass::BuildDescriptors(ID3D12Resource* depthStencilBuffer)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	md3dDevice->CreateShaderResourceView(depthStencilBuffer, &srvDesc, mhCpuSrv );
}