#include "UploadBuffer.h"
#include "../Geometry/GeometryGenerator.h"
#include "GraphicsCore.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

using namespace LogSystem;

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
	if (!DX12GameApp::Initialize())
		return false;

	// Reset the command list to prep for initialization commands.
	_forwardPassCmdList->Reset();

	mCamera.getInstance().SetPosition(0.0f, 2.0f, -15.0f);

	//ShadowMap Init
	mShadowMap = std::make_unique<ShadowMap>(md3dDevice.Get(), 2048, 2048);

	//DepthPass Init
	mDepthPass = std::make_unique<DepthPass>(md3dDevice.Get());

	//RenderTarget Init
	mRenderTexture = std::make_unique<RenderTexture>(md3dDevice.Get(), mClientWidth, mClientHeight);

	//GBuffer Init
	//mGBufferPass = std::make_unique<GBuffer>(md3dDevice.Get(), mClientWidth, mClientHeight);

	BuildRootSignature();
	BuildDescriptorHeaps();
	GraphicsUtils::IMGUIInit(mhMainWnd, md3dDevice, mSrvDescriptorHeap.GetDescriptorHeap());
	BuildShadersAndInputLayout();

	matCBIndexUtils.getInstance().Init();
	mGizmoManager.getInstance().BuildScene(mGizmosTextures, matCBIndexUtils);
	mSceneManager.getInstance().BuildScene(mResourcesTextures, matCBIndexUtils);
	mPostProcessManager.getInstance().Build(matCBIndexUtils);

	BuildRenderItems();
	BuildFrameResources();
	BuildPSOs();

	// Execute the initialization commands.
	_forwardPassCmdList->Close();
	ID3D12CommandList* cmdsLists[] = { _forwardPassCmdList->GetInternal().Get() };
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
	DX12GameApp::OnResize();

	mCamera.getInstance().SetLens((mSceneManager.getInstance().mCameraSetting.mCamFov / 180) * Mathf::Pi, AspectRatio(), mSceneManager.getInstance().mCameraSetting.mCamClipN, mSceneManager.getInstance().mCameraSetting.mCamClipF);

	////	GBuffer OnResize
	//if (mGBufferPass != nullptr)
	//{
	//	mGBufferPass->OnResize(mClientWidth, mClientHeight);
	//	mGBufferPass->RebuildDescriptors();
	//}
}

void GraphicsCore::Update(const GameTimer& gt)
{
	OnKeyboardInput(gt);
	mCamera.getInstance().SetLens((mSceneManager.getInstance().mCameraSetting.mCamFov / 180) * Mathf::Pi, AspectRatio(), mSceneManager.getInstance().mCameraSetting.mCamClipN, mSceneManager.getInstance().mCameraSetting.mCamClipF);

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
	//OnTestMove(gt);
}


void GraphicsCore::UpdateLights(const GameTimer& gt)
{
	mSceneManager.getInstance().MainLight->GetComponent<Transform>(1)->Tick(gt);
}

void GraphicsCore::UpdateObjectCBs(const GameTimer& gt)
{
	mSceneManager.getInstance().UpdateObjectBuffer(mAllRitems, mGizmoManager.getInstance().mRenderObjects.size());


	XMMATRIX view = mCamera.getInstance().GetView();				//WorldToView的变换矩�?
	auto viewDeterminant = XMMatrixDeterminant(view);
	XMMATRIX invView = XMMatrixInverse(&viewDeterminant, view);		//ViewToWorld的变换矩�?

	auto currObjectCB = mCurrFrameResource->ObjectCB.get();

	for (auto& e : mAllRitems)
	{ 
		XMFLOAT4X4* eWorldMatrix = &e->World;

		XMMATRIX world = XMLoadFloat4x4(eWorldMatrix);
		XMMATRIX InvWorld = XMMatrixInverse(&XMMatrixDeterminant(world), world);
		XMMATRIX texTransform = XMLoadFloat4x4(&e->TexTransform);
		
		//	视锥剔除
		XMMATRIX viewToLocal = XMMatrixMultiply(invView, InvWorld);
		//	创建视锥�?
		BoundingFrustum localSpaceFrustum;
		localSpaceFrustum.CreateFromMatrix(localSpaceFrustum, mCamera.GetProj());
		//	将视锥体从观察空间变换到局部空�?
		localSpaceFrustum.Transform(localSpaceFrustum, viewToLocal);

		bool enable = e->Enable;
		if ((localSpaceFrustum.Contains(e->Bound) != DirectX::DISJOINT) && enable)
		{
			ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
			XMStoreFloat4x4(&objConstants.InvWorld, XMMatrixTranspose(InvWorld));
			XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));
			objConstants.MaterialIndex = e->Mat->MatCBIndex;

			currObjectCB->CopyData(e->ObjCBIndex, objConstants);
		}
	}
}

