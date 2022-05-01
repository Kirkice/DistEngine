#include "DefaultSceneRender.h"
#include "../../System/EngineSystem/SystemUtils.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

namespace Dist
{
	//	初始化场景渲染Render
	void DefaultSceneRender::InitSceneRender(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList, Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer, int SwapChainBufferCount, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap, UINT mDsvDescriptorSize, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap, UINT mRtvDescriptorSize, UINT mCbvSrvUavDescriptorSize, int width, int height)
	{
		mSceneBounds.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
		mSceneBounds.Radius = sqrtf(10.0f * 10.0f + 15.0f * 15.0f);

		//	初始化阴影
		InitShadow(md3dDevice);

		//	初始化SSAO
		InitSSAO(md3dDevice, mCommandList, width, height);

		//	初始化渲染目标
		InitRenderTarget(md3dDevice, width, height);

		//	构建场景描述符
		BuildDescriptorHeaps(md3dDevice, mCommandList, mDepthStencilBuffer, SwapChainBufferCount, mDsvHeap, mDsvDescriptorSize, mRtvHeap, mRtvDescriptorSize, mCbvSrvUavDescriptorSize);

		//	构建帧资源
		BuildFrameResources(md3dDevice);
	}

	//	更新场景渲染
	void DefaultSceneRender::UpdateSceneRender(const GameTimer& gt, int mClientWidth, int mClientHeight)
	{
		//	更新阴影位置
		UpdateShadowTransform(gt);

		//	更新Pass常量Buffer
		UpdateMainPassCB(gt, mClientWidth, mClientHeight);

		//	更新阴影常量Buffer
		UpdateShadowPassCB(gt);

		//	更新SSAO常量Buffer
		UpdateSsaoCB(gt);
	}

