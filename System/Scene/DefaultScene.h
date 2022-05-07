#pragma once
#include "../../Component/MeshRender.h"
#include "../../Component/DirectionLight.h"
#include "../../Component/Camera.h"
#include "../EngineSystem/GameTimer.h"
#include "../EngineSystem/SystemUtils.h"
#include "../EngineSystem/Texture.h"
#include "../EngineSystem/DefaultFrameResource.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

namespace Dist
{
	class DefaultScene
	{
	public:
		DefaultScene() = default;
		DefaultScene(const DefaultScene& rhs) = delete;
		DefaultScene& operator=(const DefaultScene& rhs) = delete;
		virtual ~DefaultScene() = default;

	public:
		//----------------
		//	场景结构内容
		//----------------
		//	主光源
		DirectionLight mMainLight;

		//	Mesh Render	
		std::vector<std::unique_ptr<PBRMeshRender>> mMeshRender;

		//	Camera
		Camera mCamera;

		//	SkyBox Textures
		std::wstring mSkyBoxTexPath;

		//----------------
		//	场景资源内容
		//----------------
		//	需要加载的Shader
		std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;

		//	需要设置的InputLayout
		std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

		//----------------
		//	场景绑定资源
		//----------------
		//	场景根签名
		ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
		//	SSAO根签名
		ComPtr<ID3D12RootSignature> mSsaoRootSignature = nullptr; 
		//	渲染项
		std::vector<std::unique_ptr<PBRRenderItem>> mAllRitems;
		//	渲染PSO层级
		std::vector<PBRRenderItem*> mRitemLayer[(int)RenderLayer::Count];
		//	场景帧资源
		std::vector<std::unique_ptr<DefaultFrameResource>> mFrameResources;
		DefaultFrameResource* mCurrFrameResource = nullptr;
		int mCurrFrameResourceIndex = 0;

		//	场景包围盒
		DirectX::BoundingSphere mSceneBounds;

	public:

		//	初始化场景
		void InitScene(Microsoft::WRL::ComPtr<ID3D12Device> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList);

		//	更新场景
		void UpdateScene(const GameTimer& gt);

	private:

		//	构建场景灯光
		void BuildLights();

		//	构建场景相机
		void BuildCamera();

		//	构建根签名
		void BuildRootSignature(Microsoft::WRL::ComPtr<ID3D12Device> device);

		//	构建SSAO根签名
		void BuildSsaoRootSignature(Microsoft::WRL::ComPtr<ID3D12Device> device);

		//	构建着色器
		void BuildShadersAndInputLayout();

		//	更新灯光
		void UpdateLights(const GameTimer& gt);

		//	更新物体CB
		void UpdateObjectCBs(const GameTimer& gt);

		//	更新材质
		void UpdateMaterials(const GameTimer& gt);

		//	加载MeshRender
		void LoadMeshRender();

		//	加载渲染项目
		void LoadRenderItem(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList);
	};
}