void GraphicsCore::UpdateMaterialBuffer(const GameTimer& gt)
{
	auto PBRMaterialBuffer = mCurrFrameResource->PBRMaterialBuffer.get();
	auto SkyBoxMaterialBuffer = mCurrFrameResource->SkyBoxMaterialBuffer.get();
	auto PostProcessBuffer = mCurrFrameResource->PostMaterialBuffer.get();

	//	更新GizmosBuffer
	mGizmoManager.getInstance().UpdateSceneMaterialBuffer(PBRMaterialBuffer);

	//	更新场景材质Buffer
	mSceneManager.getInstance().UpdateSceneMaterialBuffer(PBRMaterialBuffer, SkyBoxMaterialBuffer, matCBIndexUtils);

	//	更新后处理材质Buffer
	mPostProcessManager.getInstance().UpdateMaterialBuffer(PostProcessBuffer, matCBIndexUtils);
}

   

void GraphicsCore::UpdateShadowTransform(const GameTimer& gt)
{
	// Only the first "main" light casts a shadow.
	XMVECTOR lightDir = mSceneManager.getInstance().MainLight->GetComponent<Transform>(1)->forward.ToSIMD();
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
	XMMATRIX view = mCamera.getInstance().GetView();
	XMMATRIX proj = mCamera.getInstance().GetProj();

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
	mCamera.getInstance().tick(gt);
	mMainPassCB.EyePosW = mCamera.getInstance().position;
	mMainPassCB.RenderTargetSize = Vector2((float)mClientWidth, (float)mClientHeight);
	mMainPassCB.InvRenderTargetSize = Vector2(1.0f / mClientWidth, 1.0f / mClientHeight);
	mMainPassCB.NearZ = 1.0f;
	mMainPassCB.FarZ = 1000.0f;
	mMainPassCB.TotalTime = gt.TotalTime();
	mMainPassCB.DeltaTime = gt.DeltaTime();

	//Fog
	mMainPassCB.FogColor = mFogSettings.FogColor;
	mMainPassCB.LinearFogParam = Vector4(mFogSettings.fogStrat, mFogSettings.fogEnd, mFogSettings.FogDensity, mFogSettings.EnableHeightFog);
	mMainPassCB.HeightFogParam = Vector4(mFogSettings.FogFeather,mFogSettings.FogStep,mFogSettings.HeightMin,mFogSettings.HeightMax);
	mMainPassCB.VolumeFogParam0 = Vector4(mFogSettings.CubePos, mFogSettings.NoiseStrength);
	mMainPassCB.VolumeFogParam1 = Vector4(mFogSettings.CubeScale);
	mMainPassCB.VolumeFogParam2 = Vector4(mFogSettings.CameraDir);

	//Scattering
	mMainPassCB.Parames01 = Vector4(mSceneManager.getInstance().mSkyBoxSetting.EnableScatteringSky, mSceneManager.getInstance().mSkyBoxSetting.SunHeight, mSceneManager.getInstance().mSkyBoxSetting.ZenithOffset, mSceneManager.getInstance().mSkyBoxSetting.Density);
	mMainPassCB.Parames02 = Vector4(mSceneManager.getInstance().mSkyBoxSetting.MultiScatterPhase, mSceneManager.getInstance().mSkyBoxSetting.AnisotropicIntensity, 0, 0);
	mMainPassCB.SkyColor = mSceneManager.getInstance().mSkyBoxSetting.SkyColor;

	//AntialiasingSettings
	mMainPassCB.FxaaParames = Vector4(mAntialiasingSettings.AbsoluteLumaThreshold, mAntialiasingSettings.RelativeLumaThreshold, mAntialiasingSettings.ConsoleCharpness, mAntialiasingSettings.DebugMode);

	//Light
	mMainPassCB.DirectionLights.Direction = mSceneManager.getInstance().MainLight->GetComponent<Transform>(1)->forward;
	mMainPassCB.DirectionLights.Strength = mSceneManager.getInstance().MainLight->GetComponent<DirectionLight>(2)->intensity;
	mMainPassCB.DirectionLights.Color = Vector3(mSceneManager.getInstance().MainLight->GetComponent<DirectionLight>(2)->color);
	mMainPassCB.DirectionLights.CastShadow = 1;
	mMainPassCB.DirectionLights.Position = mSceneManager.getInstance().MainLight->GetComponent<Transform>(1)->position;
	bool lightEnable = mSceneManager.getInstance().MainLight->Enable;
	mMainPassCB.DirectionLights.Active = (int)mSceneManager.getInstance().MainLight->Enable;

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

void GraphicsCore::UpdateRenderItems(const GameTimer& gt)
{

}

void GraphicsCore::BuildRootSignature()
{
	mRootSignature.Build(RootSignature::RootSignatureType::Default, md3dDevice, 9, 0, 0, 20, 9, 0);
}

//swap chain  BuildDescriptorHeaps
void GraphicsCore::BuildDescriptorHeaps()
{
	mSrvDescriptorHeap.Build(md3dDevice, 128);

	CD3DX12_CPU_DESCRIPTOR_HANDLE CPUDescriptor(mSrvDescriptorHeap.GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
	CD3DX12_GPU_DESCRIPTOR_HANDLE GPUDescriptor(mSrvDescriptorHeap.GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

	UINT mCbvSrvUavDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	UINT RenderTargetStartIndex = 0;
	UINT TextureIndex = 0;
	//Load Icon
	for (size_t i = 0; i < mProjectIconTextures.size(); i++)
	{
		int image_width = 0;
		int image_height = 0;

		ID3D12Resource* texture = NULL;

		// Load the texture from a file
		bool ret = ResourcesManager::LoadTextureFromFile(mProjectIconTextures[i].c_str(), md3dDevice, CPUDescriptor, &texture, &image_width, &image_height);

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
		CPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
		GPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
	}

	//Load Gizmo
	for (size_t i = 0; i < mProjectGizmoTextures.size(); i++)
	{
		int image_width = 0;
		int image_height = 0;

		ID3D12Resource* texture = NULL;

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

		CPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
		GPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
	}

	//Load Project
	for (size_t i = 0; i < mProjectResourceTextures.size(); i++)
	{
		int image_width = 0;
		int image_height = 0;

		ID3D12Resource* texture = NULL;

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

		CPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
		GPUDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
	}

	RenderTargetStartIndex = TextureIndex;

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
				_forwardPassCmdList->GetInternal().Get(), tex->Path.c_str(),
				tex->Resource, tex->UploadHeap));

			mCubeMapTextures[tex->Name] = std::move(tex);
		}
	}

	GraphicsUtils::BuildTextureCubeSrvDesc(md3dDevice, mCbvSrvUavDescriptorSize, CPUDescriptor, GPUDescriptor, mCubeMapTextures, "Sky_diffuseIBL");
	GraphicsUtils::BuildTextureCubeSrvDesc(md3dDevice, mCbvSrvUavDescriptorSize, CPUDescriptor, GPUDescriptor, mCubeMapTextures, "Sky_specularIBL");

	mRenderTexture->BuildDescriptors(
		CPUDescriptor,
		GPUDescriptor,
		mCbvSrvUavDescriptorSize
	);

	mShadowMap->BuildDescriptors(
		CPUDescriptor, 
		GPUDescriptor,
		GetDsv(1), 
		mCbvSrvUavDescriptorSize
	);

	mDepthPass->BuildDescriptors(
		mDepthStencilBuffer.Get(),
		CPUDescriptor,
		GPUDescriptor,
		mCbvSrvUavDescriptorSize
	);

	//mGBufferPass->BuildDescriptors(
	//	CPUDescriptor,
	//	GPUDescriptor,
	//	GetRtv(SwapChainBufferCount),
	//	mCbvSrvUavDescriptorSize,
	//	mRtvDescriptorSize
	//);
}


