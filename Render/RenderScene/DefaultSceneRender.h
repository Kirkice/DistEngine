#pragma once
#include "../RenderPass/ShadowMapPass.h"
#include "../RenderUtils/RenderTexture.h"
#include "../RenderPass/ScreenSpaceAmbientOcclusionPass.h"
#include "../../System/Scene/DefaultScene.h"
#include "../../System/EngineSystem/SystemUtils.h"
#include <ResourcesManager.h>

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

namespace Dist
{
	class DefaultSceneRender:public DefaultScene
	{
	public:
		DefaultSceneRender() = default;
		DefaultSceneRender(const DefaultSceneRender& rhs) = delete;
		DefaultSceneRender& operator=(const DefaultSceneRender& rhs) = delete;
		virtual ~DefaultSceneRender() = default;

	public:

		//	初始化场景渲染
		void InitSceneRender(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList, Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer, int SwapChainBufferCount, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap, UINT mDsvDescriptorSize, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap, UINT mRtvDescriptorSize, UINT mCbvSrvUavDescriptorSize, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSRV, ResourceManager& manager, int width, int height);

		//	更新场景渲染
		void UpdateSceneRender(const GameTimer& gt, int mClientWidth, int mClientHeight);

		//	构建PSO
		void BuildPSO(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, DXGI_FORMAT mBackBufferFormat, bool m4xMsaaState, UINT m4xMsaaQuality, DXGI_FORMAT mDepthStencilFormat);

		//	GetCPUSRV
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCpuSrv(int index, ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap, UINT mCbvSrvUavDescriptorSize)const;

		//	GetGPUSRV
		CD3DX12_GPU_DESCRIPTOR_HANDLE GetGpuSrv(int index, ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap, UINT mCbvSrvUavDescriptorSize)const;

		//	GetDSV
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetDsv(int index, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap, UINT mDsvDescriptorSize)const;

		//	GetRTV
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetRtv(int index, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap, UINT mRtvDescriptorSize)const;

		//	渲染阴影
		std::unique_ptr<ShadowMapPass> mShadowMapPass;

		//	ssao
		std::unique_ptr<SsaoPass> mSsao;

		//	渲染目标
		std::unique_ptr<RenderTexture> mTarget;

		//  GBuffer
		std::unique_ptr<RenderTexture> mGBuffer[4];
		//	PSO
		std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;

		//	SceneBounds
		DirectX::BoundingSphere mSceneBounds;

		//	Pass
		PassConstants mMainPassCB;  // index 0 of pass cbuffer.
		PassConstants mShadowPassCB;// index 1 of pass cbuffer.

		//Shadow
		float mLightNearZ = 0.0f;
		float mLightFarZ = 0.0f;
		XMFLOAT3 mLightPosW;
		XMFLOAT4X4 mLightView = Dist::Identity4x4();
		XMFLOAT4X4 mLightProj = Dist::Identity4x4();
		XMFLOAT4X4 mShadowTransform = Dist::Identity4x4();

	private:

		//-------------------------Init-------------------------------

		//	构建描述符
		void BuildDescriptorHeaps(Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer, int SwapChainBufferCount, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap, UINT mDsvDescriptorSize, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSRV, ResourceManager& manager, UINT mRtvDescriptorSize, UINT mCbvSrvUavDescriptorSize);

		//	构建帧资源
		void BuildFrameResources(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice);

		//	初始化阴影
		void InitShadow(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice);

		//	初始化SSAO
		void InitSSAO(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList, int width, int height);

		//	初始化 渲染目标
		void InitRenderTarget(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, int width, int height);

		//	初始化GBuffer
		void InitGBuffer(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, int width, int height);
		//-------------------------Update-------------------------------

		//	更新阴影位置
		void UpdateShadowTransform(const GameTimer& gt);

		//	更新Pass常量Buffer
		void UpdateMainPassCB(const GameTimer& gt, int mClientWidth, int mClientHeight);

		//	更新阴影常量Buffer
		void UpdateShadowPassCB(const GameTimer& gt);

		//	更新SSAO常量Buffer
		void UpdateSsaoCB(const GameTimer& gt);

	};
}