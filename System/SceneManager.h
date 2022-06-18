#pragma once
#include "../Component/MeshRender.h"
#include "../Component/DirectionLight.h"
#include "../Component/Camera.h"
#include "GraphicsUtils.h"
#include "GameTimer.h"
#include "d3dUtil.h"
#include "Texture2D.h"
#include "FrameResource.h"
#include "FbxLoader.h"
#include "ObjLoader.h"

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
	//	场景类型
	SceneType Type;

	//	主光源
	DirectionLight mMainLight;

	//	Mesh Render	
	std::vector<std::unique_ptr<MeshRender>> mMeshRender;

	//	天空盒设置 
	SkyBoxSetting mSkyBoxSetting;

	//	灯光设置
	LightSetting mLightSetting;

public:

	void BuildScene(
		std::unordered_map<std::string, std::unique_ptr<Texture2D>>& mResourcesTextures,
		MaterialIndexUtils& matCBIndexUtils
	);

	//	更新场景的Material Buffer
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

private:
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
