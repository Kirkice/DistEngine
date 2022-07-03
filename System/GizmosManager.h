#pragma once
#include "../Component/MeshRender.h"
#include "GraphicsUtils.h"
#include "GameTimer.h"
#include "FrameResource.h"
#include "d3dUtil.h"
#include "Texture2D.h"

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
	std::vector<std::unique_ptr<MeshRender>> mMeshRender;

public:

	//	�������� �õ�Gizmos������
	void BuildScene(
		std::unordered_map<std::string, std::unique_ptr<Texture2D>>& mGizmosTextures,
		MaterialIndexUtils& matCBIndexUtils
		);

	//	���³�����Material Buffer
	void UpdateSceneMaterialBuffer(UploadBuffer<PBRMaterialData>* PBRMaterialBuffer);

	//	����RenderItem
	void BuildRenderItem(
		std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count],
		std::vector<std::unique_ptr<RenderItem>>& mAllRitems,
		Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice,
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList
	);

	//	����CBuffer
	void UpdateObjectBuffer(std::vector<std::unique_ptr<RenderItem>>& mAllRitems, DirectionLight& mMainLight);
};