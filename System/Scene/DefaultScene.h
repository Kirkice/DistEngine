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
		//	���ص�ͼƬ����
		enum TexturesType
		{
			RenderItem,
			Gizom,
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
		// ��Ⱦ��
		std::vector<std::unique_ptr<PBRRenderItem>> mAllRitems;

	public:
		DefaultScene();
		~DefaultScene();

		//	��ʼ������
		void InitScene(Microsoft::WRL::ComPtr<ID3D12Device> device);

		//	���³���
		void UpdateScene();

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

		//	������Ⱦ��
		void BuildRenderItems();

		//	���µƹ�
		void UpdateLights(const GameTimer& gt);

		//	��������CB
		void UpdateObjectCBs(const GameTimer& gt);

		//	���²���
		void UpdateMaterials(const GameTimer& gt);

		//	������Ӱλ��
		void UpdateShadowTransform(const GameTimer& gt);

		//	����Pass CB
		void UpdateMainPassCB(const GameTimer& gt);

		//	����Shadow CB
		void UpdateShadowPassCB(const GameTimer& gt);

		//	����SSAO CB
		void UpdateSsaoCB(const GameTimer& gt);

		//	����MeshRender
		void LoadMeshRender();

		//	������Ⱦ��Ŀ
		void LoadRenderItem();

		//	����ͼƬ·��
		std::vector<ComPtr<ID3D12Resource>>& LoadTextures(DefaultScene::TexturesType type);
	};
}
