#include "CopyColor.h"
#include <DirectXPackedVector.h>

using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;

CopyColor::CopyColor(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, UINT width, UINT height)
{
	md3dDevice = device;
	OnResize(width, height);
}

void CopyColor::OnResize(UINT newWidth, UINT newHeight)
{
	if (mRenderTargetWidth != newWidth || mRenderTargetHeight != newHeight)
	{
		mRenderTargetWidth = newWidth;
		mRenderTargetHeight = newHeight;

		// We render to ambient map at half the resolution.
		mViewport.TopLeftX = 0.0f;
		mViewport.TopLeftY = 0.0f;
		mViewport.Width = mRenderTargetWidth / 2.0f;
		mViewport.Height = mRenderTargetHeight / 2.0f;
		mViewport.MinDepth = 0.0f;
		mViewport.MaxDepth = 1.0f;

		mScissorRect = { 0, 0, (int)mRenderTargetWidth / 2, (int)mRenderTargetHeight / 2 };

		BuildResources();
	}
}

void CopyColor::BuildResources()
{
	mCopyColorMap = nullptr;

	D3D12_RESOURCE_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = mRenderTargetWidth;
	texDesc.Height = mRenderTargetHeight;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = CopyColorFormat;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	float copyColorClearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	CD3DX12_CLEAR_VALUE optClear(CopyColorFormat, copyColorClearColor);
	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&optClear,
		IID_PPV_ARGS(&mCopyColorMap)));
}

ID3D12Resource* CopyColor::GetCopyColorMap()
{
	return mCopyColorMap.Get();
}

CD3DX12_CPU_DESCRIPTOR_HANDLE CopyColor::CopyColorRtv()const
{
	return mhCopyColorCpuRtv;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE CopyColor::CopyColorSrv()const
{
	return mhCopyColorGpuSrv;
}

void CopyColor::BuildDescriptors(
	CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
	CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
	CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuRtv)
{
	mhCopyColorCpuSrv = hCpuSrv;
	mhCopyColorGpuSrv = hGpuSrv;
	mhCopyColorCpuRtv = hCpuRtv;
}

void CopyColor::SetPSOs(ID3D12PipelineState* copyColorPso)
{
	mCopyColorPso = copyColorPso;
}

void CopyColor::RenderCopyColor(ID3D12GraphicsCommandList* cmdList, FrameResource* currFrame)
{
	cmdList->RSSetViewports(1, &mViewport);
	cmdList->RSSetScissorRects(1, &mScissorRect);

	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mCopyColorMap.Get(),D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));

	float clearValue[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	cmdList->ClearRenderTargetView(mhCopyColorCpuRtv, clearValue, 0, nullptr);

	cmdList->OMSetRenderTargets(1, &mhCopyColorCpuRtv, true, nullptr);

	//cmdList->SetGraphicsRootDescriptorTable(2, mhNormalMapGpuSrv);
	cmdList->SetPipelineState(mCopyColorPso);

	cmdList->IASetVertexBuffers(0, 0, nullptr);
	cmdList->IASetIndexBuffer(nullptr);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->DrawInstanced(6, 1, 0, 0);

	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mCopyColorMap.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ));
}