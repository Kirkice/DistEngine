#pragma once
#include "../Component/MeshRender.h"
#include "../Component/DirectionLight.h"
#include "../Component/Camera.h"
#include "../Component/Transform.h"
#include "../Component/GameObject.h"
#include "../Physics/BoundingBox.h"
#include "../Graphics/GraphicsUtils.h"
#include "GameTimer.h"
#include "DX12Utils.h"
#include "../Graphics/FrameResource.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class PostProcessManager
{
public:
	PostProcessManager() = default;
	~PostProcessManager() = default;
	PostProcessManager(SceneType type);

	static PostProcessManager& getInstance() {
		static PostProcessManager instance;
		return instance;
	}

public:
	//	Mesh Render	
	std::vector < std::unique_ptr<GameObject>> mRenderObjects;

public:

	void Build(MaterialIndexUtils& matCBIndexUtils);

	//	更新场景的Material Buffer
	void UpdateMaterialBuffer(
		UploadBuffer<PostprocessingData>* PostProcessBuffer,
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
};