#pragma once
#include "InputSystem.h"
#include "Material.h"
#include "MeshGeometry.h"
#include "Camera.h"
#include "FrameResource.h"
#include "ShadowMap.h"
#include "Ssao.h"
#include "ResourcesManager.h"
#include "SceneManager.h"
#include "RootSignature.h"
#include "DescriptorHeap.h"
#include "ShaderManager.h"
#include "PipelineStateObject.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class GraphicsCore : public InputSystem
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
	//	更新灯光
	void UpdateLights(const GameTimer& gt);
	//	更新ObjectCB
	void UpdateObjectCBs(const GameTimer& gt);
	//	更新材质Buffer
	void UpdateMaterialBuffer(const GameTimer& gt);
	//	更新ShadowTransform
	void UpdateShadowTransform(const GameTimer& gt);
	//	更新MainPass
	void UpdateMainPassCB(const GameTimer& gt);
	//	更新ShadowPass
	void UpdateShadowPassCB(const GameTimer& gt);
	//	更新SSAO PASS
	void UpdateSsaoCB(const GameTimer& gt);


	//	构建根签名
	void BuildRootSignature();
	//	构建描述符堆
	void BuildDescriptorHeaps();
	//	构建Shader
	void BuildShadersAndInputLayout();
	//	构建PSO
	void BuildPSOs();
	//	构建帧资源
	void BuildFrameResources();
	//	构建渲染项
	void BuildRenderItems();





	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);
	void DrawSceneToShadowMap();
	void DrawNormalsAndDepth();

	CD3DX12_CPU_DESCRIPTOR_HANDLE GetDsv(int index)const;
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetRtv(int index)const;

public:

	//Textures
	std::unordered_map<std::string, std::unique_ptr<Texture2D>> mIconTextures;
	std::unordered_map<std::string, std::unique_ptr<Texture2D>> mResourcesTextures;
	std::unordered_map<std::string, std::unique_ptr<Texture2D>> mGizmosTextures;
	std::unordered_map<std::string, std::unique_ptr<TextureCube>> mCubeMapTextures;

	//	场景管理
	SceneManager mSceneManager = SceneManager(SceneType::Default);


	std::vector<std::unique_ptr<FrameResource>> mFrameResources;
	FrameResource* mCurrFrameResource = nullptr;
	int mCurrFrameResourceIndex = 0;

	//	根签名
	RootSignature mRootSignature = RootSignature();
	RootSignature mSsaoRootSignature = RootSignature();

	//	描述符堆
	DescriptorHeap mSrvDescriptorHeap = DescriptorHeap();

	//	PSO
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;

	//	ShaderManager
	ShaderManager mShaderManager = ShaderManager();

	// List of all the render items.
	std::vector<std::unique_ptr<RenderItem>> mAllRitems;

	// Render items divided by PSO.
	std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count];























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