#include "RenderTexture.h"

RenderTexture::RenderTexture(ID3D12Device* device, UINT width, UINT height)
{
	md3dDevice = device;

	mWidth = width;
	mHeight = height;

	mViewport = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f };
	mScissorRect = { 0, 0, (int)width, (int)height };

	BuildResource();
}

UINT RenderTexture::Width()const
{
	return mWidth;
}

UINT RenderTexture::Height()const
{
	return mHeight;
}

ID3D12Resource* RenderTexture::Resource()
{
	return mRenderTexture.Get();
}

CD3DX12_GPU_DESCRIPTOR_HANDLE RenderTexture::GpuSrv()const
{
	return mhGpuSrv;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE RenderTexture::CpuSrv()const
{
	return mhCpuSrv;
}

D3D12_VIEWPORT RenderTexture::Viewport()const  
{
	return mViewport;
}

D3D12_RECT RenderTexture::ScissorRect()const 
{
	return mScissorRect;
}

void RenderTexture::BuildDescriptors(
	CD3DX12_CPU_DESCRIPTOR_HANDLE& CPUDescriptor,
	CD3DX12_GPU_DESCRIPTOR_HANDLE& GPUDescriptor,
	UINT mCbvSrvUavDescriptorSize
)
{
	// Save references to the descriptors. 
	mhCpuSrv = CPUDescriptor;
	mhGpuSrv = GPUDescriptor;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = mTargetFormat;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	md3dDevice->CreateShaderResourceView(mRenderTexture.Get(), &srvDesc, CPUDescriptor);

	CPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
	GPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
}

//Build Resource
void RenderTexture::BuildResource()
{
	D3D12_RESOURCE_DESC texDesc;
	ZeroMemory(&texDesc, 0, sizeof(D3D12_RESOURCE_DESC));
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = mWidth;
	texDesc.Height = mHeight;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = mTargetFormat;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;


	D3D12_CLEAR_VALUE stClear = {};
	stClear.Format = mTargetFormat;
	const float clearColor[4] = { 1, 1, 1, 1 };
	memcpy(stClear.Color, &clearColor, 4 * sizeof(float));

	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		&stClear,
		IID_PPV_ARGS(&mRenderTexture)));
}
