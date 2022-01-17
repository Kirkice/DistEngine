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

	static const DXGI_FORMAT mColorAttachmentFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;

	CD3DX12_CPU_DESCRIPTOR_HANDLE mColorAttachmentRtv()const;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mColorAttachmentSrv()const;

	void BuildDescriptors(
		ID3D12Resource* depthStencilBuffer,
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
		CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuRtv,
		UINT cbvSrvUavDescriptorSize,
		UINT rtvDescriptorSize);

	void RebuildDescriptors(ID3D12Resource* depthStencilBuffer);

	void SetPSOs(ID3D12PipelineState* colorPso);

	void OnResize(UINT newWidth, UINT newHeight);

	void ComputeColor(ID3D12GraphicsCommandList* cmdList, FrameResource* currFrame);

private:
	void BuildResources();

private:
	ID3D12Device* md3dDevice;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> mColorRootSig;
	ID3D12PipelineState* mColorPso = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> mColorAttachment;

	CD3DX12_CPU_DESCRIPTOR_HANDLE mColorAttachmentCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mColorAttachmentGpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mColorAttachmentCpuRtv;

	UINT mRenderTargetWidth;
	UINT mRenderTargetHeight;

	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissorRect;
};
#endif
