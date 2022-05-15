#pragma once
#include "../Component/Material.h"
#include "MeshGeometry.h" 
#include "d3dUtil.h"
#include "Mathf/Mathf.h"
using namespace Mathf;

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

	PBRMaterial* Mat = nullptr;
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

