#include "UploadBuffer.h"
#include "GeometryGenerator.h"
#include "GraphicsCore.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

GraphicsCore::GraphicsCore(HINSTANCE hInstance) : InputSystem(hInstance)
{
}

GraphicsCore::~GraphicsCore()
{
	if (md3dDevice != nullptr)
		FlushCommandQueue();
}

bool GraphicsCore::Initialize()
{
	if (!D3DApp::Initialize())
		return false;

	// Reset the command list to prep for initialization commands.
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	mCamera.SetPosition(0.0f, 2.0f, -15.0f);

	//ShadowMap Init
	mShadowMap = std::make_unique<ShadowMap>(md3dDevice.Get(), 2048, 2048);

	//SSAO Init
	mSsao = std::make_unique<Ssao>(md3dDevice.Get(), mCommandList.Get(), mClientWidth, mClientHeight);

	//RenderTarget Init
	mRenderTarget = std::make_unique<RenderTarget>(md3dDevice.Get(), mClientWidth, mClientHeight);

	BuildRootSignature();
	BuildDescriptorHeaps();
	BuildShadersAndInputLayout();
	mSceneManager.BuildScene(mResourcesTextures);

	BuildRenderItems();
	BuildFrameResources();
	BuildPSOs();

	mSsao->SetPSOs(mPSOs["ssao"].Get(), mPSOs["ssaoBlur"].Get());

	// Execute the initialization commands.
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until initialization is complete.
	FlushCommandQueue();

	return true;
}

void GraphicsCore::CreateRtvAndDsvDescriptorHeaps()
{
	// Add +1 for screen normal map, +2 for ambient maps.
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount + 3;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));

	// Add +1 DSV for shadow map.
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 2;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));

	D3D12_DESCRIPTOR_HEAP_DESC SrvHeapDesc;
	SrvHeapDesc.NumDescriptors = 1;
	SrvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	SrvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	SrvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&SrvHeapDesc, IID_PPV_ARGS(mSrvHeap.GetAddressOf())));
}

void GraphicsCore::OnResize()
{
	D3DApp::OnResize();

	mCamera.SetLens((mCamFov / 180) * Mathf::Pi, AspectRatio(), mCamClipN, mCamClipF);

	if (mSsao != nullptr)
	{
		mSsao->OnResize(mClientWidth, mClientHeight);

		// Resources changed, so need to rebuild descriptors.
		mSsao->RebuildDescriptors(mDepthStencilBuffer.Get());
	}
}

