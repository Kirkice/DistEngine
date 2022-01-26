#pragma once

#include "d3dUtil.h"

class RenderTarget
{
public:

	RenderTarget() = default;
	RenderTarget(DXGI_FORMAT format);
	~RenderTarget();

	HRESULT InitRenderTarget(UINT width, UINT height, ID3D12Device* md3dDevice);

	int GetWidth() { return mTargetWidth; }
	int GetHeight() { return mTargetHeight; }
	DXGI_FORMAT GetFormat() { return mTargetFormat; }
	ID3D12Resource* GetResource() { return mBackBuffer.Get(); }

private:

	UINT mTargetWidth;
	UINT mTargetHeight;
	DXGI_FORMAT mTargetFormat;
	Microsoft::WRL::ComPtr<ID3D12Resource> mBackBuffer;
};