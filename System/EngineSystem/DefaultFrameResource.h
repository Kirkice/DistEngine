#pragma once
#include "SystemUtils.h"
#include "../../Core/Mathf/Mathf.h"
#include "../../Component/DirectionLight.h"
#include "../../Component/PointLight.h"
#include "../../Component/SpotLight.h"
#include "UploadBuffer.h"

namespace Dist
{
	struct ObjectConstants
	{
		DirectX::XMFLOAT4X4 World = Dist::Identity4x4();
		DirectX::XMFLOAT4X4 InvWorld = Dist::Identity4x4();
		DirectX::XMFLOAT4X4 TexTransform = Dist::Identity4x4();
		UINT     MaterialIndex;
		UINT     ObjPad0;
		UINT     ObjPad1;
		UINT     ObjPad2;
	};

	struct PassConstants
	{
		DirectX::XMFLOAT4X4 View = Dist::Identity4x4();
		DirectX::XMFLOAT4X4 InvView = Dist::Identity4x4();
		DirectX::XMFLOAT4X4 Proj = Dist::Identity4x4();
		DirectX::XMFLOAT4X4 InvProj = Dist::Identity4x4();
		DirectX::XMFLOAT4X4 ViewProj = Dist::Identity4x4();
		DirectX::XMFLOAT4X4 InvViewProj = Dist::Identity4x4();
		DirectX::XMFLOAT4X4 ViewProjTex = Dist::Identity4x4();
		DirectX::XMFLOAT4X4 ShadowTransform = Dist::Identity4x4();

		Vector3 EyePosW = { 0.0f, 0.0f, 0.0f };
		float cbPerObjectPad1 = 0.0f;
		Vector2 RenderTargetSize = { 0.0f, 0.0f };
		Vector2 InvRenderTargetSize = { 0.0f, 0.0f };
		float NearZ = 0.0f;
		float FarZ = 0.0f;
		float TotalTime = 0.0f;
		float DeltaTime = 0.0f;

		//Light
		DirectionLight directionLight;
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
		float OcclusionRadius = 0.5f;
		float OcclusionFadeStart = 0.2f;
		float OcclusionFadeEnd = 2.0f;
		float SurfaceEpsilon = 0.05f;
	};

	struct SkyBoxMaterialData
	{
		Color SkyBoxTint = Color(1.0f, 1.0f, 1.0f, 1.0f);
		float SkyBoxExposure = 1;
		float SkyBoxRotation = 0;
		float ACES = 1;
	};

	struct DefaultVertex
	{
		Vector3 Pos;
		Vector3 Normal;
		Vector2 TexC;
		Vector3 TangentU;
	};

	struct PBRMaterialData
	{
		Color DiffuseColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
		float Smoothness = 0.5f;
		float Metallic = 0.5f;
		float Occlusion = 0.0f;
		Color EmissionColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
		float EmissionStrength = 0.0f;

		// Used in texture mapping.
		DirectX::XMFLOAT4X4 MatTransform = Dist::Identity4x4();

		UINT DiffuseMapIndex = 0;
		UINT NormalMapIndex = 0;
		UINT MsoMapIndex = 0;
		UINT EmissionMapIndex = 0;
		UINT LUTMapIndex = 0;
	};

	struct DefaultFrameResource
	{
	public:

		DefaultFrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT skinnedObjectCount, UINT materialCount);
		DefaultFrameResource(const DefaultFrameResource& rhs) = delete;
		DefaultFrameResource& operator=(const DefaultFrameResource& rhs) = delete;
		~DefaultFrameResource();

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAlloc;

		std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
		std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;
		std::unique_ptr<UploadBuffer<SsaoConstants>> SsaoCB = nullptr;
		std::unique_ptr<UploadBuffer<PBRMaterialData>> PBRMaterialBuffer = nullptr;
		std::unique_ptr<UploadBuffer<SkyBoxMaterialData>> SkyBoxMaterialBuffer = nullptr;
		UINT64 Fence = 0;
	};
}