#include "GBufferPass.h"
#include <DirectXPackedVector.h>

using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;

GBuffer::GBuffer(
	ID3D12Device* device,
	UINT width,
	UINT height
)
{
	md3dDevice = device;

	mWidth = width;
	mHeight = height;

	mViewport = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f };
	mScissorRect = { 0, 0, (int)width, (int)height };
	BuildResource();
}


void GBuffer::BuildResource()
{
	D3D12_RESOURCE_DESC texDesc;
	ZeroMemory(&texDesc, 0, sizeof(D3D12_RESOURCE_DESC));
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = mWidth;
	texDesc.Height = mHeight;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = mFormat;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;


	D3D12_CLEAR_VALUE stClear = {};
	stClear.Format = mFormat;
	const float clearColor[4] = { 1, 1, 1, 1 };
	memcpy(stClear.Color, &clearColor, 4 * sizeof(float));

	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		&stClear,
		IID_PPV_ARGS(&GBuffer0)));

	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		&stClear,
		IID_PPV_ARGS(&GBuffer1)));

	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		&stClear,
		IID_PPV_ARGS(&GBuffer2)));

	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		&stClear,
		IID_PPV_ARGS(&GBuffer3)));
}


void GBuffer::BuildDescriptors(
	CD3DX12_CPU_DESCRIPTOR_HANDLE& CPUDescriptor,
	CD3DX12_GPU_DESCRIPTOR_HANDLE& GPUDescriptor,
	UINT mCbvSrvUavDescriptorSize
)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = mFormat;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;


	mhGBuffer0CpuSrv = CPUDescriptor;
	mhGBuffer0GpuSrv = GPUDescriptor;
	md3dDevice->CreateShaderResourceView(GBuffer0.Get(), &srvDesc, CPUDescriptor);
	CPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
	GPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);

	mhGBuffer1CpuSrv = CPUDescriptor;
	mhGBuffer1GpuSrv = GPUDescriptor;
	md3dDevice->CreateShaderResourceView(GBuffer1.Get(), &srvDesc, CPUDescriptor);
	CPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
	GPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);

	mhGBuffer2CpuSrv = CPUDescriptor;
	mhGBuffer2GpuSrv = GPUDescriptor;
	md3dDevice->CreateShaderResourceView(GBuffer2.Get(), &srvDesc, CPUDescriptor);
	CPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
	GPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);

	mhGBuffer3CpuSrv = CPUDescriptor;
	mhGBuffer3GpuSrv = GPUDescriptor;
	md3dDevice->CreateShaderResourceView(GBuffer3.Get(), &srvDesc, CPUDescriptor);
	CPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
	GPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
}

ID3D12Resource* GBuffer::GetGBuffer0()
{
	return GBuffer0.Get();
}

ID3D12Resource* GBuffer::GetGBuffer1()
{
	return GBuffer1.Get();
}

ID3D12Resource* GBuffer::GetGBuffer2()
{
	return GBuffer2.Get();
}

ID3D12Resource* GBuffer::GetGBuffer3()
{
	return GBuffer3.Get();
}

CD3DX12_GPU_DESCRIPTOR_HANDLE GBuffer::GBuffer0GPUSrv()const
{
	return mhGBuffer0GpuSrv;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE GBuffer::GBuffer1GPUSrv()const
{
	return mhGBuffer1GpuSrv;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE GBuffer::GBuffer2GPUSrv()const
{
	return mhGBuffer2GpuSrv;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE GBuffer::GBuffer3GPUSrv()const
{
	return mhGBuffer3GpuSrv;
}

/// <summary>
/// CPU SRV
/// </summary>
/// <returns></returns>
CD3DX12_CPU_DESCRIPTOR_HANDLE GBuffer::GBuffer0CPUSrv()const
{
	return mhGBuffer0CpuSrv;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE GBuffer::GBuffer1CPUSrv()const
{
	return mhGBuffer1CpuSrv;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE GBuffer::GBuffer2CPUSrv()const
{
	return mhGBuffer2CpuSrv;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE GBuffer::GBuffer3CPUSrv()const
{
	return mhGBuffer3CpuSrv;
}