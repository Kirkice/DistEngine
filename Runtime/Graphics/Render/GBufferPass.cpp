#include "GBufferPass.h"
#include <DirectXPackedVector.h>

using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;

const int GBufferSize = 4;

GBuffer::GBuffer(
	ID3D12Device* device,
	UINT width,
	UINT height
)
{
	md3dDevice = device;

	OnResize(width, height);
}

/// <summary>
/// 检查宽高
/// </summary>
/// <param name="newWidth"></param>
/// <param name="newHeight"></param>
void GBuffer::OnResize(UINT newWidth, UINT newHeight)
{
	if (mWidth != newWidth || mHeight != newHeight)
	{
		mWidth = newWidth;
		mHeight = newHeight;

		// We render to ambient map at half the resolution.
		mViewport.TopLeftX = 0.0f;
		mViewport.TopLeftY = 0.0f;
		mViewport.Width = mWidth / 2.0f;
		mViewport.Height = mHeight / 2.0f;
		mViewport.MinDepth = 0.0f;
		mViewport.MaxDepth = 1.0f;

		mScissorRect = { 0, 0, (int)mWidth / 2, (int)mHeight / 2 };

		BuildResources();
	}
}

//Build Resource
void GBuffer::BuildResources()
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


	float ClearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	CD3DX12_CLEAR_VALUE optClear(mFormat, ClearColor);

	for (size_t i = 0; i < GBufferSize; i++)
	{
		ThrowIfFailed(md3dDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&texDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
			&optClear,
			IID_PPV_ARGS(&GBufferResources[i])));
	}
}

/// <summary>
/// 获取GBuffer Resources
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
ID3D12Resource* GBuffer::GetGBuffer(int index)
{
	return GBufferResources[index].Get();
}

/// <summary>
/// 获取GBuffer的CPU SRV
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
CD3DX12_CPU_DESCRIPTOR_HANDLE GBuffer::GET_GBUFFER_CPU_SRV(int index)const
{
	return mhGBufferCpuSrv[index];
}

/// <summary>
/// 获取GBuffer的GPU SRV
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
CD3DX12_GPU_DESCRIPTOR_HANDLE GBuffer::GET_GBUFFER_GPU_SRV(int index)const
{
	return mhGBufferGpuSrv[index];
}

/// <summary>
/// 构建描述符
/// </summary>
/// <param name="CPUDescriptor"></param>
/// <param name="GPUDescriptor"></param>
/// <param name="mCbvSrvUavDescriptorSize"></param>
void GBuffer::BuildDescriptors(
	CD3DX12_CPU_DESCRIPTOR_HANDLE& CPUDescriptor,
	CD3DX12_GPU_DESCRIPTOR_HANDLE& GPUDescriptor,
	UINT mCbvSrvUavDescriptorSize
)
{
	for (size_t i = 0; i < GBufferSize; i++)
	{
		mhGBufferCpuSrv[i] = CPUDescriptor;
		CPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);

		mhGBufferGpuSrv[i] = GPUDescriptor;
		GPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
	}

	RebuildDescriptors();
}

void GBuffer::RebuildDescriptors()
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = mFormat;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	for (size_t i = 0; i < GBufferSize; i++)
	{
		md3dDevice->CreateShaderResourceView(GBufferResources[i].Get(), &srvDesc, mhGBufferCpuSrv[i]);
	}
}

void GBuffer::SetPSOs(ID3D12PipelineState* gbufferPSO)
{
	mGBufferPSO = gbufferPSO;
}