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
		DirectionLight DirectionLights[4];
		PointLight PointLights[4];
		SpotLight SpotLights[4];
	};

	struct PBRMaterialData
	{
		Color DiffuseColor = (1, 1, 1, 1);
		float Smoothness = 0.5f;
		float Metallic = 0.5f;
		float Occlusion = 0.0f;
		Color EmissionColor = (0, 0, 0, 1);
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
		std::unique_ptr<UploadBuffer<PBRMaterialData>> PBRMaterialBuffer = nullptr;
		UINT64 Fence = 0;
	};
}