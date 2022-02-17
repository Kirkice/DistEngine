#pragma once
#include "Mathf.h"
#include "Material.h"
#include "MeshGeometry.h"
#include "Light.h"
#include "d3dApp.h"
#include "Camera.h"
#include "FrameResource.h"
#include "ShadowMap.h"
#include "SkinnedData.h"
#include "LoadM3d.h"
#include "Ssao.h"
#include "FbxLoader.h"
#include "PhysicsCore.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Renderer;

namespace Renderer
{
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
		Count
	};

	class RenderCore : public PhysicsCore
	{
	public:
		RenderCore(HINSTANCE hInstance);
		RenderCore(const RenderCore& rhs) = delete;
		RenderCore& operator=(const RenderCore& rhs) = delete;
		~RenderCore();

	public:
		void Bounding_BuildMaterials(std::unordered_map<std::string, std::unique_ptr<Material>>& mMaterials);
		void Bounding_BuildRenderItems(
			std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count],
			std::unordered_map<std::string, std::unique_ptr<Material>>& mMaterials,
			std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& mGeometries,
			std::vector<std::unique_ptr<RenderItem>>& mAllRitems);
		void Bounding_UpdateObjectBuffer(UINT ObjCBIndex, XMFLOAT4X4* eWorldMatrix);


		//Editor Gizmo
		void EditorGizmo_LoadTextures(std::unordered_map<std::string, std::unique_ptr<Texture>>& mTextures);
		void EditorGizmo_BuildDescriptorHeaps(std::vector<ComPtr<ID3D12Resource>>& tex2DList, std::unordered_map<std::string, std::unique_ptr<Texture>>& mTextures);
		void EditorGizmo_BuildMaterials(std::unordered_map<std::string, std::unique_ptr<Material>>& mMaterials);
		void EditorGizmo_BuildRenderItems(
			std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count],
			std::unordered_map<std::string, std::unique_ptr<Material>>& mMaterials,
			std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& mGeometries,
			std::vector<std::unique_ptr<RenderItem>>& mAllRitems);
		void EditorGizmo_UpdateObjectBuffer(UINT ObjCBIndex,XMFLOAT4X4* eWorldMatrix);

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
		   
		//Post processing 
		void Post_UpdateMaterialBuffer(PostProcessingMat* mat, UploadBuffer<PostprocessingData>* currMaterialBuffer);
		void Post_BuildMaterials(std::unordered_map<std::string, std::unique_ptr<PostProcessingMat>>& mMaterials);


		//Lighting Settings
		float EnvironmentShadowColor[3] = { 0.3f,0.3f,0.3f };
		float SkyBoxTintColor[3] = { 1,1,1 };
		float SkyBoxExposure = 1;
		float SkyBoxRotation = 0;
		float  ACES = 1;

		//Light
		//Direction Lights
		XMFLOAT3 mRotatedLightDirections;
		XMFLOAT3 mDirectionLightsDir = { 0.57f,-0.57f,0.57f };
		XMFLOAT3 mDirectionLightsPos = { -2,5,0 };
		float mDirectionLightsColor[3] = { 1,1,1 };
		float mDirectionLightsAngle[3] = { 0,0,0 };
		float mDirectionLightsScale[3] = { 1,1,1 };
		float mDirectionLightsStrength = 1.2f;
		bool mDirectionLightsCastShadow = true;
		bool mDirectionLightsActive = true;

		//Point Lights
		XMFLOAT3 mPointLightsPos = { 1.5f,2,0 };
		float mPointLightsColor[3] = { 1,1,1 };
		float mPointLightsRange = 3.0;
		float mPointLightsRot[3] = { 0,0,0 };
		float mPointLightsScale[3] = { 1,1,1 };
		float mPointLightsStrength = 3;
		bool mPointLightsActive = false;

		//SpotLights
		XMFLOAT3 mRotatedLightSpots;
		XMFLOAT3 mSpotLightsDir = { 0.57f,-0.57f,0.57f };
		XMFLOAT3 mSpotLightsPos = { 0,0,0 };
		float mSpotLightsColor[3] = { 1,1,1 };
		float mSpotLightsAngle[3] = { 0,0,0 };
		float mSpotLightsScale[3] = { 1,1,1 };
		float mSpotLightsRange = 15.0;
		float mSpotLightsStrength = 1;
		float mSpotLightsCutAngle = 30;
		bool mSpotLightsActive = true;
	};
}