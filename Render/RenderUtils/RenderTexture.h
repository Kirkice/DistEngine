#pragma once
#include "../../System/EngineSystem/SystemUtils.h"

namespace Dist
{
	class RenderTexture
	{
	public:
		RenderTexture(ID3D12Device* device, UINT width, UINT height);

		RenderTexture(const RenderTexture& rhs) = delete;
		RenderTexture& operator=(const RenderTexture& rhs) = delete;
		~RenderTexture() = default;

		UINT Width()const;
		UINT Height()const;

		void SetIndex(UINT Index);
		UINT GetIndex();

		ID3D12Resource* Resource();
		CD3DX12_GPU_DESCRIPTOR_HANDLE GpuSrv()const;
		CD3DX12_CPU_DESCRIPTOR_HANDLE CpuSrv()const;
		CD3DX12_CPU_DESCRIPTOR_HANDLE Rtv()const;

		D3D12_VIEWPORT Viewport()const;
		D3D12_RECT ScissorRect()const;

		void BuildDescriptors(
			CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
			CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
			CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuRtv);

	private:
		void BuildDescriptors();
		void BuildResource();

	private:
		ID3D12Device* md3dDevice = nullptr;

		D3D12_VIEWPORT mViewport;
		D3D12_RECT mScissorRect;

		UINT Index = 0;
		UINT mWidth = 0;
		UINT mHeight = 0;

		DXGI_FORMAT mFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuSrv;
		CD3DX12_GPU_DESCRIPTOR_HANDLE mhGpuSrv;
		CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuRtv;

		Microsoft::WRL::ComPtr<ID3D12Resource> mRenderTarget;
	};
}