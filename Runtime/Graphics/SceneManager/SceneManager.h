#pragma once
#include "../Component/MeshRender.h"
#include "../Component/DirectionLight.h"
#include "../Component/Camera.h"
#include "../Component/Transform.h"
#include "../Component/GameObject.h"
#include "../Physics/BoundingBox.h"
#include "../Graphics/GraphicsUtils.h"
#include "../File/Texture2D.h"
#include "../Graphics/FrameResource.h"
#include "../Geometry/FbxLoader.h"
#include "../Geometry/ObjLoader.h"
#include "../Serialization/JsonManager.h"
#include "GameTimer.h"
#include "DX12Utils.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class SceneManager
{

public:
	SceneManager() = default;
	~SceneManager() = default;
	SceneManager(SceneType type);

	static SceneManager& getInstance() {
		static SceneManager instance;
		return instance;
	}


public:
	//	��������
	SceneType Type; 

	//	����Դ
	GameObject* MainLight = new GameObject("MainLight");

	//	Mesh Render	
	std::vector < std::unique_ptr<GameObject>> mRenderObjects;

	//	��պ����� 
	SkyBoxSetting mSkyBoxSetting;

	//	�ƹ�����
	LightSetting mLightSetting;

	//	�������
	CameraSetting mCameraSetting;

public:

	void BuildScene(
		std::unordered_map<std::string, std::unique_ptr<Texture2D>>& mResourcesTextures,
		MaterialIndexUtils& matCBIndexUtils
	);

	//	���³�����Material Buffer
	void UpdateSceneMaterialBuffer(
		UploadBuffer<PBRMaterialData>* PBRMaterialBuffer, 
		UploadBuffer<SkyBoxMaterialData>* SkyMaterialBuffer,
		MaterialIndexUtils& matCBIndexUtils
	);

	void BuildRenderItem(
		std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count],
		std::vector<std::unique_ptr<RenderItem>>& mAllRitems,
		Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice,
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList,
		UINT CurrentSize,
		MaterialIndexUtils& matCBIndexUtils
	);

	void UpdateObjectBuffer(std::vector<std::unique_ptr<RenderItem>>& mAllRitems, UINT CurrentSize);

private:

	void BuildGlobalConfig();

	void BuildMainLight();

	void BuildSkyBox(MaterialIndexUtils& matCBIndexUtils);

	void BuildRenderObejct(std::unordered_map<std::string, std::unique_ptr<Texture2D>>& mResourcesTextures, MaterialIndexUtils& matCBIndexUtils);

	void BuildDefaultScene(
		std::unordered_map<std::string, std::unique_ptr<Texture2D>>& mResourcesTextures,
		MaterialIndexUtils& matCBIndexUtils
	);
	void BuildConelBoxScene(
		std::unordered_map<std::string, std::unique_ptr<Texture2D>>& mResourcesTextures,
		MaterialIndexUtils& matCBIndexUtils
	);
	void BuildToonScene(
		std::unordered_map<std::string, std::unique_ptr<Texture2D>>& mResourcesTextures,
		MaterialIndexUtils& matCBIndexUtils
	);
	void BuildWaterScene(
		std::unordered_map<std::string, std::unique_ptr<Texture2D>>& mResourcesTextures,
		MaterialIndexUtils& matCBIndexUtils
	);

	void UpdateDefaultSceneMaterialBuffer(
		UploadBuffer<PBRMaterialData>* PBRMaterialBuffer, 
		UploadBuffer<SkyBoxMaterialData>* SkyMaterialBuffer,
		MaterialIndexUtils& matCBIndexUtils
	);
	void UpdateConelBoxSceneMaterialBuffer(
		UploadBuffer<PBRMaterialData>* PBRMaterialBuffer, 
		UploadBuffer<SkyBoxMaterialData>* SkyMaterialBuffer,
		MaterialIndexUtils& matCBIndexUtils
	);
	void UpdateToonSceneMaterialBuffer(
		UploadBuffer<PBRMaterialData>* PBRMaterialBuffer, 
		UploadBuffer<SkyBoxMaterialData>* SkyMaterialBuffer,
		MaterialIndexUtils& matCBIndexUtils
	);
	void UpdateWaterSceneMaterialBuffer(
		UploadBuffer<PBRMaterialData>* PBRMaterialBuffer, 
		UploadBuffer<SkyBoxMaterialData>* SkyMaterialBuffer,
		MaterialIndexUtils& matCBIndexUtils
	);
};
