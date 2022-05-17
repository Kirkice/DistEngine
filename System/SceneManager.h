#pragma once
#include "../Component/MeshRender.h"
#include "../Component/DirectionLight.h"
#include "../Component/Camera.h"
#include "GraphicsUtils.h"
#include "GameTimer.h"
#include "d3dUtil.h"
#include "Texture2D.h"
#include "FrameResource.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class SceneManager
{
public:
	SceneManager(SceneType type);
	~SceneManager();

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
	void BuildScene(std::unordered_map<std::string, std::unique_ptr<Texture2D>>& mResourcesTextures);

	//	更新场景的Material Buffer
	void UpdateSceneMaterialBuffer(UploadBuffer<PBRMaterialData>* PBRMaterialBuffer, UploadBuffer<SkyBoxMaterialData>* SkyMaterialBuffer);

	void BuildRenderItem(
		std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count], 
		std::vector<std::unique_ptr<RenderItem>>& mAllRitems,
		Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, 
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList
	);

private:
	void BuildDefaultScene(std::unordered_map<std::string, std::unique_ptr<Texture2D>>& mResourcesTextures);
	void BuildConelBoxScene(std::unordered_map<std::string, std::unique_ptr<Texture2D>>& mResourcesTextures);
	void BuildToonScene(std::unordered_map<std::string, std::unique_ptr<Texture2D>>& mResourcesTextures);
	void BuildWaterScene(std::unordered_map<std::string, std::unique_ptr<Texture2D>>& mResourcesTextures);

	void UpdateDefaultSceneMaterialBuffer(UploadBuffer<PBRMaterialData>* PBRMaterialBuffer, UploadBuffer<SkyBoxMaterialData>* SkyMaterialBuffer);
	void UpdateConelBoxSceneMaterialBuffer(UploadBuffer<PBRMaterialData>* PBRMaterialBuffer, UploadBuffer<SkyBoxMaterialData>* SkyMaterialBuffer);
	void UpdateToonSceneMaterialBuffer(UploadBuffer<PBRMaterialData>* PBRMaterialBuffer, UploadBuffer<SkyBoxMaterialData>* SkyMaterialBuffer);
	void UpdateWaterSceneMaterialBuffer(UploadBuffer<PBRMaterialData>* PBRMaterialBuffer, UploadBuffer<SkyBoxMaterialData>* SkyMaterialBuffer);
};
