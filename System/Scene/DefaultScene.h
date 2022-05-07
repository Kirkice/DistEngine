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

		//	��Ҫ���õ�InputLayout
		std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

		//----------------
		//	��������Դ
		//----------------
		//	������ǩ��
		ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
		//	SSAO��ǩ��
		ComPtr<ID3D12RootSignature> mSsaoRootSignature = nullptr; 
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
	};
}
