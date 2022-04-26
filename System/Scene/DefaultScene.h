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
		//	���ص�ͼƬ����
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
		//	�����ṹ����
		//----------------
		//	����Դ
		DirectionLight mMainLight;

		//	Mesh Render	
		std::vector<std::unique_ptr<PBRMeshRender>> mMeshRender;

		//	Camera
		Camera mCamera;

		//	SkyBox Textures
		std::wstring mSkyBoxTexPath;

		//----------------
		//	������Դ����
		//----------------
		//	��Ҫ���ص�Shader
		std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;
		//	��Ҫ���ص�����
		std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
		//	��Ҫ���ص�CubeMap����
		std::unordered_map<std::string, std::unique_ptr<Texture>> mSkyTextures;
		//	��Ҫ���ص�Gizmo����
		std::unordered_map<std::string, std::unique_ptr<Texture>> mGizmoTextures;
		//	��Ҫ���õ�InputLayout
		std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

		//----------------
		//	��������Դ
		//----------------
		//	������ǩ��
		ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
		//	SSAO��ǩ��
		ComPtr<ID3D12RootSignature> mSsaoRootSignature = nullptr; 
		//	����SRV
		ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;
		//	��Ⱦ��
		std::vector<std::unique_ptr<PBRRenderItem>> mAllRitems;
		//	��ȾPSO�㼶
		std::vector<PBRRenderItem*> mRitemLayer[(int)RenderLayer::Count];
		//	����֡��Դ
		std::vector<std::unique_ptr<DefaultFrameResource>> mFrameResources;
		DefaultFrameResource* mCurrFrameResource = nullptr;
		int mCurrFrameResourceIndex = 0;

		//	������Χ��
		DirectX::BoundingSphere mSceneBounds;
	public:
		DefaultScene();
		~DefaultScene();

		//	��ʼ������
		void InitScene(Microsoft::WRL::ComPtr<ID3D12Device> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList);

		//	���³���
		void UpdateScene(const GameTimer& gt);

	private:

		//	���������ƹ�
		void BuildLights();

		//	�����������
		void BuildCamera();

		//	������ǩ��
		void BuildRootSignature(Microsoft::WRL::ComPtr<ID3D12Device> device);

		//	����SSAO��ǩ��
		void BuildSsaoRootSignature(Microsoft::WRL::ComPtr<ID3D12Device> device);

		//	������ɫ��
		void BuildShadersAndInputLayout();

		//	���µƹ�
		void UpdateLights(const GameTimer& gt);

		//	��������CB
		void UpdateObjectCBs(const GameTimer& gt);

		//	���²���
		void UpdateMaterials(const GameTimer& gt);

		//	����MeshRender
		void LoadMeshRender();

		//	������Ⱦ��Ŀ
		void LoadRenderItem(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList);

		//	����ͼƬ·��
		std::vector<ComPtr<ID3D12Resource>>& LoadTextures(DefaultScene::TexturesType type);
	};
}