void GraphicsCore::Update(const GameTimer& gt)
{
	OnKeyboardInput(gt);
	mCamera.SetLens((mCamFov / 180) * Mathf::Pi, AspectRatio(), mCamClipN, mCamClipF);

	// Cycle through the circular frame resource array.
	mCurrFrameResourceIndex = (mCurrFrameResourceIndex + 1) % gNumFrameResources;
	mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();

	// Has the GPU finished processing the commands of the current frame resource?
	// If not, wait until the GPU has completed commands up to this fence point.
	if (mCurrFrameResource->Fence != 0 && mFence->GetCompletedValue() < mCurrFrameResource->Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(mFence->SetEventOnCompletion(mCurrFrameResource->Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	UpdateLights(gt);
	UpdateObjectCBs(gt);
	UpdateMaterialBuffer(gt);
	UpdateShadowTransform(gt);
	UpdateMainPassCB(gt);
	UpdateShadowPassCB(gt);
	UpdateSsaoCB(gt);
}


void GraphicsCore::UpdateLights(const GameTimer& gt)
{
	mSceneManager.mMainLight.Tick();
}

void GraphicsCore::UpdateObjectCBs(const GameTimer& gt)
{
	auto currObjectCB = mCurrFrameResource->ObjectCB.get();
	for (auto& e : mAllRitems)
	{
		XMFLOAT4X4* eWorldMatrix = &e->World;

		XMMATRIX world = XMLoadFloat4x4(eWorldMatrix);
		XMMATRIX InvWorld = XMMatrixInverse(&XMMatrixDeterminant(world), world);
		XMMATRIX texTransform = XMLoadFloat4x4(&e->TexTransform);

		ObjectConstants objConstants;
		XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
		XMStoreFloat4x4(&objConstants.InvWorld, XMMatrixTranspose(InvWorld));
		XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));
		objConstants.MaterialIndex = e->Mat->MatCBIndex;
		
		currObjectCB->CopyData(e->ObjCBIndex, objConstants);
	}
}

void GraphicsCore::UpdateMaterialBuffer(const GameTimer& gt)
{
	auto PBRMaterialBuffer = mCurrFrameResource->PBRMaterialBuffer.get();
	auto SkyBoxMaterialBuffer = mCurrFrameResource->SkyBoxMaterialBuffer.get();
	//	更惨材质Buffer
	mSceneManager.UpdateSceneMaterialBuffer(PBRMaterialBuffer, SkyBoxMaterialBuffer);
}

   

void GraphicsCore::UpdateShadowTransform(const GameTimer& gt)
{
	// Only the first "main" light casts a shadow.
	XMVECTOR lightDir = mSceneManager.mMainLight.forward.ToSIMD();
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

void GraphicsCore::UpdateMainPassCB(const GameTimer& gt)
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

	mMainPassCB.DirectionLights.forward = mSceneManager.mMainLight.forward;
	mMainPassCB.DirectionLights.intensity = mSceneManager.mMainLight.intensity;
	mMainPassCB.DirectionLights.color = mSceneManager.mMainLight.color;
	mMainPassCB.DirectionLights.isMainLight = true;
	mMainPassCB.DirectionLights.position = mSceneManager.mMainLight.position;
	mMainPassCB.DirectionLights.Enable = mSceneManager.mMainLight.Enable;

	auto currPassCB = mCurrFrameResource->PassCB.get();
	currPassCB->CopyData(0, mMainPassCB);
}

void GraphicsCore::UpdateShadowPassCB(const GameTimer& gt)
{
	XMMATRIX view = XMLoadFloat4x4(&mLightView);
	XMMATRIX proj = XMLoadFloat4x4(&mLightProj);

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	UINT w = mShadowMap->Width();
	UINT h = mShadowMap->Height();

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

void GraphicsCore::UpdateSsaoCB(const GameTimer& gt)
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

void GraphicsCore::BuildRootSignature()
{
	mRootSignature.Build(RootSignature::RootSignatureType::Default, md3dDevice, 5, 0, 0, 60, 5, 0);
	mSsaoRootSignature.Build(RootSignature::RootSignatureType::SSAO, md3dDevice, 2, 0, 0, 1, 2, 0);
}

//swap chain  BuildDescriptorHeaps
void GraphicsCore::BuildDescriptorHeaps()
{
	mSrvDescriptorHeap.Build(md3dDevice, 128);

	CD3DX12_CPU_DESCRIPTOR_HANDLE CPUDescriptor(mSrvDescriptorHeap.GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
	CD3DX12_GPU_DESCRIPTOR_HANDLE GPUDescriptor(mSrvDescriptorHeap.GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

	UINT mCbvSrvUavDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	UINT TextureIndex = 1;
	//Load Icon
	for (size_t i = 0; i < mProjectIconTextures.size(); i++)
	{
		int image_width = 0;
		int image_height = 0;

		ID3D12Resource* texture = NULL;

		// Load the texture from a file
		bool ret = ResourcesManager::LoadTextureFromFile(mProjectIconTextures[i].c_str(), md3dDevice, CPUDescriptor, &texture, &image_width, &image_height);
		CPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
		GPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);

		if (ret)
		{
			auto tex = std::make_unique<Texture2D>();
			tex->Width = image_width;
			tex->Height = image_height;
			tex->Bit = 32;
			tex->Size = (float)((image_width * image_height * 32) / 8388608);
			tex->type = TexturesType::Icon;
			tex->TexIndex = TextureIndex;
			TextureIndex++;
			tex->CpuHandle = CPUDescriptor;
			tex->GpuHandle = GPUDescriptor;
			tex->Name = mProjectIconName[i];
			tex->Path = mProjectIconTextures[i];
			mIconTextures[tex->Name] = std::move(tex);
		}
	}

	//Load Gizmo
	for (size_t i = 0; i < mProjectGizmoTextures.size(); i++)
	{
		int image_width = 0;
		int image_height = 0;

		ID3D12Resource* texture = NULL;

		CPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
		GPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);

		// Load the texture from a file
		bool ret = ResourcesManager::LoadTextureFromFile(mProjectGizmoTextures[i].c_str(), md3dDevice, CPUDescriptor, &texture, &image_width, &image_height);
		IM_ASSERT(ret);

		auto tex = std::make_unique<Texture2D>();
		tex->Width = image_width;
		tex->Height = image_height;
		tex->Bit = 32;
		tex->Size = (float)((image_width * image_height * 32) / 8388608);
		tex->type = TexturesType::Gizom;
		tex->TexIndex = TextureIndex;
		TextureIndex++;
		tex->CpuHandle = CPUDescriptor;
		tex->GpuHandle = GPUDescriptor;
		tex->Name = mProjectGizmoName[i];
		tex->Path = mProjectGizmoTextures[i];
		mGizmosTextures[tex->Name] = std::move(tex);
	}

	//Load Project
	for (size_t i = 0; i < mProjectResourceTextures.size(); i++)
	{
		int image_width = 0;
		int image_height = 0;

		ID3D12Resource* texture = NULL;

		CPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
		GPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);

		// Load the texture from a file
		bool ret = ResourcesManager::LoadTextureFromFile(mProjectResourceTextures[i].c_str(), md3dDevice, CPUDescriptor, &texture, &image_width, &image_height);
		IM_ASSERT(ret);

		auto tex = std::make_unique<Texture2D>();
		tex->Width = image_width;
		tex->Height = image_height;
		tex->Bit = 32;
		tex->Size = (float)((image_width * image_height * 32) / 8388608);
		tex->type = TexturesType::Project;
		tex->TexIndex = TextureIndex;
		TextureIndex++;
		tex->CpuHandle = CPUDescriptor;
		tex->GpuHandle = GPUDescriptor;
		tex->Name = mProjectResourceName[i];
		tex->Path = mProjectResourceTextures[i];
		mResourcesTextures[tex->Name] = std::move(tex);
	}

	//Load CubeMap
	for (size_t i = 0; i < mProjectCubeMapTextures.size(); i++)
	{
		if (mCubeMapTextures.find(mProjectCubeMapName[i]) == std::end(mCubeMapTextures))
		{
			auto tex = std::make_unique<TextureCube>();
			tex->Name = mProjectCubeMapName[i];
			tex->Path = mProjectCubeMapTextures[i];
			tex->CpuHandle = CPUDescriptor;
			tex->GpuHandle = GPUDescriptor;
			tex->type = TexturesType::CubeMap;
			tex->TexIndex = TextureIndex;
			TextureIndex++;
			ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
				mCommandList.Get(), tex->Path.c_str(),
				tex->Resource, tex->UploadHeap));

			mCubeMapTextures[tex->Name] = std::move(tex);
		}
	}

	GraphicsUtils::BuildTextureCubeSrvDesc(md3dDevice, mCbvSrvUavDescriptorSize, CPUDescriptor, GPUDescriptor, mCubeMapTextures, "DGarden_diffuseIBL");
	GraphicsUtils::BuildTextureCubeSrvDesc(md3dDevice, mCbvSrvUavDescriptorSize, CPUDescriptor, GPUDescriptor, mCubeMapTextures, "DGarden_specularIBL");

	//mShadowMap->BuildDescriptors(
	//	GetCpuSrv(mShadowMapHeapIndex, mSrvDescriptorHeap),
	//	GetGpuSrv(mShadowMapHeapIndex, mSrvDescriptorHeap),
	//	GetDsv(1));

	//mSsao->BuildDescriptors(
	//	mDepthStencilBuffer.Get(),
	//	GetCpuSrv(mSsaoHeapIndexStart, mSrvDescriptorHeap),
	//	GetGpuSrv(mSsaoHeapIndexStart, mSrvDescriptorHeap),
	//	GetRtv(SwapChainBufferCount),
	//	mCbvSrvUavDescriptorSize,
	//	mRtvDescriptorSize);

	//mRenderTarget->BuildDescriptors(
	//	GetCpuSrv(mRenderTargetIndex, mSrvDescriptorHeap),
	//	GetGpuSrv(mRenderTargetIndex, mSrvDescriptorHeap),
	//	GetRtv(SwapChainBufferCount));
}


