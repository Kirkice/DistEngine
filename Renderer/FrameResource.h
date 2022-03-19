#pragma once

#include "d3dUtil.h"
#include "Mathf.h"
#include "UploadBuffer.h"
#include "Light.h"

using namespace Mathf;
using namespace Renderer;

struct ObjectConstants
{
    DirectX::XMFLOAT4X4 World = Mathf::Identity4x4();
    DirectX::XMFLOAT4X4 InvWorld = Mathf::Identity4x4();
	DirectX::XMFLOAT4X4 TexTransform = Mathf::Identity4x4();
	UINT     MaterialIndex;
	UINT     ObjPad0;
	UINT     ObjPad1;
	UINT     ObjPad2;
};

struct SkinnedConstants
{
    DirectX::XMFLOAT4X4 BoneTransforms[96];
};

struct PassConstants
{
    DirectX::XMFLOAT4X4 View = Mathf::Identity4x4();
    DirectX::XMFLOAT4X4 InvView = Mathf::Identity4x4();
    DirectX::XMFLOAT4X4 Proj = Mathf::Identity4x4();
    DirectX::XMFLOAT4X4 InvProj = Mathf::Identity4x4();
    DirectX::XMFLOAT4X4 ViewProj = Mathf::Identity4x4();
    DirectX::XMFLOAT4X4 InvViewProj = Mathf::Identity4x4();
    DirectX::XMFLOAT4X4 ViewProjTex = Mathf::Identity4x4();
    DirectX::XMFLOAT4X4 ShadowTransform = Mathf::Identity4x4();

    Mathf::Vector3 EyePosW = { 0.0f, 0.0f, 0.0f };
    float cbPerObjectPad1 = 0.0f;
    Mathf::Vector2 RenderTargetSize = { 0.0f, 0.0f };
    Mathf::Vector2 InvRenderTargetSize = { 0.0f, 0.0f };
    float NearZ = 0.0f;
    float FarZ = 0.0f;
    float TotalTime = 0.0f;
    float DeltaTime = 0.0f;

    //Light
    DirectionLight DirectionLights;
    PointLight PointLights;
    SpotLight SpotLights;
};

struct SsaoConstants
{
    DirectX::XMFLOAT4X4 Proj;
    DirectX::XMFLOAT4X4 InvProj;
    DirectX::XMFLOAT4X4 ProjTex;
    DirectX::XMFLOAT4 OffsetVectors[14];

    // For SsaoBlur.hlsl
    DirectX::XMFLOAT4 BlurWeights[3];

    XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };

    // Coordinates given in view space.
    float OcclusionRadius  = 0.5f;
    float OcclusionFadeStart = 0.2f;
    float OcclusionFadeEnd = 2.0f;
    float SurfaceEpsilon = 0.05f;
};

struct SkyBoxMaterialData
{
	Vector4 SkyBoxTint = { 1.0f, 1.0f, 1.0f, 1.0f };
	float SkyBoxExposure = 1;
	float SkyBoxRotation = 0;
	float ACES = 1;
};

struct PBRMaterialData
{
    Vector4 BaseColor = (1, 1, 1, 1);
    float Smoothness = 0.5f;
    float Metallic = 0.5f;
    float Occlusion = 0.0f;
    Vector4 EmissionColor = (0,0,0,1);
    float EmissionStrength = 0.0f;
    float ReceiveShadow = 0.0f;

	// Used in texture mapping.
	DirectX::XMFLOAT4X4 MatTransform = Mathf::Identity4x4();

	UINT DiffuseMapIndex = 0;
	UINT NormalMapIndex = 0;
	UINT MsoMapIndex = 0;
	UINT EmissionMapIndex = 0;
    UINT LUTMapIndex = 0;
};

struct NPRMaterialData
{
	DirectX::XMFLOAT4X4 MatTransform = Mathf::Identity4x4();

	float OutLineWidth = 0.5f;
	DirectX::XMFLOAT4 OutLineColor = { 0, 0, 0, 1 };

	//��Ӱ
	float shadowRange_1st = 0.5f;
	float shadowFeather_1st = 0.5f;
	DirectX::XMFLOAT4 shadowColor_1st = { 1,1,1,1 };
	float shadowRange_2nd = 0.5f;
	float shadowFeather_2nd = 0.5f;
	DirectX::XMFLOAT4 shadowColor_2nd = { 1,1,1,1 };

	//��Ե��
	float rimRange = 0.5f;
	float rimFeather = 0.5f;
	DirectX::XMFLOAT4 rimColor = { 1,1,1,1 };

	//�Է���
	float emissStrength = 0.5f;
	DirectX::XMFLOAT4 emissColor = { 1,1,1,1 };

	//�߹�
	float specularRange = 0.5;
	float specularFeather = 0.5f;
	DirectX::XMFLOAT4 specularColor = { 1,1,1,1 };

	//������Ӱ��ɫ
	DirectX::XMFLOAT4 LightShadowColor = { 1,1,1,1 };
};

struct PostprocessingData
{
    //RGBSplit   Decolourize (Turn Gray)
    float Strength = 0.02f;

    //Bloom
    float Threshold = 0.3f;
    float SoftKnee = 0.1f;
    float Radius = 3.5f;
    float Intensity = 0.5f;

    //Brightness And Contrast
    float Brightness = 1.2f;
    float Contrast = 1.0f;

    //HSV
    float Hue = 0;
    float Saturation = 0;
    float Value = 0;

    //Mosaic
    float PixelSize = 0.01f;

    //Radial Blur
    float BlurFactory = 0.01f;

    //EdgeDetection
    float EdgeWdith = 1.5f;
    float EdgeNeonFade = 1.0f;
    DirectX::XMFLOAT4 OutLineColor = {0,1.3f,2.6f,1};
    float BackgroundFade = 0;
    DirectX::XMFLOAT4 BackgroundColor = {0,0.35f,1,1};

    //WhiteBalance
    float Temperature = 0;
    float Tint = 0;

    //Sharpen
    //Vignette
    DirectX::XMFLOAT4 VignetteColor = {0,0,0,1};

    //Oil Paint
    float ResoultionValue = 2.0f;

	//Vignette
    float Width = 1.4f;
	float Height = 0.7f;
	float Smooth = 10;
	float Alpha = 1;
};

struct Vertex
{
    XMFLOAT3 Pos;
    XMFLOAT3 Normal;
    XMFLOAT2 TexC;
    XMFLOAT3 TangentU;
};

struct SkinnedVertex
{
    XMFLOAT3 Pos;
    XMFLOAT3 Normal;
    XMFLOAT2 TexC;
    XMFLOAT3 TangentU;
    XMFLOAT3 BoneWeights;
    BYTE BoneIndices[4];
};

struct FrameResource
{
public:
    
    FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT skinnedObjectCount, UINT materialCount);
    FrameResource(const FrameResource& rhs) = delete;
    FrameResource& operator=(const FrameResource& rhs) = delete;
    ~FrameResource();

    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAlloc;

    std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
    std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;
    std::unique_ptr<UploadBuffer<SkinnedConstants>> SkinnedCB = nullptr;
    std::unique_ptr<UploadBuffer<SsaoConstants>> SsaoCB = nullptr;
	std::unique_ptr<UploadBuffer<PBRMaterialData>> PBRMaterialBuffer = nullptr;
    std::unique_ptr<UploadBuffer<SkyBoxMaterialData>> SkyBoxMaterialBuffer = nullptr;
    std::unique_ptr<UploadBuffer<PostprocessingData>> PostMaterialBuffer = nullptr;
    UINT64 Fence = 0;
};