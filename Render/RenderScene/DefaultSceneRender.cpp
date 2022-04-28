#include "DefaultSceneRender.h"
#include "../../System/EngineSystem/SystemUtils.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

namespace Dist
{
	DefaultSceneRender::DefaultSceneRender(DefaultScene scene)
	{
		mDefaultScene = scene;
	}

	DefaultSceneRender::~DefaultSceneRender()
	{

	}

	//	初始化场景渲染Render
	void DefaultSceneRender::InitSceneRender()
	{

	}

	//	更新场景渲染
	void DefaultSceneRender::UpdateSceneRender()
	{

	}

	//	构建描述符
	void DefaultSceneRender::BuildDescriptorHeaps(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList, Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer, int SwapChainBufferCount, UINT mCbvSrvUavDescriptorSize, UINT mRtvDescriptorSize)
	{
		//	Create the SRV heap.
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = 128;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mSrvDescriptorHeap)));

		//	Fill out the heap with actual descriptors.
		CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(mSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		//	加载场景图片	ICON图片
		std::vector<ComPtr<ID3D12Resource>> RenderTex2DList = mDefaultScene.LoadTextures(DefaultScene::TexturesType::RenderItem);
		std::vector<ComPtr<ID3D12Resource>> GizmoTex2DList = mDefaultScene.LoadTextures(DefaultScene::TexturesType::Gizom);

		//	Environment Tex / CubeMap
		LoadSkyBoxTextures(md3dDevice, mCommandList);
		ComPtr<ID3D12Resource> skyCubeMap = mSkyTextures["skyCubeMap"]->Resource;
		ComPtr<ID3D12Resource> diffuseIBL = mSkyTextures["DiffuseIBL"]->Resource;

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		for (UINT i = 0; i < (UINT)GizmoTex2DList.size(); ++i)
		{
			srvDesc.Format = GizmoTex2DList[i]->GetDesc().Format;
			srvDesc.Texture2D.MipLevels = GizmoTex2DList[i]->GetDesc().MipLevels;
			md3dDevice->CreateShaderResourceView(GizmoTex2DList[i].Get(), &srvDesc, hDescriptor);

			// next descriptor
			hDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
		}

		for (UINT i = 0; i < (UINT)RenderTex2DList.size(); ++i)
		{
			srvDesc.Format = RenderTex2DList[i]->GetDesc().Format;
			srvDesc.Texture2D.MipLevels = RenderTex2DList[i]->GetDesc().MipLevels;
			md3dDevice->CreateShaderResourceView(RenderTex2DList[i].Get(), &srvDesc, hDescriptor);

			// next descriptor
			hDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
		}

		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = diffuseIBL->GetDesc().MipLevels;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		srvDesc.Format = diffuseIBL->GetDesc().Format;
		md3dDevice->CreateShaderResourceView(diffuseIBL.Get(), &srvDesc, hDescriptor);
		hDescriptor.Offset(1, mCbvSrvUavDescriptorSize);

		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = skyCubeMap->GetDesc().MipLevels;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		srvDesc.Format = skyCubeMap->GetDesc().Format;
		md3dDevice->CreateShaderResourceView(skyCubeMap.Get(), &srvDesc, hDescriptor);

		mIBLTexHeapIndex = (UINT)RenderTex2DList.size() + (UINT)GizmoTex2DList.size();
		mSkyTexHeapIndex = mIBLTexHeapIndex + 1;
		mShadowMapHeapIndex = mSkyTexHeapIndex + 1;
		mRenderTargetIndex = mShadowMapHeapIndex + 1;
		mSsaoHeapIndexStart = mRenderTargetIndex + 1;
		mSsaoAmbientMapIndex = mSsaoHeapIndexStart + 3;
		mNullCubeSrvIndex = mSsaoAmbientMapIndex + 5;
		mNullTexSrvIndex1 = mNullCubeSrvIndex + 1;
		mNullTexSrvIndex2 = mNullTexSrvIndex1 + 1;

		auto nullSrv = GetCpuSrv(mNullCubeSrvIndex, mSrvDescriptorHeap);
		mNullSrv = GetGpuSrv(mNullCubeSrvIndex, mSrvDescriptorHeap);

		md3dDevice->CreateShaderResourceView(nullptr, &srvDesc, nullSrv);
		nullSrv.Offset(1, mCbvSrvUavDescriptorSize);


		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		md3dDevice->CreateShaderResourceView(nullptr, &srvDesc, nullSrv);

		nullSrv.Offset(1, mCbvSrvUavDescriptorSize);
		md3dDevice->CreateShaderResourceView(nullptr, &srvDesc, nullSrv);

		mShadowMapPass->BuildDescriptors(
			GetCpuSrv(mShadowMapHeapIndex, mSrvDescriptorHeap),
			GetGpuSrv(mShadowMapHeapIndex, mSrvDescriptorHeap),
			GetDsv(1));

		mSsao->BuildDescriptors(
			mDepthStencilBuffer.Get(),
			GetCpuSrv(mSsaoHeapIndexStart, mSrvDescriptorHeap),
			GetGpuSrv(mSsaoHeapIndexStart, mSrvDescriptorHeap),
			GetRtv(SwapChainBufferCount),
			mCbvSrvUavDescriptorSize,
			mRtvDescriptorSize);

		mTarget->BuildDescriptors(
			GetCpuSrv(mRenderTargetIndex, mSrvDescriptorHeap),
			GetGpuSrv(mRenderTargetIndex, mSrvDescriptorHeap),
			GetRtv(SwapChainBufferCount));
	}

	//	构建帧资源
	void DefaultSceneRender::BuildFrameResources(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice)
	{
		for (int i = 0; i < gNumFrameResources; ++i)
		{
			mDefaultScene.mFrameResources.push_back(std::make_unique<DefaultFrameResource>(md3dDevice.Get(),
				2, (UINT)mDefaultScene.mAllRitems.size(),
				1,
				(UINT)mDefaultScene.mMeshRender.size()));
		}
	}

	//	加载天空球图片
	void DefaultSceneRender::LoadSkyBoxTextures(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList)
	{
		std::vector<std::string> texNames =
		{
			"skyCubeMap",
			"DiffuseIBL",
		};

		std::vector<std::wstring> texFilenames =
		{
			L"Textures/DGarden_specularIBL.dds",
			L"Textures/DGarden_diffuseIBL.dds",
		};

		for (int i = 0; i < (int)texNames.size(); ++i)
		{
			// Don't create duplicates.
			if (mSkyTextures.find(texNames[i]) == std::end(mSkyTextures))
			{
				auto texMap = std::make_unique<Texture>();
				texMap->Name = texNames[i];
				texMap->Filename = texFilenames[i];
				ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
					mCommandList.Get(), texMap->Filename.c_str(),
					texMap->Resource, texMap->UploadHeap));

				mSkyTextures[texMap->Name] = std::move(texMap);
			}
		}
	}

	//	初始化阴影贴图
	void DefaultSceneRender::InitShadow(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice)
	{
		mShadowMapPass = std::make_unique<ShadowMapPass>(md3dDevice.Get(), 2048, 2048);
	}

	//	初始化SSAO
	void DefaultSceneRender::InitSSAO(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList,int width, int height)
	{
		mSsao = std::make_unique<SsaoPass>(md3dDevice.Get(), mCommandList.Get(), width, height);
	}

	//	初始化渲染目标
	void DefaultSceneRender::InitRenderTarget(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, int width, int height)
	{
		mTarget = std::make_unique<RenderTexture>(md3dDevice.Get(), width, height);
	}
}