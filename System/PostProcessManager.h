#pragma once
#include "../Component/MeshRender.h"
#include "../Component/DirectionLight.h"
#include "../Component/Camera.h"
#include "GraphicsUtils.h"
#include "GameTimer.h"
#include "d3dUtil.h"
#include "FrameResource.h"

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
	std::vector<std::unique_ptr<MeshRender>> mMeshRender;

public:

	void Build(MaterialIndexUtils& matCBIndexUtils);

	//	更新场景的Material Buffer
	void UpdateMaterialBuffer(
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
};