void GraphicsCore::BuildShadersAndInputLayout()
{
	mShaderManager.Build();
}

void GraphicsCore::BuildPSOs()
{
	mPSOs.Add(md3dDevice, mShaderManager, mRootSignature, "standardVS", "opaquePS", "opaque", mBackBufferFormat, m4xMsaaState, m4xMsaaQuality,mDepthStencilFormat);

	//
	// 	PSO for Bounding
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC BoundingPsoDesc = opaquePsoDesc;
	BoundingPsoDesc.InputLayout = { mShaderManager.mInputLayout.data(), (UINT)mShaderManager.mInputLayout.size() };
	BoundingPsoDesc.pRootSignature = mRootSignature.GetSignature();
	BoundingPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["boundingVS"]->GetBufferPointer()),
		mShaderManager.mShaders["boundingVS"]->GetBufferSize()
	};
	BoundingPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["boundingPS"]->GetBufferPointer()),
		mShaderManager.mShaders["boundingPS"]->GetBufferSize()
	};
	BoundingPsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	BoundingPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&BoundingPsoDesc, IID_PPV_ARGS(&mPSOs["bounding"])));

	//
	// 	PSO for OutLine
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC outlinePsoDesc = opaquePsoDesc;
	outlinePsoDesc.InputLayout = { mShaderManager.mInputLayout.data(), (UINT)mShaderManager.mInputLayout.size() };
	outlinePsoDesc.pRootSignature = mRootSignature.GetSignature();
	outlinePsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["outlineVS"]->GetBufferPointer()),
		mShaderManager.mShaders["outlineVS"]->GetBufferSize()
	};
	outlinePsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["outlinePS"]->GetBufferPointer()),
		mShaderManager.mShaders["outlinePS"]->GetBufferSize()
	};
	outlinePsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
	outlinePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	outlinePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	outlinePsoDesc.SampleMask = UINT_MAX;
	outlinePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	outlinePsoDesc.NumRenderTargets = 1;
	outlinePsoDesc.RTVFormats[0] = mBackBufferFormat;
	outlinePsoDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	outlinePsoDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	outlinePsoDesc.DSVFormat = mDepthStencilFormat;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&outlinePsoDesc, IID_PPV_ARGS(&mPSOs["outline"])));

	//
	// 	PSO for EditorGizmo
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC GizmoPsoDesc = opaquePsoDesc;
	GizmoPsoDesc.InputLayout = { mShaderManager.mInputLayout.data(), (UINT)mShaderManager.mInputLayout.size() };
	GizmoPsoDesc.pRootSignature = mRootSignature.GetSignature();
	GizmoPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["gizmoVS"]->GetBufferPointer()),
		mShaderManager.mShaders["gizmoVS"]->GetBufferSize()
	};
	GizmoPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["gizmoPS"]->GetBufferPointer()),
		mShaderManager.mShaders["gizmoPS"]->GetBufferSize()
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

	//
	// 	PSO for Unlit
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC UnlitPsoDesc = GizmoPsoDesc;
	UnlitPsoDesc.InputLayout = { mShaderManager.mInputLayout.data(), (UINT)mShaderManager.mInputLayout.size() };
	UnlitPsoDesc.pRootSignature = mRootSignature.GetSignature();
	UnlitPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["unitVS"]->GetBufferPointer()),
		mShaderManager.mShaders["unitVS"]->GetBufferSize()
	};
	UnlitPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["unitPS"]->GetBufferPointer()),
		mShaderManager.mShaders["unitPS"]->GetBufferSize()
	};
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&UnlitPsoDesc, IID_PPV_ARGS(&mPSOs["Unlit"])));

	//
	// PSO for PBR objects.
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC litPsoDesc = opaquePsoDesc;
	litPsoDesc.InputLayout = { mShaderManager.mInputLayout.data(), (UINT)mShaderManager.mInputLayout.size() };
	litPsoDesc.pRootSignature = mRootSignature.GetSignature();
	litPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["litVS"]->GetBufferPointer()),
		mShaderManager.mShaders["litVS"]->GetBufferSize()
	};
	litPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["litPS"]->GetBufferPointer()),
		mShaderManager.mShaders["litPS"]->GetBufferSize()
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


	//
	// 	PSO for PBR objects (Transparent).
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC lit_trans_PsoDesc = litPsoDesc;
	lit_trans_PsoDesc.BlendState.RenderTarget[0] = transparencyBlendDesc;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&lit_trans_PsoDesc, IID_PPV_ARGS(&mPSOs["transparent"])));

	//
	// PSO for shadow map pass.
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC smapPsoDesc = opaquePsoDesc;
	smapPsoDesc.RasterizerState.DepthBias = 100000;
	smapPsoDesc.RasterizerState.DepthBiasClamp = 0.0f;
	smapPsoDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
	smapPsoDesc.pRootSignature = mRootSignature.GetSignature();
	smapPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["shadowVS"]->GetBufferPointer()),
		mShaderManager.mShaders["shadowVS"]->GetBufferSize()
	};
	smapPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["shadowOpaquePS"]->GetBufferPointer()),
		mShaderManager.mShaders["shadowOpaquePS"]->GetBufferSize()
	};

	// Shadow map pass does not have a render target.
	smapPsoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
	smapPsoDesc.NumRenderTargets = 0;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&smapPsoDesc, IID_PPV_ARGS(&mPSOs["shadow_opaque"])));


	//
	// PSO for debug layer.
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC debugPsoDesc = opaquePsoDesc;
	debugPsoDesc.pRootSignature = mRootSignature.GetSignature();
	debugPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["debugVS"]->GetBufferPointer()),
		mShaderManager.mShaders["debugVS"]->GetBufferSize()
	};
	debugPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["debugPS"]->GetBufferPointer()),
		mShaderManager.mShaders["debugPS"]->GetBufferSize()
	};
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&debugPsoDesc, IID_PPV_ARGS(&mPSOs["debug"])));

	//
	// PSO for drawing normals.
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC drawNormalsPsoDesc = opaquePsoDesc;
	drawNormalsPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["drawNormalsVS"]->GetBufferPointer()),
		mShaderManager.mShaders["drawNormalsVS"]->GetBufferSize()
	};
	drawNormalsPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["drawNormalsPS"]->GetBufferPointer()),
		mShaderManager.mShaders["drawNormalsPS"]->GetBufferSize()
	};
	drawNormalsPsoDesc.RTVFormats[0] = Ssao::NormalMapFormat;
	drawNormalsPsoDesc.SampleDesc.Count = 1;
	drawNormalsPsoDesc.SampleDesc.Quality = 0;
	drawNormalsPsoDesc.DSVFormat = mDepthStencilFormat;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&drawNormalsPsoDesc, IID_PPV_ARGS(&mPSOs["drawNormals"])));


	//Postprocessing PSO

	//
	// PSO for SSAO.
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC ssaoPsoDesc = opaquePsoDesc;
	ssaoPsoDesc.InputLayout = { nullptr, 0 };
	ssaoPsoDesc.pRootSignature = mSsaoRootSignature.GetSignature();
	ssaoPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["ssaoVS"]->GetBufferPointer()),
		mShaderManager.mShaders["ssaoVS"]->GetBufferSize()
	};
	ssaoPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["ssaoPS"]->GetBufferPointer()),
		mShaderManager.mShaders["ssaoPS"]->GetBufferSize()
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
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["ssaoBlurVS"]->GetBufferPointer()),
		mShaderManager.mShaders["ssaoBlurVS"]->GetBufferSize()
	};
	ssaoBlurPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["ssaoBlurPS"]->GetBufferPointer()),
		mShaderManager.mShaders["ssaoBlurPS"]->GetBufferSize()
	};
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&ssaoBlurPsoDesc, IID_PPV_ARGS(&mPSOs["ssaoBlur"])));

	//
	// 	PSO for FinalBlit
	//
		D3D12_GRAPHICS_PIPELINE_STATE_DESC FinalBlitPsoDesc = opaquePsoDesc;
		FinalBlitPsoDesc.InputLayout = { mShaderManager.mInputLayout.data(), (UINT)mShaderManager.mInputLayout.size() };
		FinalBlitPsoDesc.pRootSignature = mRootSignature.GetSignature();
		FinalBlitPsoDesc.VS =
		{
			reinterpret_cast<BYTE*>(mShaderManager.mShaders["finalBlitVS"]->GetBufferPointer()),
			mShaderManager.mShaders["finalBlitVS"]->GetBufferSize()
		};
		FinalBlitPsoDesc.PS =
		{
			reinterpret_cast<BYTE*>(mShaderManager.mShaders["finalBlitPS"]->GetBufferPointer()),
			mShaderManager.mShaders["finalBlitPS"]->GetBufferSize()
		};
		ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&FinalBlitPsoDesc, IID_PPV_ARGS(&mPSOs["FinalBlit"])));

	//
	// 	PSO for CopyColor
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC CopyColorPsoDesc = opaquePsoDesc;
	CopyColorPsoDesc.InputLayout = { mShaderManager.mInputLayout.data(), (UINT)mShaderManager.mInputLayout.size() };
	CopyColorPsoDesc.pRootSignature = mRootSignature.GetSignature();
	CopyColorPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["copyColorVS"]->GetBufferPointer()),
		mShaderManager.mShaders["copyColorVS"]->GetBufferSize()
	};
	CopyColorPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["copyColorPS"]->GetBufferPointer()),
		mShaderManager.mShaders["copyColorPS"]->GetBufferSize()
	};
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&CopyColorPsoDesc, IID_PPV_ARGS(&mPSOs["CopyColor"])));

	//
	// 	PSO for RGBSplit
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC RGBSplitPsoDesc = opaquePsoDesc;
	RGBSplitPsoDesc.InputLayout = { mShaderManager.mInputLayout.data(), (UINT)mShaderManager.mInputLayout.size() };
	RGBSplitPsoDesc.pRootSignature = mRootSignature.GetSignature();
	RGBSplitPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["rgbSplitVS"]->GetBufferPointer()),
		mShaderManager.mShaders["rgbSplitVS"]->GetBufferSize()
	};
	RGBSplitPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["rgbSplitPS"]->GetBufferPointer()),
		mShaderManager.mShaders["rgbSplitPS"]->GetBufferSize()
	};
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&RGBSplitPsoDesc, IID_PPV_ARGS(&mPSOs["RGBSplit"])));


	//
	// 	PSO for RadialBlur
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC RadialBlurPsoDesc = opaquePsoDesc;
	RadialBlurPsoDesc.InputLayout = { mShaderManager.mInputLayout.data(), (UINT)mShaderManager.mInputLayout.size() };
	RadialBlurPsoDesc.pRootSignature = mRootSignature.GetSignature();
	RadialBlurPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["radialBlurVS"]->GetBufferPointer()),
		mShaderManager.mShaders["radialBlurVS"]->GetBufferSize()
	};
	RadialBlurPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["radialBlurPS"]->GetBufferPointer()),
		mShaderManager.mShaders["radialBlurPS"]->GetBufferSize()
	};
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&RadialBlurPsoDesc, IID_PPV_ARGS(&mPSOs["RadialBlur"])));


	//
	// 	PSO for Vignette
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC vignettePsoDesc = opaquePsoDesc;
	vignettePsoDesc.InputLayout = { mShaderManager.mInputLayout.data(), (UINT)mShaderManager.mInputLayout.size() };
	vignettePsoDesc.pRootSignature = mRootSignature.GetSignature();
	vignettePsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["vignetteVS"]->GetBufferPointer()),
		mShaderManager.mShaders["vignetteVS"]->GetBufferSize()
	};
	vignettePsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["vignettePS"]->GetBufferPointer()),
		mShaderManager.mShaders["vignettePS"]->GetBufferSize()
	};
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&vignettePsoDesc, IID_PPV_ARGS(&mPSOs["Vignette"])));


	//
	// 	PSO for Decolor
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC decolorPsoDesc = opaquePsoDesc;
	decolorPsoDesc.InputLayout = { mShaderManager.mInputLayout.data(), (UINT)mShaderManager.mInputLayout.size() };
	decolorPsoDesc.pRootSignature = mRootSignature.GetSignature();
	decolorPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["decolorVS"]->GetBufferPointer()),
		mShaderManager.mShaders["decolorVS"]->GetBufferSize()
	};
	decolorPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["decolorPS"]->GetBufferPointer()),
		mShaderManager.mShaders["decolorPS"]->GetBufferSize()
	};
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&decolorPsoDesc, IID_PPV_ARGS(&mPSOs["Decolor"])));

	//
	// 	PSO for Brightness
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC brightnessPsoDesc = opaquePsoDesc;
	brightnessPsoDesc.InputLayout = { mShaderManager.mInputLayout.data(), (UINT)mShaderManager.mInputLayout.size() };
	brightnessPsoDesc.pRootSignature = mRootSignature.GetSignature();
	brightnessPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["brightnessVS"]->GetBufferPointer()),
		mShaderManager.mShaders["brightnessVS"]->GetBufferSize()
	};
	brightnessPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["brightnessPS"]->GetBufferPointer()),
		mShaderManager.mShaders["brightnessPS"]->GetBufferSize()
	};
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&brightnessPsoDesc, IID_PPV_ARGS(&mPSOs["Brightness"])));

	//
	// 	PSO for HSV
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC hsvPsoDesc = opaquePsoDesc;
	hsvPsoDesc.InputLayout = { mShaderManager.mInputLayout.data(), (UINT)mShaderManager.mInputLayout.size() };
	hsvPsoDesc.pRootSignature = mRootSignature.GetSignature();
	hsvPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["hsvVS"]->GetBufferPointer()),
		mShaderManager.mShaders["hsvVS"]->GetBufferSize()
	};
	hsvPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["hsvPS"]->GetBufferPointer()),
		mShaderManager.mShaders["hsvPS"]->GetBufferSize()
	};
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&hsvPsoDesc, IID_PPV_ARGS(&mPSOs["HSV"])));

	//
	// 	PSO for Mosaic
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC mosaicPsoDesc = opaquePsoDesc;
	mosaicPsoDesc.InputLayout = { mShaderManager.mInputLayout.data(), (UINT)mShaderManager.mInputLayout.size() };
	mosaicPsoDesc.pRootSignature = mRootSignature.GetSignature();
	mosaicPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["mosaicVS"]->GetBufferPointer()),
		mShaderManager.mShaders["mosaicVS"]->GetBufferSize()
	};
	mosaicPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["mosaicPS"]->GetBufferPointer()),
		mShaderManager.mShaders["mosaicPS"]->GetBufferSize()
	};
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&mosaicPsoDesc, IID_PPV_ARGS(&mPSOs["Mosaic"])));

	//
	// 	PSO for Sharpen
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC sharpenPsoDesc = opaquePsoDesc;
	sharpenPsoDesc.InputLayout = { mShaderManager.mInputLayout.data(), (UINT)mShaderManager.mInputLayout.size() };
	sharpenPsoDesc.pRootSignature = mRootSignature.GetSignature();
	sharpenPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["sharpenVS"]->GetBufferPointer()),
		mShaderManager.mShaders["sharpenVS"]->GetBufferSize()
	};
	sharpenPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["sharpenPS"]->GetBufferPointer()),
		mShaderManager.mShaders["sharpenPS"]->GetBufferSize()
	};
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&sharpenPsoDesc, IID_PPV_ARGS(&mPSOs["Sharpen"])));


	//
	// 	PSO for Spherize
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC spherizePsoDesc = opaquePsoDesc;
	spherizePsoDesc.InputLayout = { mShaderManager.mInputLayout.data(), (UINT)mShaderManager.mInputLayout.size() };
	spherizePsoDesc.pRootSignature = mRootSignature.GetSignature();
	spherizePsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["spherizeVS"]->GetBufferPointer()),
		mShaderManager.mShaders["spherizeVS"]->GetBufferSize()
	};
	spherizePsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["spherizePS"]->GetBufferPointer()),
		mShaderManager.mShaders["spherizePS"]->GetBufferSize()
	};
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&spherizePsoDesc, IID_PPV_ARGS(&mPSOs["Spherize"])));


	//
	// 	PSO for WhiteBalance
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC whiteBalancePsoDesc = opaquePsoDesc;
	whiteBalancePsoDesc.InputLayout = { mShaderManager.mInputLayout.data(), (UINT)mShaderManager.mInputLayout.size() };
	whiteBalancePsoDesc.pRootSignature = mRootSignature.GetSignature();
	whiteBalancePsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["whiteBalanceVS"]->GetBufferPointer()),
		mShaderManager.mShaders["whiteBalanceVS"]->GetBufferSize()
	};
	whiteBalancePsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["whiteBalancePS"]->GetBufferPointer()),
		mShaderManager.mShaders["whiteBalancePS"]->GetBufferSize()
	};
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&whiteBalancePsoDesc, IID_PPV_ARGS(&mPSOs["WhiteBalance"])));




	//
	// 	PSO for OilPaint
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC oilPaintPsoDesc = opaquePsoDesc;
	oilPaintPsoDesc.InputLayout = { mShaderManager.mInputLayout.data(), (UINT)mShaderManager.mInputLayout.size() };
	oilPaintPsoDesc.pRootSignature = mRootSignature.GetSignature();
	oilPaintPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["oilPaintVS"]->GetBufferPointer()),
		mShaderManager.mShaders["oilPaintVS"]->GetBufferSize()
	};
	oilPaintPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["oilPaintPS"]->GetBufferPointer()),
		mShaderManager.mShaders["oilPaintPS"]->GetBufferSize()
	};
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&oilPaintPsoDesc, IID_PPV_ARGS(&mPSOs["OilPaint"])));




	//
	// 	PSO for Relief
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC reliefPsoDesc = opaquePsoDesc;
	reliefPsoDesc.InputLayout = { mShaderManager.mInputLayout.data(), (UINT)mShaderManager.mInputLayout.size() };
	reliefPsoDesc.pRootSignature = mRootSignature.GetSignature();
	reliefPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["reliefVS"]->GetBufferPointer()),
		mShaderManager.mShaders["reliefVS"]->GetBufferSize()
	};
	reliefPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["reliefPS"]->GetBufferPointer()),
		mShaderManager.mShaders["reliefPS"]->GetBufferSize()
	};
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&reliefPsoDesc, IID_PPV_ARGS(&mPSOs["Relief"])));



	//
	// 	PSO for EdgeDetection
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC edgeDetectionPsoDesc = opaquePsoDesc;
	edgeDetectionPsoDesc.InputLayout = { mShaderManager.mInputLayout.data(), (UINT)mShaderManager.mInputLayout.size() };
	edgeDetectionPsoDesc.pRootSignature = mRootSignature.GetSignature();
	edgeDetectionPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["edgeDetectionVS"]->GetBufferPointer()),
		mShaderManager.mShaders["edgeDetectionVS"]->GetBufferSize()
	};
	edgeDetectionPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["edgeDetectionPS"]->GetBufferPointer()),
		mShaderManager.mShaders["edgeDetectionPS"]->GetBufferSize()
	};
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&edgeDetectionPsoDesc, IID_PPV_ARGS(&mPSOs["EdgeDetection"])));






	//
	// PSO for sky.
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC skyPsoDesc = opaquePsoDesc;

	// The camera is inside the sky sphere, so just turn off culling.
	skyPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	// Make sure the depth function is LESS_EQUAL and not just LESS.  
	// Otherwise, the normalized depth values at z = 1 (NDC) will 
	// fail the depth test if the depth buffer was cleared to 1.
	skyPsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	skyPsoDesc.pRootSignature = mRootSignature.GetSignature();
	skyPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["skyVS"]->GetBufferPointer()),
		mShaderManager.mShaders["skyVS"]->GetBufferSize()
	};
	skyPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaderManager.mShaders["skyPS"]->GetBufferPointer()),
		mShaderManager.mShaders["skyPS"]->GetBufferSize()
	};
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&skyPsoDesc, IID_PPV_ARGS(&mPSOs["sky"])));

}