void GraphicsCore::BuildShadersAndInputLayout()
{
	mShaderManager.Build();
}

void GraphicsCore::BuildPSOs()
{
	// 	PSO for Line.
	PipelineStateObject LineObject = PipelineStateObject();
	LineObject.BuildDefault(mShaderManager,mRootSignature);
	LineObject.SetShader(mShaderManager, mRootSignature, "lineVS", "linePS");
	LineObject.SetRasterizerState(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE);
	LineObject.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(LineObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["Line"])));

	// 	PSO for Gizmo.
	PipelineStateObject GizmoObject = PipelineStateObject();
	GizmoObject.BuildDefault(mShaderManager, mRootSignature);
	GizmoObject.SetShader(mShaderManager, mRootSignature, "gizmoVS", "gizmoPS");
	GizmoObject.SetDefaultBlend();
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(GizmoObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["Gizmo"])));

	//PSO for Unlit.
	PipelineStateObject AxisObject = PipelineStateObject();
	AxisObject.BuildDefault(mShaderManager, mRootSignature);
	AxisObject.SetShader(mShaderManager, mRootSignature, "unlitVS", "unlitPS");
	AxisObject.SetDefaultBlend();
	AxisObject.SetDepthStencilState(true, D3D12_DEPTH_WRITE_MASK_ALL, D3D12_COMPARISON_FUNC_ALWAYS);
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(AxisObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["Axis"])));

	//	PSO for GBuffer
	PipelineStateObject GBufferObject = PipelineStateObject();
	GBufferObject.BuildDefault(mShaderManager, mRootSignature);
	GBufferObject.SetShader(mShaderManager, mRootSignature, "gbufferVS", "gbufferPS");
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(GBufferObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["GBuffer"])));

	// PSO for Lit.
	PipelineStateObject LitObject = PipelineStateObject();
	LitObject.BuildDefault(mShaderManager, mRootSignature);
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(LitObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["litOpaque"])));

	// 	PSO for Lit. (Transparent)
	PipelineStateObject LitTransparentObject = PipelineStateObject();
	LitTransparentObject.BuildDefault(mShaderManager, mRootSignature);
	LitTransparentObject.SetDefaultBlend();
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(LitTransparentObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["transparent"])));

	// PSO for shadow map pass.
	PipelineStateObject SmapObject = PipelineStateObject();
	SmapObject.Build(mShaderManager,mRootSignature,"shadowVS","shadowOpaquePS",0,DXGI_FORMAT_UNKNOWN);
	SmapObject.SetRasterizerDepth(100000, 0.0f, 1.0f);
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(SmapObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["shadow_opaque"])));

	// PSO for debug layer.
	PipelineStateObject DebugObject = PipelineStateObject();
	DebugObject.BuildDefault(mShaderManager, mRootSignature);
	DebugObject.SetShader(mShaderManager, mRootSignature, "debugVS", "debugPS");
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(DebugObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["debug"])));

	// 	PSO for FinalBlit
	PipelineStateObject FinalBlitObject = PipelineStateObject();
	FinalBlitObject.BuildDefault(mShaderManager, mRootSignature);
	FinalBlitObject.SetShader(mShaderManager, mRootSignature, "finalBlitVS", "finalBlitPS");
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(FinalBlitObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["FinalBlit"])));


	// 	PSO for CopyColor
	PipelineStateObject CopyColorObject = PipelineStateObject();
	CopyColorObject.BuildDefault(mShaderManager, mRootSignature);
	CopyColorObject.SetShader(mShaderManager, mRootSignature, "copyColorVS", "copyColorPS");
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(CopyColorObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["CopyColor"])));

	// 	PSO for RGBSplit
	PipelineStateObject RGBSplitObject = PipelineStateObject();
	RGBSplitObject.BuildDefault(mShaderManager, mRootSignature);
	RGBSplitObject.SetShader(mShaderManager, mRootSignature, "rgbSplitVS", "rgbSplitPS");
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(RGBSplitObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["RGBSplit"])));

	// 	PSO for RadialBlur
	PipelineStateObject RadialBlurObject = PipelineStateObject();
	RadialBlurObject.BuildDefault(mShaderManager, mRootSignature);
	RadialBlurObject.SetShader(mShaderManager, mRootSignature, "radialBlurVS", "radialBlurPS");
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(RadialBlurObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["RadialBlur"])));

	// 	PSO for Vignette
	PipelineStateObject VignetteObject = PipelineStateObject();
	VignetteObject.BuildDefault(mShaderManager, mRootSignature);
	VignetteObject.SetShader(mShaderManager, mRootSignature, "vignetteVS", "vignettePS");
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(VignetteObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["Vignette"])));

	// 	PSO for Decolor
	PipelineStateObject DecolorObject = PipelineStateObject();
	DecolorObject.BuildDefault(mShaderManager, mRootSignature);
	DecolorObject.SetShader(mShaderManager, mRootSignature, "decolorVS", "decolorPS");
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(DecolorObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["Decolor"])));

	// 	PSO for Brightness
	PipelineStateObject BrightnessObject = PipelineStateObject();
	BrightnessObject.BuildDefault(mShaderManager, mRootSignature);
	BrightnessObject.SetShader(mShaderManager, mRootSignature, "brightnessVS", "brightnessPS");
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(BrightnessObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["Brightness"])));

	// 	PSO for HSV
	PipelineStateObject HsvObject = PipelineStateObject();
	HsvObject.BuildDefault(mShaderManager, mRootSignature);
	HsvObject.SetShader(mShaderManager, mRootSignature, "hsvVS", "hsvPS");
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(HsvObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["HSV"])));

	// 	PSO for Mosaic
	PipelineStateObject MosaicObject = PipelineStateObject();
	MosaicObject.BuildDefault(mShaderManager, mRootSignature);
	MosaicObject.SetShader(mShaderManager, mRootSignature, "mosaicVS", "mosaicPS");
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(MosaicObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["Mosaic"])));

	// 	PSO for Sharpen
	PipelineStateObject SharpenObject = PipelineStateObject();
	SharpenObject.BuildDefault(mShaderManager, mRootSignature);
	SharpenObject.SetShader(mShaderManager, mRootSignature, "sharpenVS", "sharpenPS");
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(SharpenObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["Sharpen"])));

	// 	PSO for Spherize
	PipelineStateObject SpherizeObject = PipelineStateObject();
	SpherizeObject.BuildDefault(mShaderManager, mRootSignature);
	SpherizeObject.SetShader(mShaderManager, mRootSignature, "spherizeVS", "spherizePS");
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(SpherizeObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["Spherize"])));

	// 	PSO for WhiteBalance
	PipelineStateObject WhiteBalanceObject = PipelineStateObject();
	WhiteBalanceObject.BuildDefault(mShaderManager, mRootSignature);
	WhiteBalanceObject.SetShader(mShaderManager, mRootSignature, "whiteBalanceVS", "whiteBalancePS");
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(WhiteBalanceObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["WhiteBalance"])));

	// 	PSO for OilPaint
	PipelineStateObject OilPaintObject = PipelineStateObject();
	OilPaintObject.BuildDefault(mShaderManager, mRootSignature);
	OilPaintObject.SetShader(mShaderManager, mRootSignature, "oilPaintVS", "oilPaintPS");
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(OilPaintObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["OilPaint"])));

	// 	PSO for Relief
	PipelineStateObject ReliefObject = PipelineStateObject();
	ReliefObject.BuildDefault(mShaderManager, mRootSignature);
	ReliefObject.SetShader(mShaderManager, mRootSignature, "reliefVS", "reliefPS");
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(ReliefObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["Relief"])));

	// 	PSO for EdgeDetection
	PipelineStateObject EdgeDetectionObject = PipelineStateObject();
	EdgeDetectionObject.BuildDefault(mShaderManager, mRootSignature);
	EdgeDetectionObject.SetShader(mShaderManager, mRootSignature, "edgeDetectionVS", "edgeDetectionPS");
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(EdgeDetectionObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["EdgeDetection"])));

	// 	PSO for VolumeFog
	PipelineStateObject VolumeFogObject = PipelineStateObject();
	VolumeFogObject.BuildDefault(mShaderManager, mRootSignature);
	VolumeFogObject.SetShader(mShaderManager, mRootSignature, "volumeFogVS", "volumeFogPS");
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(VolumeFogObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["VolumeFog"])));

	// 	PSO for FxAA
	PipelineStateObject FxAAObject = PipelineStateObject();
	FxAAObject.BuildDefault(mShaderManager, mRootSignature);
	FxAAObject.SetShader(mShaderManager, mRootSignature, "FastApproximateAntialiasingVS", "FastApproximateAntialiasingPS");
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(FxAAObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["FxAA"])));

	// PSO for sky.
	PipelineStateObject SkyBoxObject = PipelineStateObject();
	SkyBoxObject.BuildDefault(mShaderManager, mRootSignature);
	SkyBoxObject.SetShader(mShaderManager, mRootSignature, "skyVS", "skyPS");
	SkyBoxObject.SetDepthStencilState(true, D3D12_DEPTH_WRITE_MASK_ALL, D3D12_COMPARISON_FUNC_LESS_EQUAL);
	SkyBoxObject.SetRasterizerState(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE);
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(SkyBoxObject.GetPSODesc(), IID_PPV_ARGS(&mPSOs["sky"])));
}