	//	获取CPU SRV
	CD3DX12_CPU_DESCRIPTOR_HANDLE DefaultSceneRender::GetCpuSrv(int index, ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap, UINT mCbvSrvUavDescriptorSize)const
	{
		auto srv = CD3DX12_CPU_DESCRIPTOR_HANDLE(mSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		srv.Offset(index, mCbvSrvUavDescriptorSize);
		return srv;
	}

	//	获取GPU SRV
	CD3DX12_GPU_DESCRIPTOR_HANDLE DefaultSceneRender::GetGpuSrv(int index, ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap, UINT mCbvSrvUavDescriptorSize)const
	{
		auto srv = CD3DX12_GPU_DESCRIPTOR_HANDLE(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		srv.Offset(index, mCbvSrvUavDescriptorSize);
		return srv;
	}

	//	获取DSV
	CD3DX12_CPU_DESCRIPTOR_HANDLE DefaultSceneRender::GetDsv(int index, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap, UINT mDsvDescriptorSize)const
	{
		auto dsv = CD3DX12_CPU_DESCRIPTOR_HANDLE(mDsvHeap->GetCPUDescriptorHandleForHeapStart());
		dsv.Offset(index, mDsvDescriptorSize);
		return dsv;
	}

	//	获取RTV
	CD3DX12_CPU_DESCRIPTOR_HANDLE DefaultSceneRender::GetRtv(int index, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap, UINT mRtvDescriptorSize)const
	{
		auto rtv = CD3DX12_CPU_DESCRIPTOR_HANDLE(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
		rtv.Offset(index, mRtvDescriptorSize);
		return rtv;
	}

	//	构建描述符
	void DefaultSceneRender::BuildDescriptorHeaps(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList, Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer, int SwapChainBufferCount, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap, UINT mDsvDescriptorSize, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap, UINT mRtvDescriptorSize, UINT mCbvSrvUavDescriptorSize)
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
		std::vector<ComPtr<ID3D12Resource>> RenderTex2DList = LoadTextureResources(DefaultScene::TexturesType::RenderItem);
		std::vector<ComPtr<ID3D12Resource>> GizmoTex2DList = LoadTextureResources(DefaultScene::TexturesType::Gizom);

		//	Environment Tex / CubeMap
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

		auto nullSrv = GetCpuSrv(mNullCubeSrvIndex, mSrvDescriptorHeap, mCbvSrvUavDescriptorSize);
		mNullSrv = GetGpuSrv(mNullCubeSrvIndex, mSrvDescriptorHeap, mCbvSrvUavDescriptorSize);

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
			GetCpuSrv(mShadowMapHeapIndex, mSrvDescriptorHeap, mCbvSrvUavDescriptorSize),
			GetGpuSrv(mShadowMapHeapIndex, mSrvDescriptorHeap, mCbvSrvUavDescriptorSize),
			GetDsv(1, mDsvHeap, mDsvDescriptorSize));

		mSsao->BuildDescriptors(
			mDepthStencilBuffer.Get(),
			GetCpuSrv(mSsaoHeapIndexStart, mSrvDescriptorHeap, mCbvSrvUavDescriptorSize),
			GetGpuSrv(mSsaoHeapIndexStart, mSrvDescriptorHeap, mCbvSrvUavDescriptorSize),
			GetRtv(SwapChainBufferCount, mRtvHeap, mRtvDescriptorSize),
			mCbvSrvUavDescriptorSize,
			mRtvDescriptorSize);

		mTarget->BuildDescriptors(
			GetCpuSrv(mRenderTargetIndex, mSrvDescriptorHeap,mCbvSrvUavDescriptorSize),
			GetGpuSrv(mRenderTargetIndex, mSrvDescriptorHeap,mCbvSrvUavDescriptorSize),
			GetRtv(SwapChainBufferCount, mRtvHeap, mRtvDescriptorSize));
	}

	//	构建帧资源
	void DefaultSceneRender::BuildFrameResources(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice)
	{ 
		for (int i = 0; i < gNumFrameResources; ++i)
		{
				mFrameResources.push_back(std::make_unique<DefaultFrameResource>(md3dDevice.Get(),
				2, (UINT)mAllRitems.size(),
				1,
				(UINT)mMeshRender.size()));
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

	//	构建PSO
	void DefaultSceneRender::BuildPSO(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, DXGI_FORMAT mBackBufferFormat, bool m4xMsaaState, UINT m4xMsaaQuality, DXGI_FORMAT mDepthStencilFormat)
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;

		// PSO for opaque objects.
		{
			ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
			opaquePsoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
			opaquePsoDesc.pRootSignature = mRootSignature.Get();
			opaquePsoDesc.VS =
			{
				reinterpret_cast<BYTE*>(mShaders["standardVS"]->GetBufferPointer()),
				mShaders["standardVS"]->GetBufferSize()
			};
			opaquePsoDesc.PS =
			{
				reinterpret_cast<BYTE*>(mShaders["opaquePS"]->GetBufferPointer()),
				mShaders["opaquePS"]->GetBufferSize()
			};
			opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			opaquePsoDesc.SampleMask = UINT_MAX;
			opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			opaquePsoDesc.NumRenderTargets = 1;
			opaquePsoDesc.RTVFormats[0] = mBackBufferFormat;
			opaquePsoDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
			opaquePsoDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
			opaquePsoDesc.DSVFormat = mDepthStencilFormat;
			ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&mPSOs["opaque"])));
		}


		// 	PSO for editor gizmo
		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC GizmoPsoDesc = opaquePsoDesc;
			GizmoPsoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
			GizmoPsoDesc.pRootSignature = mRootSignature.Get();
			GizmoPsoDesc.VS =
			{
				reinterpret_cast<BYTE*>(mShaders["gizmoVS"]->GetBufferPointer()),
				mShaders["gizmoVS"]->GetBufferSize()
			};
			GizmoPsoDesc.PS =
			{
				reinterpret_cast<BYTE*>(mShaders["gizmoPS"]->GetBufferPointer()),
				mShaders["gizmoPS"]->GetBufferSize()
			};