void GraphicsCore::BuildFrameResources()
{
	for (int i = 0; i < gNumFrameResources; ++i)
	{
		mFrameResources.push_back(std::make_unique<FrameResource>(md3dDevice.Get(),
			2, (UINT)mAllRitems.size(),
			(UINT)(mSceneManager.mMeshRender.size())));
	}
}

void GraphicsCore::BuildRenderItems()
{
	mSceneManager.BuildRenderItem(mRitemLayer, mAllRitems, md3dDevice, mCommandList);
}

void GraphicsCore::DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems)
{
	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

	auto objectCB = mCurrFrameResource->ObjectCB->Resource();

	for (size_t i = 0; i < ritems.size(); ++i)
	{
		auto ri = ritems[i];

		cmdList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
		cmdList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
		cmdList->IASetPrimitiveTopology(ri->PrimitiveType);

		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ri->ObjCBIndex * objCBByteSize;

		cmdList->SetGraphicsRootConstantBufferView(0, objCBAddress);

		cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
	}
}

void GraphicsCore::DrawSceneToShadowMap()
{
	mCommandList->RSSetViewports(1, &mShadowMap->Viewport());
	mCommandList->RSSetScissorRects(1, &mShadowMap->ScissorRect());

	// Change to DEPTH_WRITE.
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mShadowMap->Resource(),
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	// Clear the back buffer and depth buffer.
	mCommandList->ClearDepthStencilView(mShadowMap->Dsv(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.
	mCommandList->OMSetRenderTargets(0, nullptr, false, &mShadowMap->Dsv());

	// Bind the pass constant buffer for the shadow map pass.
	UINT passCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(PassConstants));
	auto passCB = mCurrFrameResource->PassCB->Resource();
	D3D12_GPU_VIRTUAL_ADDRESS passCBAddress = passCB->GetGPUVirtualAddress() + 1 * passCBByteSize;
	mCommandList->SetGraphicsRootConstantBufferView(2, passCBAddress);

	mCommandList->SetPipelineState(mPSOs["shadow_opaque"].Get());
	DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::Opaque]);

	// Change back to GENERIC_READ so we can read the texture in a shader.
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mShadowMap->Resource(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ));
}