void GraphicsCore::BuildFrameResources()
{
	for (int i = 0; i < gNumFrameResources; ++i)
	{
		mFrameResources.push_back(
			std::make_unique<FrameResource>(md3dDevice.Get(),
			2, 
			(UINT)mAllRitems.size(),
			(UINT)(mGizmoManager.getInstance().mRenderObjects.size()) +
			(UINT)(mSceneManager.getInstance().mRenderObjects.size()) +
			(UINT)(mPostProcessManager.getInstance().mRenderObjects.size())
			)
		);
	}
}

void GraphicsCore::BuildRenderItems()
{
	mGizmoManager.getInstance().BuildRenderItem(mRitemLayer, mAllRitems, md3dDevice, _forwardPassCmdList->GetInternal());
	mSceneManager.getInstance().BuildRenderItem(mRitemLayer, mAllRitems, md3dDevice, _forwardPassCmdList->GetInternal(), mGizmoManager.getInstance().mRenderObjects.size(), matCBIndexUtils);
	mPostProcessManager.getInstance().BuildRenderItem(mRitemLayer, mAllRitems, md3dDevice, _forwardPassCmdList->GetInternal(), mGizmoManager.getInstance().mRenderObjects.size() + mSceneManager.getInstance().mRenderObjects.size(), matCBIndexUtils);
}

