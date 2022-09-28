#pragma once
#include "../input/InputSystem.h"
#include "../Materials/Material.h"
#include "../Geometry/MeshGeometry.h"
#include "../Component/Camera.h"
#include "FrameResource.h"
#include "../Render/ShadowMap.h"
#include "../Render/DepthPass.h"
#include "../File/ResourcesManager.h"
#include "../SceneManager/SceneManager.h"
#include "RootSignature.h"
#include "DescriptorHeap.h"
#include "../Shaders/ShaderManager.h"
#include "PipelineStateObject.h"
#include "../Render/RenderTexture.h"
//#include "../Render/GBufferPass.h"
#include "../SceneManager/GizmosManager.h"
#include "../SceneManager/PostProcessManager.h"

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
	virtual void Update(const GameTimer& gt)override;
	virtual void OnResize()override;

private:
	virtual void CreateRtvAndDsvDescriptorHeaps()override;

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
	//	更新渲染项
	void UpdateRenderItems(const GameTimer& gt);

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

	void OnMouseMoveAndSelect(WPARAM btnState, int x, int y);
	//void OnTestMove(const GameTimer& gt);


	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);
	void DrawSceneToShadowMap();

	CD3DX12_CPU_DESCRIPTOR_HANDLE GetDsv(int index)const;
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetRtv(int index)const;

public:

	//Textures
	std::unordered_map<std::string, std::unique_ptr<Texture2D>> mIconTextures;
	std::unordered_map<std::string, std::unique_ptr<Texture2D>> mResourcesTextures;
	std::unordered_map<std::string, std::unique_ptr<Texture2D>> mGizmosTextures;
	std::unordered_map<std::string, std::unique_ptr<TextureCube>> mCubeMapTextures;

	//	场景管理
	MaterialIndexUtils matCBIndexUtils = MaterialIndexUtils();
	GizmosManager mGizmoManager = GizmosManager();
	SceneManager mSceneManager = SceneManager(SceneType::Default);
	PostProcessManager mPostProcessManager = PostProcessManager();

	std::vector<std::unique_ptr<FrameResource>> mFrameResources;
	FrameResource* mCurrFrameResource = nullptr;
	int mCurrFrameResourceIndex = 0;

	//	根签名
	RootSignature mRootSignature = RootSignature();

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

	std::unique_ptr<RenderTexture> mRenderTexture;
	std::unique_ptr<ShadowMap> mShadowMap;
	std::unique_ptr<DepthPass> mDepthPass;
	//std::unique_ptr<GBuffer> mGBufferPass;


	DirectX::BoundingSphere mSceneBounds;

	//Shadow
	float mLightNearZ = 0.0f;
	float mLightFarZ = 0.0f;
	XMFLOAT3 mLightPosW;
	XMFLOAT4X4 mLightView = Mathf::Identity4x4();
	XMFLOAT4X4 mLightProj = Mathf::Identity4x4();
	XMFLOAT4X4 mShadowTransform = Mathf::Identity4x4();

	//Bounding
	bool ShowBounding = false;

	bool ShowGizmos = true;

	bool ShowWire = true;

	PostProcessSwitch mPostProcessSwitch = PostProcessSwitch();

	//	Fog Settings
	FogSetting mFogSettings;
	//	AntialiasingSettings
	AntialiasingSettings mAntialiasingSettings;
};