void GraphicsCore::DrawNormalsAndDepth()
{
	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	auto normalMap = mSsao->NormalMap();
	auto normalMapRtv = mSsao->NormalMapRtv();

	// Change to RENDER_TARGET.
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(normalMap,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// Clear the screen normal map and depth buffer.
	float clearValue[] = { 0.0f, 0.0f, 1.0f, 0.0f };
	mCommandList->ClearRenderTargetView(normalMapRtv, clearValue, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.
	mCommandList->OMSetRenderTargets(1, &normalMapRtv, true, &DepthStencilView());

	// Bind the constant buffer for this pass.
	auto passCB = mCurrFrameResource->PassCB->Resource();
	mCommandList->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());

	mCommandList->SetPipelineState(mPSOs["drawNormals"].Get());
	DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::Opaque]);

	// Change back to GENERIC_READ so we can read the texture in a shader.
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(normalMap,
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ));
}

CD3DX12_CPU_DESCRIPTOR_HANDLE GraphicsCore::GetDsv(int index)const
{
	auto dsv = CD3DX12_CPU_DESCRIPTOR_HANDLE(mDsvHeap->GetCPUDescriptorHandleForHeapStart());
	dsv.Offset(index, mDsvDescriptorSize);
	return dsv;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE GraphicsCore::GetRtv(int index)const
{
	auto rtv = CD3DX12_CPU_DESCRIPTOR_HANDLE(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
	rtv.Offset(index, mRtvDescriptorSize);
	return rtv;
}