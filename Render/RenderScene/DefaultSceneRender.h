#pragma once
#include "../RenderPass/ShadowMapPass.h"
#include "../RenderUtils/RenderTexture.h"
#include "../RenderPass/ScreenSpaceAmbientOcclusionPass.h"
#include "../../System/Scene/DefaultScene.h"
#include "../../System/EngineSystem/SystemUtils.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

namespace Dist
{
	class DefaultSceneRender
	{
	public:

		//	Ĭ�ϳ���
		DefaultScene mDefaultScene;

		//	Default SRV DescriptorHeap
		ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;

		//	�����ͼƬ
		std::unordered_map<std::string, std::unique_ptr<Texture>> mSkyTextures;

		//	�յ�Handle
		CD3DX12_GPU_DESCRIPTOR_HANDLE mNullSrv;

		//	Index
		UINT mIBLTexHeapIndex = 0;
		UINT mSkyTexHeapIndex = 0;
		UINT mRenderTargetIndex = 0;
		UINT mCopyColorIndex = 0;
		UINT mShadowMapHeapIndex = 0;
		UINT mSsaoHeapIndexStart = 0;
		UINT mSsaoAmbientMapIndex = 0;

		UINT mNullCubeSrvIndex = 0;
		UINT mNullTexSrvIndex1 = 0;
		UINT mNullTexSrvIndex2 = 0;

	public:
		DefaultSceneRender(DefaultScene scene);
		~DefaultSceneRender();

		//	��ʼ��������Ⱦ
		void InitSceneRender();

		//	���³�����Ⱦ
		void UpdateSceneRender();

		//	GetCPUSRV
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCpuSrv(int index, ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap)const;

		//	GetGPUSRV
		CD3DX12_GPU_DESCRIPTOR_HANDLE GetGpuSrv(int index, ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap)const;

		//	GetDSV
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetDsv(int index)const;

		//	GetRTV
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetRtv(int index)const;

		//	��Ⱦ��Ӱ
		std::unique_ptr<ShadowMapPass> mShadowMapPass;

		//	ssao
		std::unique_ptr<SsaoPass> mSsao;

		//	��ȾĿ��
		std::unique_ptr<RenderTexture> mTarget;

	private:

		//-------------------------Init-------------------------------

		//	����������
		void BuildDescriptorHeaps(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList, Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer, int SwapChainBufferCount, UINT mCbvSrvUavDescriptorSize, UINT mRtvDescriptorSize);

		//	����֡��Դ
		void BuildFrameResources(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice);

		//	���������ͼƬ
		void LoadSkyBoxTextures(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList);

		//	��ʼ����Ӱ
		void InitShadow(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice);

		//	��ʼ��SSAO
		void InitSSAO(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList, int width, int height);

		//	��ʼ�� ��ȾĿ��
		void InitRenderTarget(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, int width, int height);




		//-------------------------Update-------------------------------

		//	������Ӱλ��
		void UpdateShadowTransform(const GameTimer& gt);

		//	����Pass����Buffer
		void UpdateMainPassCB(const GameTimer& gt); 

		//	������Ӱ����Buffer
		void UpdateShadowPassCB(const GameTimer& gt);

		//	����SSAO����Buffer
		void UpdateSsaoCB(const GameTimer& gt);

	};
}