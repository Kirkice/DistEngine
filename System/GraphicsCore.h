#pragma once
#include "../Core/Mathf/Mathf.h"
#include "Material.h"
#include "MeshGeometry.h"
#include "d3dApp.h"
#include "Camera.h"
#include "FrameResource.h"
#include "ShadowMap.h"
#include "Ssao.h"
#include "RendererCore.h"
#include "ResourcesManager.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class GraphicsCore : public RenderCore
{
public:
	GraphicsCore(HINSTANCE hInstance);
	GraphicsCore(const GraphicsCore& rhs) = delete;
	GraphicsCore& operator=(const GraphicsCore& rhs) = delete;
	~GraphicsCore();

	virtual bool Initialize()override;

private:
	virtual void CreateRtvAndDsvDescriptorHeaps()override;
	virtual void OnResize()override;
	virtual void Update(const GameTimer& gt)override;

public:

	void UpdateLights(const GameTimer& gt);
	void AnimateMaterials(const GameTimer& gt);
	void UpdateObjectCBs(const GameTimer& gt);
	void UpdatePBRMaterialBuffer(const GameTimer& gt);
	void UpdateShadowTransform(const GameTimer& gt);
	void UpdateMainPassCB(const GameTimer& gt);
	void UpdateShadowPassCB(const GameTimer& gt);
	void UpdateSsaoCB(const GameTimer& gt);

	void BuildRootSignature();
	void BuildSsaoRootSignature();

	void BuildDescriptorHeaps();

	void BuildShadersAndInputLayout();
	void BuildShapeGeometry();
	void BuildPSOs();
	void BuildFrameResources();
	void BuildMaterials();
	void BuildRenderItems();

	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);
	void DrawSceneToShadowMap();
	void DrawNormalsAndDepth();

	CD3DX12_CPU_DESCRIPTOR_HANDLE GetCpuSrv(int index, ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap)const;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetGpuSrv(int index, ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap)const;
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetDsv(int index)const;
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetRtv(int index)const;

	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 7> GetStaticSamplers();

public:
	ResourcesManager mResManager = ResourcesManager();

	std::vector<std::unique_ptr<FrameResource>> mFrameResources;
	FrameResource* mCurrFrameResource = nullptr;
	int mCurrFrameResourceIndex = 0;

	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	ComPtr<ID3D12RootSignature> mSsaoRootSignature = nullptr;

	ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
	std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;

	//Textures
	std::unordered_map<std::string, std::unique_ptr<Texture2D>> mIconTextures;
	std::unordered_map<std::string, std::unique_ptr<Texture2D>> mResourcesTextures;
	std::unordered_map<std::string, std::unique_ptr<Texture2D>> mGizmosTextures;
	std::unordered_map<std::string, std::unique_ptr<TextureCube>> mCubeMapTextures;

	std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
	std::vector<D3D12_INPUT_ELEMENT_DESC> mSkinnedInputLayout;

	// List of all the render items.
	std::vector<std::unique_ptr<RenderItem>> mAllRitems;

	// Render items divided by PSO.
	std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count];

	UINT mNullCubeSrvIndex = 0;
	UINT mNullTexSrvIndex1 = 0;
	UINT mNullTexSrvIndex2 = 0;

	CD3DX12_GPU_DESCRIPTOR_HANDLE mNullSrv;

	PassConstants mMainPassCB;  // index 0 of pass cbuffer.
	PassConstants mShadowPassCB;// index 1 of pass cbuffer.

	std::unique_ptr<RenderTarget> mRenderTarget;
	std::unique_ptr<ShadowMap> mShadowMap;

	std::unique_ptr<Ssao> mSsao;

	DirectX::BoundingSphere mSceneBounds;

	//Shadow
	float mLightNearZ = 0.0f;
	float mLightFarZ = 0.0f;
	XMFLOAT3 mLightPosW;
	XMFLOAT4X4 mLightView = Mathf::Identity4x4();
	XMFLOAT4X4 mLightProj = Mathf::Identity4x4();
	XMFLOAT4X4 mShadowTransform = Mathf::Identity4x4();

	float BackGroundTransform[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float BackGroundRotation = 0.0f;
	float BackGroundScale[4] = { 0.6f, 0.6f, 0.6f, 0.6f };


	//��ȾĿ�����
	float TargetTransform[3] = { 0.0f, 1.0f, 0.0f };
	float TargetScale[3] = { 2.0f, 2.0f, 2.0f };
	float TargetRotationAngle[3] = { 0,0,0 };

	//Camera
	float mCamFov = 45;
	float mCamClipN = 0.3;
	float mCamClipF = 1000;

	bool renderSkyBox = true;
	float SolidColor[4] = { 0,0,0,0 };

	//Bounding
	bool ShowBounding = false;

	//Lighting Settings
	float EnvironmentShadowColor[3] = { 0.3f,0.3f,0.3f };
	float SkyBoxTintColor[3] = { 1,1,1 };
	float SkyBoxExposure = 1;
	float SkyBoxRotation = 0;
	float  ACES = 1;

	//Light
	//Direction Lights
	XMFLOAT3 mRotatedLightDirections;
	XMFLOAT3 mDirectionLightsDir = { 0.57f,-0.57f,0.57f };
	XMFLOAT3 mDirectionLightsPos = { -2,5,0 };
	float mDirectionLightsColor[3] = { 1,1,1 };
	float mDirectionLightsAngle[3] = { 0,0,0 };
	float mDirectionLightsScale[3] = { 1,1,1 };
	float mDirectionLightsStrength = 1.2f;
	bool mDirectionLightsCastShadow = true;
	bool mDirectionLightsActive = true;

	//Point Lights
	XMFLOAT3 mPointLightsPos = { 1.5f,2,0 };
	float mPointLightsColor[3] = { 1,1,1 };
	float mPointLightsRange = 3.0;
	float mPointLightsRot[3] = { 0,0,0 };
	float mPointLightsScale[3] = { 1,1,1 };
	float mPointLightsStrength = 3;
	bool mPointLightsActive = false;

	//SpotLights
	XMFLOAT3 mRotatedLightSpots;
	XMFLOAT3 mSpotLightsDir = { 0.57f,-0.57f,0.57f };
	XMFLOAT3 mSpotLightsPos = { 0,0,0 };
	float mSpotLightsColor[3] = { 1,1,1 };
	float mSpotLightsAngle[3] = { 0,0,0 };
	float mSpotLightsScale[3] = { 1,1,1 };
	float mSpotLightsRange = 15.0;
	float mSpotLightsStrength = 1;
	float mSpotLightsCutAngle = 30;
	bool mSpotLightsActive = true;
};