void GraphicsCore::DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems)
{
	UINT objCBByteSize = DX12Utils::CalcConstantBufferByteSize(sizeof(ObjectConstants));

	auto objectCB = mCurrFrameResource->ObjectCB->Resource();

	for (size_t i = 0; i < ritems.size(); ++i)
	{
		auto ri = ritems[i];

		if (ri->Enable)
		{
			cmdList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
			cmdList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
			cmdList->IASetPrimitiveTopology(ri->PrimitiveType);

			D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ri->ObjCBIndex * objCBByteSize;

			cmdList->SetGraphicsRootConstantBufferView(0, objCBAddress);

			cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
		}
	}
}

void GraphicsCore::DrawSceneToShadowMap()
{
	_forwardPassCmdList->GetInternal()->SetGraphicsRootDescriptorTable(5, mSrvDescriptorHeap.GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

	_forwardPassCmdList->GetInternal()->RSSetViewports(1, &mShadowMap->Viewport());
	_forwardPassCmdList->GetInternal()->RSSetScissorRects(1, &mShadowMap->ScissorRect());

	// Change to DEPTH_WRITE.
	_forwardPassCmdList->GetInternal()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mShadowMap->Resource(),
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	// Clear the back buffer and depth buffer.
	_forwardPassCmdList->GetInternal()->ClearDepthStencilView(mShadowMap->Dsv(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.
	_forwardPassCmdList->GetInternal()->OMSetRenderTargets(0, nullptr, false, &mShadowMap->Dsv());

	// Bind the pass constant buffer for the shadow map pass.
	UINT passCBByteSize = DX12Utils::CalcConstantBufferByteSize(sizeof(PassConstants));
	auto passCB = mCurrFrameResource->PassCB->Resource();
	D3D12_GPU_VIRTUAL_ADDRESS passCBAddress = passCB->GetGPUVirtualAddress() + 1 * passCBByteSize;
	_forwardPassCmdList->GetInternal()->SetGraphicsRootConstantBufferView(2, passCBAddress);

	_forwardPassCmdList->GetInternal()->SetPipelineState(mPSOs["shadow_opaque"].Get());
	DrawRenderItems(_forwardPassCmdList->GetInternal().Get(), mRitemLayer[(int)RenderLayer::Opaque]);

	// Change back to GENERIC_READ so we can read the texture in a shader.
	_forwardPassCmdList->GetInternal()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mShadowMap->Resource(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ));
}

void GraphicsCore::OnMouseMoveAndSelect(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		XMFLOAT4X4 P = mCamera.getInstance().GetProj4x4f();

		// Compute picking ray in view space.
		float vx = (+2.0f * x / mClientWidth - 1.0f) / P(0, 0);
		float vy = (-2.0f * y / mClientHeight + 1.0f) / P(1, 1);

		// Ray definition in view space.
		XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

		XMMATRIX V = mCamera.getInstance().GetView();
		XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(V), V);

		for (auto ri : mRitemLayer[(int)RenderLayer::Axis])
		{
			auto geo = ri->Geo;

			XMMATRIX W = XMLoadFloat4x4(&ri->World);
			XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);

			// Tranform ray to vi space of Mesh.
			XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

			rayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
			rayDir = XMVector3TransformNormal(rayDir, toLocal);

			// Make the ray direction unit length for the intersection tests.
			rayDir = XMVector3Normalize(rayDir);

			float tmin = 1000.0f;
			if (ri->Bound.Intersects(rayOrigin, rayDir, tmin))
			{
				//float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
				//float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));
			}
		}
	}
}

