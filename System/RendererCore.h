#pragma once
#include "../Core/Mathf/Mathf.h"
#include "Material.h"
#include "MeshGeometry.h"
#include "d3dApp.h"
#include "Camera.h"
#include "FrameResource.h"
#include "ShadowMap.h"
#include "Ssao.h"
#include "InputSystem.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

struct RenderItem
{
	RenderItem() = default;
	RenderItem(const RenderItem& rhs) = delete;

	//World Matrix
	XMFLOAT4X4 World = Mathf::Identity4x4();
	//Tex Transform
	XMFLOAT4X4 TexTransform = Mathf::Identity4x4();

	int NumFramesDirty = gNumFrameResources;

	UINT ObjCBIndex = -1;

	Material* Mat = nullptr;
	MeshGeometry* Geo = nullptr;

	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation = 0;
};

//Render Layer
enum class RenderLayer : int
{
	Opaque = 0,
	Transparent = 1,
	Debug = 2,
	Sky = 3,
	Gizmo = 4,
	Unlit = 5,
	Bounding = 6,
	PostProcessing = 7,
	Count
};

class RenderCore : public InputSystem
{
public:
	RenderCore(HINSTANCE hInstance);
	RenderCore(const RenderCore& rhs) = delete;
	RenderCore& operator=(const RenderCore& rhs) = delete;
	~RenderCore();

public:

	//SkyBox
	void SkyBox_UpdateMaterialBuffer(Material* mat, UploadBuffer<SkyBoxMaterialData>* currMaterialBuffer);
	void SkyBox_LoadTextures(std::unordered_map<std::string, std::unique_ptr<Texture>>& mSkyTextures);
	void SkyBox_BuildDescriptorHeaps(ComPtr<ID3D12Resource>& skyCubeMap, ComPtr<ID3D12Resource>& diffuseIBL, std::unordered_map<std::string, std::unique_ptr<Texture>>& mSkyTextures);
	void SkyBox_BuildMaterials(std::unordered_map<std::string, std::unique_ptr<Material>>& mMaterials);
	void SkyBox_BuildRenderItems(
		std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count],
		std::unordered_map<std::string, std::unique_ptr<Material>>& mMaterials,
		std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& mGeometries,
		std::vector<std::unique_ptr<RenderItem>>& mAllRitems);

	//PBR Demo
	void PBRDemo_UpdateMaterialBuffer(Material* mat, UploadBuffer<PBRMaterialData>* currMaterialBuffer);
	void PBRDemo_LoadTextures(std::unordered_map<std::string, std::unique_ptr<Texture>>& mTextures);
	void PBRDemo_BuildDescriptorHeaps(std::vector<ComPtr<ID3D12Resource>>& tex2DList, std::unordered_map<std::string, std::unique_ptr<Texture>>& mTextures);
	void PBRDemo_BuildMaterials(std::unordered_map<std::string, std::unique_ptr<Material>>& mMaterials);
	void PBRDemo_BuildRenderItems(
		std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count],
		std::unordered_map<std::string, std::unique_ptr<Material>>& mMaterials,
		std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& mGeometries,
		std::vector<std::unique_ptr<RenderItem>>& mAllRitems);
	void PBRDemo_UpdateObjectBuffer();
};