#pragma once
#include "../../Component/MeshRender.h"
#include "../../Component/DirectionLight.h"
#include "../../Component/Camera.h"
#include "../EngineSystem/GameTimer.h"
#include "../EngineSystem/SystemUtils.h"
#include "../EngineSystem/Texture.h"
#include <string.h>

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

namespace Dist
{
	class DefaultScene
	{
		//	加载的图片类型
		enum TexturesType
		{
			RenderItem,
			Gizom,
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
		// 渲染项
		std::vector<std::unique_ptr<PBRRenderItem>> mAllRitems;

	public:
		DefaultScene();
		~DefaultScene();

		//	初始化场景
		void InitScene(Microsoft::WRL::ComPtr<ID3D12Device> device);

		//	更新场景
		void UpdateScene();

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

		//	构建渲染项
		void BuildRenderItems();

		//	更新灯光
		void UpdateLights(const GameTimer& gt);

		//	更新物体CB
		void UpdateObjectCBs(const GameTimer& gt);

		//	更新材质
		void UpdateMaterials(const GameTimer& gt);

		//	更新阴影位置
		void UpdateShadowTransform(const GameTimer& gt);

		//	更新Pass CB
		void UpdateMainPassCB(const GameTimer& gt);

		//	更新Shadow CB
		void UpdateShadowPassCB(const GameTimer& gt);

		//	更新SSAO CB
		void UpdateSsaoCB(const GameTimer& gt);

		//	加载MeshRender
		void LoadMeshRender();

		//	加载渲染项目
		void LoadRenderItem();

		//	加载图片路径
		std::vector<ComPtr<ID3D12Resource>>& LoadTextures(DefaultScene::TexturesType type);
	};
}
