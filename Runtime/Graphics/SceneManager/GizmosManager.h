#pragma once
#include "../Component/DirectionLight.h"
#include "../Component/MeshRender.h"
#include "../Component/Transform.h"
#include "../Component/GameObject.h"
#include "../Physics/BoundingBox.h"
#include "../Graphics/GraphicsUtils.h"
#include "../Graphics/FrameResource.h"
#include "../Serialization/JsonManager.h"
#include "../File/Texture2D.h"
#include "../Geometry/ObjLoader.h"
#include "GameTimer.h"
#include "DX12Utils.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class GizmosManager
{
public:
	GizmosManager() = default;
	~GizmosManager() = default;

	static GizmosManager& getInstance() {
		static GizmosManager instance;
		return instance;
	}

public:
	std::vector < std::unique_ptr<GameObject>> mRenderObjects;

public:

	//	构建场景 拿到Gizmos的纹理
	void BuildScene(
		std::unordered_map<std::string, std::unique_ptr<Texture2D>>& mGizmosTextures,
		MaterialIndexUtils& matCBIndexUtils
		);

	//	更新场景的Material Buffer
	void UpdateSceneMaterialBuffer(UploadBuffer<PBRMaterialData>* PBRMaterialBuffer);

	//	构建RenderItem
	void BuildRenderItem(
		std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count],
		std::vector<std::unique_ptr<RenderItem>>& mAllRitems,
		Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice,
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList
	);

	//	更新CBuffer
	void UpdateObjectBuffer(std::vector<std::unique_ptr<RenderItem>>& mAllRitems, Transform* mMainLightPos, std::unique_ptr<GameObject>& mTargetRender);

	void ShowPosition();
	void ShowRotation();
	void ShowScale();

	bool PositionUCSEnable = false;
	bool RotationUCSEnable = false;
	bool ScaleUCSEnable = false;

private:

	void BuildWirePlane(MaterialIndexUtils& matCBIndexUtils);

	void BuildLightGizmo(std::unordered_map<std::string, std::unique_ptr<Texture2D>>& mGizmosTextures,
		MaterialIndexUtils& matCBIndexUtils);

	void BuildUCSPosition(MaterialIndexUtils& matCBIndexUtils);

	void BuildUCSRotation(MaterialIndexUtils& matCBIndexUtils);

	void BuildUCSScale(MaterialIndexUtils& matCBIndexUtils);
};