//void GraphicsCore::OnTestMove(const GameTimer& gt)
//{
//	const float dt = gt.DeltaTime();
//
//	if (mGizmoManager.getInstance().PositionUCSEnable)
//	{
//		if (GetAsyncKeyState('1') & 0x8000)
//			mSceneManager.getInstance().mMeshRender[1]->AddPosition(Vector3(15 * dt,0,0));
//
//		if (GetAsyncKeyState('2') & 0x8000)
//			mSceneManager.getInstance().mMeshRender[1]->AddPosition(Vector3(-15 * dt,0,0));
//
//		if (GetAsyncKeyState('3') & 0x8000)
//			mSceneManager.getInstance().mMeshRender[1]->AddPosition(Vector3(0, 15 * dt, 0));
//
//		if (GetAsyncKeyState('4') & 0x8000)
//			mSceneManager.getInstance().mMeshRender[1]->AddPosition(Vector3(0, -15 * dt, 0));
//
//		if (GetAsyncKeyState('5') & 0x8000)
//			mSceneManager.getInstance().mMeshRender[1]->AddPosition(Vector3(0, 0, 15 * dt));
//
//		if (GetAsyncKeyState('6') & 0x8000)
//			mSceneManager.getInstance().mMeshRender[1]->AddPosition(Vector3(0, 0, -15 * dt));
//	}
//	else if(mGizmoManager.getInstance().RotationUCSEnable)
//	{
//		if (GetAsyncKeyState('1') & 0x8000)
//			mSceneManager.getInstance().mMeshRender[1]->AddEulerangle(Vector3(50 * dt, 0, 0));
//
//		if (GetAsyncKeyState('2') & 0x8000)
//			mSceneManager.getInstance().mMeshRender[1]->AddEulerangle(Vector3(-50 * dt, 0, 0));
//
//		if (GetAsyncKeyState('3') & 0x8000)
//			mSceneManager.getInstance().mMeshRender[1]->AddEulerangle(Vector3(0, 50 * dt, 0));
//
//		if (GetAsyncKeyState('4') & 0x8000)
//			mSceneManager.getInstance().mMeshRender[1]->AddEulerangle(Vector3(0, -50 * dt, 0));
//
//		if (GetAsyncKeyState('5') & 0x8000)
//			mSceneManager.getInstance().mMeshRender[1]->AddEulerangle(Vector3(0, 0, 50 * dt));
//
//		if (GetAsyncKeyState('6') & 0x8000)
//			mSceneManager.getInstance().mMeshRender[1]->AddEulerangle(Vector3(0, 0, -50 * dt));
//	}
//	else if (mGizmoManager.getInstance().ScaleUCSEnable)
//	{
//		if (GetAsyncKeyState('1') & 0x8000)
//			mSceneManager.getInstance().mMeshRender[1]->AddScale(Vector3(2 * dt, 0, 0));
//
//		if (GetAsyncKeyState('2') & 0x8000)
//			mSceneManager.getInstance().mMeshRender[1]->AddScale(Vector3(-2 * dt, 0, 0));
//
//		if (GetAsyncKeyState('3') & 0x8000)
//			mSceneManager.getInstance().mMeshRender[1]->AddScale(Vector3(0, 2 * dt, 0));
//
//		if (GetAsyncKeyState('4') & 0x8000)
//			mSceneManager.getInstance().mMeshRender[1]->AddScale(Vector3(0, -2 * dt, 0));
//
//		if (GetAsyncKeyState('5') & 0x8000)
//			mSceneManager.getInstance().mMeshRender[1]->AddScale(Vector3(0, 0, 2 * dt));
//
//		if (GetAsyncKeyState('6') & 0x8000)
//			mSceneManager.getInstance().mMeshRender[1]->AddScale(Vector3(0, 0, -2 * dt));
//	}
//}

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