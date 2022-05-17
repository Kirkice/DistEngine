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
	std::vector<std::unique_ptr<PBRMeshRender>> mMeshRender;
	std::vector<std::unique_ptr<SkyBoxMeshRender>> mSkyBoxMeshRender;

	//	天空盒设置
	SkyBoxSetting mSkyBoxSetting;

	//	灯光设置
	LightSetting mLightSetting;

public:
	//	更新场景的Material Buffer
	void UpdateSceneMaterialBuffer(UploadBuffer<PBRMaterialData>* PBRMaterialBuffer, UploadBuffer<SkyBoxMaterialData>* SkyMaterialBuffer);

	void BuildRenderItem(
		std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count], 
		std::vector<std::unique_ptr<RenderItem>>& mAllRitems,
		Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, 
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList
	);

private:
	void BuildScene();
	void BuildDefaultScene();
	void BuildConelBoxScene();
	void BuildToonScene();
	void BuildWaterScene();

	void UpdateDefaultSceneMaterialBuffer(UploadBuffer<PBRMaterialData>* PBRMaterialBuffer, UploadBuffer<SkyBoxMaterialData>* SkyMaterialBuffer);
	void UpdateConelBoxSceneMaterialBuffer(UploadBuffer<PBRMaterialData>* PBRMaterialBuffer, UploadBuffer<SkyBoxMaterialData>* SkyMaterialBuffer);
	void UpdateToonSceneMaterialBuffer(UploadBuffer<PBRMaterialData>* PBRMaterialBuffer, UploadBuffer<SkyBoxMaterialData>* SkyMaterialBuffer);
	void UpdateWaterSceneMaterialBuffer(UploadBuffer<PBRMaterialData>* PBRMaterialBuffer, UploadBuffer<SkyBoxMaterialData>* SkyMaterialBuffer);
};
