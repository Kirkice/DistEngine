#pragma once
#include "../../Component/MeshRender.h"
#include "../../Component/DirectionLight.h"
#include "../../Component/Camera.h"
#include "../EngineSystem/GameTimer.h"
#include "../EngineSystem/SystemUtils.h"
#include "../EngineSystem/Texture.h"
#include "../EngineSystem/DefaultFrameResource.h"
#include <string.h>

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

namespace Dist
{
	class DefaultScene
	{
		//	加载的图片类型
		enum class TexturesType
		{
			RenderItem,
			Gizom,
		};

		//Render Layer
		enum class RenderLayer : int
		{
			Opaque = 0,
			Transparent = 1,
			Sky = 2,
			Gizmo = 3,
			Bounding = 4,
			PostProcessing = 5,
			Count
		};


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
		//	需要加载的纹理
		std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
		//	需要加载的CubeMap纹理
		std::unordered_map<std::string, std::unique_ptr<Texture>> mSkyTextures;
		//	需要加载的Gizmo纹理
		std::unordered_map<std::string, std::unique_ptr<Texture>> mGizmoTextures;
		//	需要设置的InputLayout
		std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

		//----------------
		//	场景绑定资源
		//----------------
		//	场景根签名
		ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
		//	SSAO根签名
		ComPtr<ID3D12RootSignature> mSsaoRootSignature = nullptr; 
		//	场景SRV
		ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;
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
		DefaultScene();
		~DefaultScene();

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

		//	加载图片路径
		std::vector<ComPtr<ID3D12Resource>>& LoadTextures(DefaultScene::TexturesType type);
	};
}
