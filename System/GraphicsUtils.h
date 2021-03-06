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

	BoundingBox Bound;
	bool Enable;
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
	Axis = 5,
	Line = 6,
	PostProcess = 7,
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

//	天空盒设置	大气渲染
struct SkyBoxSetting
{
	Color Tint;
	float Exposure;
	float Rotation;
	float ACES;

	float EnableScatteringSky = 1;
	float SunHeight = 0.4f;
	Vector4 SkyColor = Vector4(0.39f, 0.57f, 1.0f, 1.0f);
	float ZenithOffset = 0.1;
	float Density = 0.7f;
	float MultiScatterPhase = 0.1;
	float AnisotropicIntensity = 0;
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

//	相机设置
struct CameraSetting
{
	float mCamFov = 45;
	float mCamClipN = 0.3;
	float mCamClipF = 1000;
	bool renderSkyBox = true;
	float SolidColor[4] = { 0,0,0,1 };
};

struct FogSetting
{
	Vector4 FogColor = Vector4{ 1, 1, 1, 0};
	//	Linear Fog
	float fogStrat = 0.0f;
	float fogEnd = 5.0f;
	float FogDensity = 0.08f;

	float EnableHeightFog = 0;
	//	Height Fog
	float FogFeather = 0.5f;
	float FogStep = 0.5f;
	float HeightMin = 0;
	float HeightMax = 5;

	float NoiseStrength = 1.0f;
	Vector3 CubePos = { 0,8,-5 };
	Vector4 CubeScale = { 8,8,8,1 };
	Vector4 CameraDir = { 0,0,8,1 };
};

struct AntialiasingSettings
{
	float AbsoluteLumaThreshold = 0.08f;
	float RelativeLumaThreshold = 0.25f;
	float ConsoleCharpness = 4;
	float DebugMode = 0;
};

struct PostProcessSwitch
{
	bool ShowVolumeFog = false;
	bool ShowFxAA = true;
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

	static void IMGUIInit(
		HWND& mhMainWnd,
		Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice,
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap
	);
};
