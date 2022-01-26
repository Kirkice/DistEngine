#include "RenderTarget.h"

RenderTarget::RenderTarget(DXGI_FORMAT format) : mTargetFormat(format)
{
	mTargetWidth = 0;
	mTargetHeight = 0;

	mTargetFormat = format;
}

RenderTarget::~RenderTarget()
{
	if (mBackBuffer)
		mBackBuffer.Reset();
}

HRESULT RenderTarget::InitRenderTarget(UINT width, UINT height, ID3D12Device* md3dDevice)
{
	mTargetWidth = width;
	mTargetHeight = height;

	if (mBackBuffer)
		mBackBuffer.Reset();

	D3D12_RESOURCE_DESC hdrDesc;
	memset(&hdrDesc, 0, sizeof(D3D12_RESOURCE_DESC));
	hdrDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	hdrDesc.Alignment = 0;
	hdrDesc.Width = mTargetWidth;
	hdrDesc.Height = mTargetHeight;
	hdrDesc.DepthOrArraySize = 1;
	hdrDesc.MipLevels = 1;
	hdrDesc.Format = mTargetFormat;
	hdrDesc.SampleDesc.Count = 1;
	hdrDesc.SampleDesc.Quality = 0;
	hdrDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	hdrDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	D3D12_HEAP_PROPERTIES heap;
	memset(&heap, 0, sizeof(heap));
	heap.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_CLEAR_VALUE stClear = {};
	stClear.Format = mTargetFormat;
	const float clearColor[4] = { 1, 1, 1, 1 };
	memcpy(stClear.Color, &clearColor, 4 * sizeof(float));

	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&heap,
		D3D12_HEAP_FLAG_NONE,
		&hdrDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		&stClear,
		IID_PPV_ARGS(&mBackBuffer)));

	return S_OK;
}