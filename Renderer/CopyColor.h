#ifndef COPY_COLOR_H
#define COPY_COLOR_H

#pragma once

#include "d3dUtil.h"
#include "FrameResource.h"

class CopyColor
{
public:
	CopyColor(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, UINT width, UINT height);

	CopyColor(const CopyColor& rhs) = delete;
	CopyColor& operator=(const CopyColor& rhs) = delete;
	~CopyColor() = default;


	static const DXGI_FORMAT CopyColorFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;

	ID3D12Resource* GetCopyColorMap();

	CD3DX12_CPU_DESCRIPTOR_HANDLE CopyColorRtv()const;
	CD3DX12_GPU_DESCRIPTOR_HANDLE CopyColorSrv()const;

	void BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
		CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuRtv);

	void SetPSOs(ID3D12PipelineState* copyColorPso);

	void OnResize(UINT newWidth, UINT newHeight);

	void RenderCopyColor(ID3D12GraphicsCommandList* cmdList, FrameResource* currFrame);

private:
	void BuildResources();

private:
	ID3D12Device* md3dDevice;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> mCopyColorRootSig;

	ID3D12PipelineState* mCopyColorPso = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> mCopyColorMap;

	CD3DX12_CPU_DESCRIPTOR_HANDLE mhCopyColorCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhCopyColorGpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhCopyColorCpuRtv;

	UINT mRenderTargetWidth;
	UINT mRenderTargetHeight;

	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissorRect;
};


#endif