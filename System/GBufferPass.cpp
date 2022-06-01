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
	/// <summary>
	/// GBuffer0
	/// </summary>
	D3D12_RESOURCE_DESC GBuffer0Desc;
	ZeroMemory(&GBuffer0Desc, 0, sizeof(D3D12_RESOURCE_DESC));
	GBuffer0Desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	GBuffer0Desc.Alignment = 0;
	GBuffer0Desc.Width = mWidth;
	GBuffer0Desc.Height = mHeight;
	GBuffer0Desc.DepthOrArraySize = 1;
	GBuffer0Desc.MipLevels = 1;
	GBuffer0Desc.Format = GBuffer0Format;
	GBuffer0Desc.SampleDesc.Count = 1;
	GBuffer0Desc.SampleDesc.Quality = 0;
	GBuffer0Desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	GBuffer0Desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;


	D3D12_CLEAR_VALUE stClear0 = {};
	stClear0.Format = GBuffer0Format;
	const float clearColor0[4] = { 1, 1, 1, 1 };
	memcpy(stClear0.Color, &clearColor0, 4 * sizeof(float));

	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&GBuffer0Desc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		&stClear0,
		IID_PPV_ARGS(&GBuffer0)));

	/// <summary>
	/// GBuffer1
	/// </summary>
	D3D12_RESOURCE_DESC GBuffer1Desc;
	ZeroMemory(&GBuffer1Desc, 0, sizeof(D3D12_RESOURCE_DESC));
	GBuffer1Desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	GBuffer1Desc.Alignment = 0;
	GBuffer1Desc.Width = mWidth;
	GBuffer1Desc.Height = mHeight;
	GBuffer1Desc.DepthOrArraySize = 1;
	GBuffer1Desc.MipLevels = 1;
	GBuffer1Desc.Format = GBuffer0Format;
	GBuffer1Desc.SampleDesc.Count = 1;
	GBuffer1Desc.SampleDesc.Quality = 0;
	GBuffer1Desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	GBuffer1Desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;


	D3D12_CLEAR_VALUE stClear1 = {};
	stClear1.Format = GBuffer1Format;
	const float clearColor1[4] = { 1, 1, 1, 1 };
	memcpy(stClear1.Color, &clearColor1, 4 * sizeof(float));

	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&GBuffer1Desc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		&stClear1,
		IID_PPV_ARGS(&GBuffer1)));

	/// <summary>
	/// GBuffer2
	/// </summary>
	D3D12_RESOURCE_DESC GBuffer2Desc;
	ZeroMemory(&GBuffer2Desc, 0, sizeof(D3D12_RESOURCE_DESC));
	GBuffer2Desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	GBuffer2Desc.Alignment = 0;
	GBuffer2Desc.Width = mWidth;
	GBuffer2Desc.Height = mHeight;
	GBuffer2Desc.DepthOrArraySize = 1;
	GBuffer2Desc.MipLevels = 1;
	GBuffer2Desc.Format = GBuffer0Format;
	GBuffer2Desc.SampleDesc.Count = 1;
	GBuffer2Desc.SampleDesc.Quality = 0;
	GBuffer2Desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	GBuffer2Desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;


	D3D12_CLEAR_VALUE stClear2 = {};
	stClear2.Format = GBuffer2Format;
	const float clearColor2[4] = { 1, 1, 1, 1 };
	memcpy(stClear2.Color, &clearColor2, 4 * sizeof(float));

	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&GBuffer2Desc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		&stClear2,
		IID_PPV_ARGS(&GBuffer2)));

	/// <summary>
	/// GBuffer3
	/// </summary>
	D3D12_RESOURCE_DESC GBuffer3Desc;
	ZeroMemory(&GBuffer3Desc, 0, sizeof(D3D12_RESOURCE_DESC));
	GBuffer3Desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	GBuffer3Desc.Alignment = 0;
	GBuffer3Desc.Width = mWidth;
	GBuffer3Desc.Height = mHeight;
	GBuffer3Desc.DepthOrArraySize = 1;
	GBuffer3Desc.MipLevels = 1;
	GBuffer3Desc.Format = GBuffer0Format;
	GBuffer3Desc.SampleDesc.Count = 1;
	GBuffer3Desc.SampleDesc.Quality = 0;
	GBuffer3Desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	GBuffer3Desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;


	D3D12_CLEAR_VALUE stClear3 = {};
	stClear3.Format = GBuffer3Format;
	const float clearColor3[4] = { 1, 1, 1, 1 };
	memcpy(stClear3.Color, &clearColor3, 4 * sizeof(float));

	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&GBuffer3Desc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		&stClear3,
		IID_PPV_ARGS(&GBuffer3)));
}


void GBuffer::BuildDescriptors(
	CD3DX12_CPU_DESCRIPTOR_HANDLE& CPUDescriptor,
	CD3DX12_GPU_DESCRIPTOR_HANDLE& GPUDescriptor,
	UINT mCbvSrvUavDescriptorSize
)
{
	mhGBuffer0CpuSrv = CPUDescriptor;
	mhGBuffer1CpuSrv = CPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
	mhGBuffer2CpuSrv = CPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
	mhGBuffer3CpuSrv = CPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);

	mhGBuffer0GpuSrv = GPUDescriptor;
	mhGBuffer1GpuSrv = GPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
	mhGBuffer2GpuSrv = GPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
	mhGBuffer3GpuSrv = GPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);

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

CD3DX12_GPU_DESCRIPTOR_HANDLE GBuffer::GBuffer0Srv()const
{
	return mhGBuffer0GpuSrv;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE GBuffer::GBuffer1Srv()const
{
	return mhGBuffer1GpuSrv;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE GBuffer::GBuffer2Srv()const
{
	return mhGBuffer2GpuSrv;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE GBuffer::GBuffer3Srv()const
{
	return mhGBuffer3GpuSrv;
}