			D3D12_RENDER_TARGET_BLEND_DESC transparencyBlendDesc;
			transparencyBlendDesc.BlendEnable = true;
			transparencyBlendDesc.LogicOpEnable = false;
			transparencyBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			transparencyBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			transparencyBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
			transparencyBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
			transparencyBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
			transparencyBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
			transparencyBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
			transparencyBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
			GizmoPsoDesc.BlendState.RenderTarget[0] = transparencyBlendDesc;

			ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&GizmoPsoDesc, IID_PPV_ARGS(&mPSOs["Gizmo"])));
		}


		// PSO for pbr objects.
		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC litPsoDesc = opaquePsoDesc;
			litPsoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
			litPsoDesc.pRootSignature = mRootSignature.Get();
			litPsoDesc.VS =
			{
				reinterpret_cast<BYTE*>(mShaders["litVS"]->GetBufferPointer()),
				mShaders["litVS"]->GetBufferSize()
			};
			litPsoDesc.PS =
			{
				reinterpret_cast<BYTE*>(mShaders["litPS"]->GetBufferPointer()),
				mShaders["litPS"]->GetBufferSize()
			};
			litPsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			litPsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			litPsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			litPsoDesc.SampleMask = UINT_MAX;
			litPsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			litPsoDesc.NumRenderTargets = 1;
			litPsoDesc.RTVFormats[0] = mBackBufferFormat;
			litPsoDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
			litPsoDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
			litPsoDesc.DSVFormat = mDepthStencilFormat;
			ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&litPsoDesc, IID_PPV_ARGS(&mPSOs["litOpaque"])));
		}

		// PSO for shadow map pass.
		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC smapPsoDesc = opaquePsoDesc;
			smapPsoDesc.RasterizerState.DepthBias = 100000;
			smapPsoDesc.RasterizerState.DepthBiasClamp = 0.0f;
			smapPsoDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
			smapPsoDesc.pRootSignature = mRootSignature.Get();
			smapPsoDesc.VS =
			{
				reinterpret_cast<BYTE*>(mShaders["shadowVS"]->GetBufferPointer()),
				mShaders["shadowVS"]->GetBufferSize()
			};
			smapPsoDesc.PS =
			{
				reinterpret_cast<BYTE*>(mShaders["shadowOpaquePS"]->GetBufferPointer()),
				mShaders["shadowOpaquePS"]->GetBufferSize()
			};

			// Shadow map pass does not have a render target.
			smapPsoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
			smapPsoDesc.NumRenderTargets = 0;
			ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&smapPsoDesc, IID_PPV_ARGS(&mPSOs["shadow_opaque"])));
		}


		// PSO for drawing normals.
		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC drawNormalsPsoDesc = opaquePsoDesc;
			drawNormalsPsoDesc.VS =
			{
				reinterpret_cast<BYTE*>(mShaders["drawNormalsVS"]->GetBufferPointer()),
				mShaders["drawNormalsVS"]->GetBufferSize()
			};
			drawNormalsPsoDesc.PS =
			{
				reinterpret_cast<BYTE*>(mShaders["drawNormalsPS"]->GetBufferPointer()),
				mShaders["drawNormalsPS"]->GetBufferSize()
			};
			drawNormalsPsoDesc.RTVFormats[0] = SsaoPass::NormalMapFormat;
			drawNormalsPsoDesc.SampleDesc.Count = 1;
			drawNormalsPsoDesc.SampleDesc.Quality = 0;
			drawNormalsPsoDesc.DSVFormat = mDepthStencilFormat;
			ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&drawNormalsPsoDesc, IID_PPV_ARGS(&mPSOs["drawNormals"])));
		}

		// PSO for SSAO.
		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC ssaoPsoDesc = opaquePsoDesc;
			ssaoPsoDesc.InputLayout = { nullptr, 0 };
			ssaoPsoDesc.pRootSignature = mSsaoRootSignature.Get();
			ssaoPsoDesc.VS =
			{
				reinterpret_cast<BYTE*>(mShaders["ssaoVS"]->GetBufferPointer()),
				mShaders["ssaoVS"]->GetBufferSize()
			};
			ssaoPsoDesc.PS =
			{
				reinterpret_cast<BYTE*>(mShaders["ssaoPS"]->GetBufferPointer()),
				mShaders["ssaoPS"]->GetBufferSize()
			};

			// SSAO effect does not need the depth buffer.
			ssaoPsoDesc.DepthStencilState.DepthEnable = false;
			ssaoPsoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
			ssaoPsoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			ssaoPsoDesc.SampleDesc.Count = 1;
			ssaoPsoDesc.SampleDesc.Quality = 0;
			ssaoPsoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
			ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&ssaoPsoDesc, IID_PPV_ARGS(&mPSOs["ssao"])));

			//
			// PSO for SSAO blur.
			//
			D3D12_GRAPHICS_PIPELINE_STATE_DESC ssaoBlurPsoDesc = ssaoPsoDesc;
			ssaoBlurPsoDesc.VS =
			{
				reinterpret_cast<BYTE*>(mShaders["ssaoBlurVS"]->GetBufferPointer()),
				mShaders["ssaoBlurVS"]->GetBufferSize()
			};
			ssaoBlurPsoDesc.PS =
			{
				reinterpret_cast<BYTE*>(mShaders["ssaoBlurPS"]->GetBufferPointer()),
				mShaders["ssaoBlurPS"]->GetBufferSize()
			};
			ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&ssaoBlurPsoDesc, IID_PPV_ARGS(&mPSOs["ssaoBlur"])));

			mSsao->SetPSOs(mPSOs["ssao"].Get(), mPSOs["ssaoBlur"].Get());
		}


		// PSO for skybox.
		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC skyPsoDesc = opaquePsoDesc;

			// The camera is inside the sky sphere, so just turn off culling.
			skyPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

			// Make sure the depth function is LESS_EQUAL and not just LESS.  
			// Otherwise, the normalized depth values at z = 1 (NDC) will 
			// fail the depth test if the depth buffer was cleared to 1.
			skyPsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
			skyPsoDesc.pRootSignature = mRootSignature.Get();
			skyPsoDesc.VS =
			{
				reinterpret_cast<BYTE*>(mShaders["skyVS"]->GetBufferPointer()),
				mShaders["skyVS"]->GetBufferSize()
			};
			skyPsoDesc.PS =
			{
				reinterpret_cast<BYTE*>(mShaders["skyPS"]->GetBufferPointer()),
				mShaders["skyPS"]->GetBufferSize()
			};
			ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&skyPsoDesc, IID_PPV_ARGS(&mPSOs["sky"])));
		}
	}



	//	更新阴影位置
	void DefaultSceneRender::UpdateShadowTransform(const GameTimer& gt)
	{
		// Only the first "main" light casts a shadow.
		XMVECTOR lightDir = mMainLight.forward.ToSIMD();
		XMVECTOR lightPos = -2.0f * mSceneBounds.Radius * lightDir;
		XMVECTOR targetPos = XMLoadFloat3(&mSceneBounds.Center);
		XMVECTOR lightUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		XMMATRIX lightView = XMMatrixLookAtLH(lightPos, targetPos, lightUp);

		XMStoreFloat3(&mLightPosW, lightPos);

		// Transform bounding sphere to light space.
		XMFLOAT3 sphereCenterLS;
		XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, lightView));

		// Ortho frustum in light space encloses scene.  
		float l = sphereCenterLS.x - mSceneBounds.Radius;
		float b = sphereCenterLS.y - mSceneBounds.Radius;
		float n = sphereCenterLS.z - mSceneBounds.Radius;
		float r = sphereCenterLS.x + mSceneBounds.Radius;
		float t = sphereCenterLS.y + mSceneBounds.Radius;
		float f = sphereCenterLS.z + mSceneBounds.Radius;

		mLightNearZ = n;
		mLightFarZ = f;
		XMMATRIX lightProj = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

		// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
		XMMATRIX T(
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, -0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.0f, 1.0f);

		XMMATRIX S = lightView * lightProj * T;
		XMStoreFloat4x4(&mLightView, lightView);
		XMStoreFloat4x4(&mLightProj, lightProj);
		XMStoreFloat4x4(&mShadowTransform, S);
	}

	//	更新Pass常量Buffer
	void DefaultSceneRender::UpdateMainPassCB(const GameTimer& gt, int mClientWidth, int mClientHeight)
	{
		XMMATRIX view = mCamera.GetView();
		XMMATRIX proj = mCamera.GetProj();

		XMMATRIX viewProj = XMMatrixMultiply(view, proj);
		XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
		XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
		XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

		// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
		XMMATRIX T(
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, -0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.0f, 1.0f);

		XMMATRIX viewProjTex = XMMatrixMultiply(viewProj, T);
		XMMATRIX shadowTransform = XMLoadFloat4x4(&mShadowTransform);

		XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
		XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
		XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
		XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
		XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
		XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
		XMStoreFloat4x4(&mMainPassCB.ViewProjTex, XMMatrixTranspose(viewProjTex));
		XMStoreFloat4x4(&mMainPassCB.ShadowTransform, XMMatrixTranspose(shadowTransform));
		mMainPassCB.EyePosW = mCamera.position;
		mMainPassCB.RenderTargetSize = Vector2((float)mClientWidth, (float)mClientHeight);
		mMainPassCB.InvRenderTargetSize = Vector2(1.0f / mClientWidth, 1.0f / mClientHeight);
		mMainPassCB.NearZ = 1.0f;
		mMainPassCB.FarZ = 1000.0f;
		mMainPassCB.TotalTime = gt.TotalTime();
		mMainPassCB.DeltaTime = gt.DeltaTime();

		//Light

		mMainPassCB.directionLight.forward = mMainLight.forward;
		mMainPassCB.directionLight.intensity = mMainLight.intensity;
		mMainPassCB.directionLight.color = mMainLight.color;
		mMainPassCB.directionLight.position = mMainLight.position;
		mMainPassCB.directionLight.Enable = mMainLight.Enable;

		auto currPassCB = mCurrFrameResource->PassCB.get();
		currPassCB->CopyData(0, mMainPassCB);
	}

	//	更新阴影常量Buffer
	void DefaultSceneRender::UpdateShadowPassCB(const GameTimer& gt)
	{
		XMMATRIX view = XMLoadFloat4x4(&mLightView);
		XMMATRIX proj = XMLoadFloat4x4(&mLightProj);

		XMMATRIX viewProj = XMMatrixMultiply(view, proj);
		XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
		XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
		XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

		UINT w = mShadowMapPass->Width();
		UINT h = mShadowMapPass->Height();

		XMStoreFloat4x4(&mShadowPassCB.View, XMMatrixTranspose(view));
		XMStoreFloat4x4(&mShadowPassCB.InvView, XMMatrixTranspose(invView));
		XMStoreFloat4x4(&mShadowPassCB.Proj, XMMatrixTranspose(proj));
		XMStoreFloat4x4(&mShadowPassCB.InvProj, XMMatrixTranspose(invProj));
		XMStoreFloat4x4(&mShadowPassCB.ViewProj, XMMatrixTranspose(viewProj));
		XMStoreFloat4x4(&mShadowPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
		mShadowPassCB.EyePosW = Vector3(mLightPosW);
		mShadowPassCB.RenderTargetSize = Vector2((float)w, (float)h);
		mShadowPassCB.InvRenderTargetSize = Vector2(1.0f / w, 1.0f / h);    
		mShadowPassCB.NearZ = mLightNearZ;
		mShadowPassCB.FarZ = mLightFarZ;

		auto currPassCB = mCurrFrameResource->PassCB.get();
		currPassCB->CopyData(1, mShadowPassCB);
	}

	//	更新SSAO常量Buffer
	void DefaultSceneRender::UpdateSsaoCB(const GameTimer& gt)
	{
		SsaoConstants ssaoCB;

		XMMATRIX P = mCamera.GetProj();

		// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
		XMMATRIX T(
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, -0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.0f, 1.0f);

		ssaoCB.Proj = mMainPassCB.Proj;
		ssaoCB.InvProj = mMainPassCB.InvProj;
		XMStoreFloat4x4(&ssaoCB.ProjTex, XMMatrixTranspose(P * T));

		mSsao->GetOffsetVectors(ssaoCB.OffsetVectors);

		auto blurWeights = mSsao->CalcGaussWeights(2.5f);
		ssaoCB.BlurWeights[0] = XMFLOAT4(&blurWeights[0]);
		ssaoCB.BlurWeights[1] = XMFLOAT4(&blurWeights[4]);
		ssaoCB.BlurWeights[2] = XMFLOAT4(&blurWeights[8]);

		ssaoCB.InvRenderTargetSize = XMFLOAT2(1.0f / mSsao->SsaoMapWidth(), 1.0f / mSsao->SsaoMapHeight());

		// Coordinates given in view space.
		ssaoCB.OcclusionRadius = 0.5f;
		ssaoCB.OcclusionFadeStart = 0.2f;
		ssaoCB.OcclusionFadeEnd = 2.0f;
		ssaoCB.SurfaceEpsilon = 0.05f;

		auto currSsaoCB = mCurrFrameResource->SsaoCB.get();
		currSsaoCB->CopyData(0, ssaoCB);
	}
}