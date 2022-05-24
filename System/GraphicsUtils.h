#pragma once
#include "FrameResource.h"
#include "../Component/MeshFliter.h"
#include "../Component/Material.h"
#include "MeshGeometry.h" 
#include "d3dUtil.h"
#include "Mathf/Mathf.h"
#include "ResourcesManager.h"

using namespace Mathf;

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

//	场景类型
enum class SceneType
{
	Default,
	ConelBox,
	Toon,
	Water,
};

//	天空盒设置
struct SkyBoxSetting
{
	Color Tint;
	float Exposure;
	float Rotation;
	float ACES;
};

//	灯光设置
struct LightSetting
{
	Color ShadowColor;
	Color AmbientColor;
	float UseLinearFog;
	float Start;
	float End;
	float UseHieghtFog;
	float HeightMin;
	float HeightMax;
	float Feather;
	float Step;
};

class GraphicsUtils
{
public:
	static MeshGeometry* BuidlMeshGeometryFromMeshData(std::string name, MeshData data, Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList);

	static void BuildTextureCubeSrvDesc(
		Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice,
		UINT mCbvSrvUavDescriptorSize,
		CD3DX12_CPU_DESCRIPTOR_HANDLE& CPUDescriptor,
		CD3DX12_GPU_DESCRIPTOR_HANDLE& GPUDescriptor,
		std::unordered_map<std::string, std::unique_ptr<TextureCube>>& mCubeMapTextures,
		std::string TexName
	);
};
