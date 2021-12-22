#pragma once
#include "Mathf.h"
#include "Material.h"
#include "MeshGeometry.h"
#include "Light.h"
#include "d3dApp.h"
#include "Camera.h"
#include "FrameResource.h"
#include "ShadowMap.h"
#include "SkinnedData.h"
#include "LoadM3d.h"
#include "Ssao.h"
#include "FbxLoader.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Renderer;

namespace Renderer
{
	struct RenderItem
	{
		RenderItem() = default;
		RenderItem(const RenderItem& rhs) = delete;

		//�������� 
		XMFLOAT4X4 World = Mathf::Identity4x4();
		//��������
		XMFLOAT4X4 TexTransform = Mathf::Identity4x4();

		int NumFramesDirty = gNumFrameResources;

		UINT ObjCBIndex = -1;

		Material* Mat = nullptr;
		MeshGeometry* Geo = nullptr;

		D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		UINT IndexCount = 0;
		UINT StartIndexLocation = 0;
		int BaseVertexLocation = 0;
	};

	//��Ⱦ�㼶
	enum class RenderLayer : int
	{
		Opaque = 0,
		Transparent = 1,
		Debug = 2,
		Sky = 3,
		Count
	};

	class GraphicsCore : public D3DApp
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
		void OnKeyboardInput(const GameTimer& gt);

	public:

		void UpdateLights(const GameTimer& gt);
		void AnimateMaterials(const GameTimer& gt);
		void UpdateObjectCBs(const GameTimer& gt);
		void UpdatePBRMaterialBuffer(const GameTimer& gt);
		void UpdateShadowTransform(const GameTimer& gt);
		void UpdateMainPassCB(const GameTimer& gt);
		void UpdateShadowPassCB(const GameTimer& gt);
		void UpdateSsaoCB(const GameTimer& gt);

		void LoadTextures();
		void BuildRootSignature();
		void BuildSsaoRootSignature();
		void BuildDescriptorHeaps();
		void BuildShadersAndInputLayout();
		void BuildShapeGeometry();
		void LoadModel();
		void BuildPSOs();
		void BuildFrameResources();
		void BuildMaterials();
		void BuildRenderItems();
		void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);
		void DrawSceneToShadowMap();
		void DrawNormalsAndDepth();

		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCpuSrv(int index)const;
		CD3DX12_GPU_DESCRIPTOR_HANDLE GetGpuSrv(int index)const;
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetDsv(int index)const;
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetRtv(int index)const;

		std::array<const CD3DX12_STATIC_SAMPLER_DESC, 7> GetStaticSamplers();

	public:
		std::vector<std::unique_ptr<FrameResource>> mFrameResources;
		FrameResource* mCurrFrameResource = nullptr;
		int mCurrFrameResourceIndex = 0;

		ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
		ComPtr<ID3D12RootSignature> mSsaoRootSignature = nullptr;

		ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;

		std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
		std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;
		std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
		std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;
		std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;

		std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
		std::vector<D3D12_INPUT_ELEMENT_DESC> mSkinnedInputLayout;

		// List of all the render items.
		std::vector<std::unique_ptr<RenderItem>> mAllRitems;

		// Render items divided by PSO.
		std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count];

		UINT mIBLTexHeapIndex = 0;
		UINT mSkyTexHeapIndex = 0;
		UINT mShadowMapHeapIndex = 0;
		UINT mSsaoHeapIndexStart = 0;
		UINT mSsaoAmbientMapIndex = 0;

		UINT mNullCubeSrvIndex = 0;
		UINT mNullTexSrvIndex1 = 0;
		UINT mNullTexSrvIndex2 = 0;

		CD3DX12_GPU_DESCRIPTOR_HANDLE mNullSrv;

		PassConstants mMainPassCB;  // index 0 of pass cbuffer.
		PassConstants mShadowPassCB;// index 1 of pass cbuffer.

		Camera mCamera;

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

		POINT mLastMousePos;

		float BackGroundTransform[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float BackGroundRotation = 0.0f;
		float BackGroundScale[4] = { 1.5f, 1.5f, 1.5f, 1.5f };


		//��ȾĿ�����
		float TargetTransform[3] = { 0.0f, 1.0f, 0.0f};
		float TargetScale[3] = { 1.0f, 1.0f, 1.0f};
		float TargetRotationAngle[3] = { 0,0,0 };

		//Camera ����
		float mCamFov = 45;
		float mCamClipN = 0.3;
		float mCamClipF = 1000;

		bool renderSkyBox = true;
		float SolidColor[4] = { 0,0,0,0 };

		//Light
		//Direction Lights
		XMFLOAT3 mRotatedLightDirections;
		XMFLOAT3 mDirectionLightsDir = { 0.57f,-0.57f,0.57f};
		XMFLOAT3 mDirectionLightsPos = {0,0,0};
		float mDirectionLightsColor[3] = {1,1,1};
		float mDirectionLightsAngle[3] = {0,0,0};
		float mDirectionLightsScale[3] = { 1,1,1 };
		float mDirectionLightsStrength = 1;
		bool mDirectionLightsCastShadow = true;
		bool mDirectionLightsActive = true;

		//Point Lights
		XMFLOAT3 mPointLightsPos = { 0,0,0 };
		float mPointLightsColor[3] = {1,1,1};
		float mPointLightsConstant = 1.0;
		float mPointLightsLinear = 0.2f;
		float mPointLightsQuadratic = 0.0f;
		float mPointLightsRot[3] = { 0,0,0 };
		float mPointLightsScale[3] = { 1,1,1 };
		float mPointLightsStrength = 1;
		bool mPointLightsActive = true;

		//SpotLights
		XMFLOAT3 mRotatedLightSpots;
		XMFLOAT3 mSpotLightsDir = { 0.57f,-0.57f,0.57f };
		XMFLOAT3 mSpotLightsPos = { 0,0,0 };
		float mSpotLightsColor[3] = { 1,1,1 };
		float mSpotLightsAngle[3] = { 0,0,0 };
		float mSpotLightsScale[3] = { 1,1,1 };
		float mSpotLightsCutoff = 15.0;
		float mSpotLightsStrength = 1;
		bool mSpotLightsActive = true;
	};
}