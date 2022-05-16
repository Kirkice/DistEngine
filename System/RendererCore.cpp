#include "UploadBuffer.h"
#include "GeometryGenerator.h"
#include "RendererCore.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

const int GizmoCount = 14;

RenderCore::RenderCore(HINSTANCE hInstance) : InputSystem(hInstance)
{
}

RenderCore::~RenderCore()
{
	if (md3dDevice != nullptr)
		FlushCommandQueue();
}

//--------------------	Sky Box	--------------------
void RenderCore::SkyBox_UpdateMaterialBuffer(Material* mat, UploadBuffer<SkyBoxMaterialData>* currMaterialBuffer)
{
	XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);
	SkyBoxMaterialData matData;
	matData.SkyBoxTint = mat->SkyBoxTint;
	matData.SkyBoxExposure = mat->SkyBoxExposure;
	matData.SkyBoxRotation = mat->SkyBoxRotation;
	matData.ACES = mat->ACES;
	currMaterialBuffer->CopyData(mat->MatCBIndex, matData);

	// Next FrameResource need to be updated too.
	mat->NumFramesDirty--;
}

void RenderCore::SkyBox_LoadTextures(std::unordered_map<std::string, std::unique_ptr<Texture>>& mSkyTextures)
{
	std::vector<std::string> texNames =
	{
		"skyCubeMap",
		"DiffuseIBL",
	};

	std::vector<std::wstring> texFilenames =
	{
		L"Textures/DGarden_specularIBL.dds",
		L"Textures/DGarden_diffuseIBL.dds",
	};

	for (int i = 0; i < (int)texNames.size(); ++i)
	{
		// Don't create duplicates.
		if (mSkyTextures.find(texNames[i]) == std::end(mSkyTextures))
		{
			auto texMap = std::make_unique<Texture>();
			texMap->Name = texNames[i];
			texMap->Filename = texFilenames[i];
			ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
				mCommandList.Get(), texMap->Filename.c_str(),
				texMap->Resource, texMap->UploadHeap));

			mSkyTextures[texMap->Name] = std::move(texMap);
		}
	}
}

void RenderCore::SkyBox_BuildDescriptorHeaps(ComPtr<ID3D12Resource>& skyCubeMap, ComPtr<ID3D12Resource>& diffuseIBL,std::unordered_map<std::string, std::unique_ptr<Texture>>& mSkyTextures)
{
	skyCubeMap = mSkyTextures["skyCubeMap"]->Resource;
	diffuseIBL = mSkyTextures["DiffuseIBL"]->Resource;
}


void RenderCore::SkyBox_BuildMaterials(std::unordered_map<std::string, std::unique_ptr<Material>>& mMaterials)
{
	auto sky = std::make_unique<Material>();
	sky->Name = "sky";
	sky->MatCBIndex = 0;
	sky->SkyBoxTint = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	sky->SkyBoxExposure = 1;
	sky->SkyBoxRotation = 0;
	sky->ACES = 1;

	mMaterials["sky"] = std::move(sky);
}

void RenderCore::SkyBox_BuildRenderItems(
	std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count],
	std::unordered_map<std::string, std::unique_ptr<Material>>& mMaterials,
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& mGeometries,
	std::vector<std::unique_ptr<RenderItem>>& mAllRitems)
{
	auto skyRitem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&skyRitem->World, XMMatrixScaling(5000.0f, 5000.0f, 5000.0f));
	skyRitem->TexTransform = Mathf::Identity4x4();
	skyRitem->ObjCBIndex = 0;
	skyRitem->Mat = mMaterials["sky"].get();
	skyRitem->Geo = mGeometries["shapeGeo"].get();
	skyRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	skyRitem->IndexCount = skyRitem->Geo->DrawArgs["sphere"].IndexCount;
	skyRitem->StartIndexLocation = skyRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
	skyRitem->BaseVertexLocation = skyRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::Sky].push_back(skyRitem.get());
	mAllRitems.push_back(std::move(skyRitem));
}



//--------------------	PBR Demo	--------------------
void RenderCore::PBRDemo_UpdateMaterialBuffer(Material* mat, UploadBuffer<PBRMaterialData>* currMaterialBuffer)
{
	XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);

	PBRMaterialData matData;
	matData.BaseColor = mat->BaseColor;
	matData.Smoothness = mat->Smoothness;
	matData.Metallic = mat->Metallic;
	matData.Occlusion = mat->Occlusion;
	matData.EmissionColor = mat->EmissionColor;
	matData.EmissionStrength = mat->EmissionStrength;
	matData.ReceiveShadow = mat->ReceiveShadow;
	XMStoreFloat4x4(&matData.MatTransform, XMMatrixTranspose(matTransform));
	matData.DiffuseMapIndex = mat->DiffuseSrvHeapIndex;
	matData.NormalMapIndex = mat->NormalSrvHeapIndex;
	matData.MsoMapIndex = mat->MsoSrvHeapIndex;
	matData.EmissionMapIndex = mat->EmissionSrvHeapIndex;
	matData.LUTMapIndex = mat->LUTSrvHeapIndex;
	currMaterialBuffer->CopyData(mat->MatCBIndex, matData);

	// Next FrameResource need to be updated too.
	mat->NumFramesDirty--;
}

void RenderCore::PBRDemo_LoadTextures(std::unordered_map<std::string, std::unique_ptr<Texture>>& mTextures)
{
	std::vector<std::string> texNames =
	{
		"BricsDiffuse",
		"BricsNormal",

		"OakDiffuse",
		"OakNormal",

		"WoodenDiffuse",
		"WoodenNormal",

		"WoodenDiffuse2",
		"WoodenNormal2",

		"WoodenDiffuse3",
		"WoodenNormal3",

		"WoodenDiffuse4",
		"WoodenNormal4",

		"sampleLUT"
	};

	std::vector<std::wstring> texFilenames =
	{
		L"Textures/BricsDiffuse.dds",
		L"Textures/BricsNormal.dds",

		L"Textures/OakDiffuse.dds",
		L"Textures/OakNormal.dds",

		L"Textures/WoodenDiffuse.dds",
		L"Textures/WoodenNormal.dds",

		L"Textures/WoodenDiffuse2.dds",
		L"Textures/WoodenNormal2.dds",

		L"Textures/WoodenDiffuse3.dds",
		L"Textures/WoodenNormal3.dds",

		L"Textures/WoodenDiffuse4.dds",
		L"Textures/WoodenNormal4.dds",

		L"Textures/sampleLUT.dds",
	};

	for (int i = 0; i < (int)texNames.size(); ++i)
	{
		// Don't create duplicates.
		if (mTextures.find(texNames[i]) == std::end(mTextures))
		{
			auto texMap = std::make_unique<Texture>();
			texMap->Name = texNames[i];
			texMap->Filename = texFilenames[i];
			ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
				mCommandList.Get(), texMap->Filename.c_str(),
				texMap->Resource, texMap->UploadHeap));

			mTextures[texMap->Name] = std::move(texMap);
		}
	}
}

void RenderCore::PBRDemo_BuildDescriptorHeaps(std::vector<ComPtr<ID3D12Resource>>& tex2DList,std::unordered_map<std::string, std::unique_ptr<Texture>>& mTextures)
{
	tex2DList =
	{
		mTextures["BricsDiffuse"]->Resource,
		mTextures["BricsNormal"]->Resource,

		mTextures["OakDiffuse"]->Resource,
		mTextures["OakNormal"]->Resource,

		mTextures["WoodenDiffuse"]->Resource,
		mTextures["WoodenNormal"]->Resource,

		mTextures["WoodenDiffuse2"]->Resource,
		mTextures["WoodenNormal2"]->Resource,

		mTextures["WoodenDiffuse3"]->Resource,
		mTextures["WoodenNormal3"]->Resource,

		mTextures["WoodenDiffuse4"]->Resource,
		mTextures["WoodenNormal4"]->Resource,
	};
}

void RenderCore::PBRDemo_BuildMaterials(std::unordered_map<std::string, std::unique_ptr<Material>>& mMaterials)
{
	auto plane = std::make_unique<Material>();
	plane->Name = "plane";
	plane->MatCBIndex = 1;
	plane->DiffuseSrvHeapIndex = GizmoCount + 0;
	plane->NormalSrvHeapIndex = GizmoCount + 1;
	plane->MsoSrvHeapIndex = GizmoCount + 16;
	plane->EmissionSrvHeapIndex = GizmoCount + 15;
	plane->LUTSrvHeapIndex = GizmoCount + 14;
	plane->BaseColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	plane->Smoothness = 0.0f;
	plane->Metallic = 0.0f;
	plane->Occlusion = 1;
	plane->EmissionColor = Vector4(0, 0, 0, 1);
	plane->EmissionStrength = 0.0f;
	plane->ReceiveShadow = 1;

	auto sphere = std::make_unique<Material>();
	sphere->Name = "sphere";
	sphere->MatCBIndex = 2;
	sphere->DiffuseSrvHeapIndex = GizmoCount + 2;
	sphere->NormalSrvHeapIndex = GizmoCount + 3;
	sphere->MsoSrvHeapIndex = GizmoCount + 16;
	sphere->EmissionSrvHeapIndex = GizmoCount + 15;
	sphere->LUTSrvHeapIndex = GizmoCount + 14;
	sphere->BaseColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	sphere->Smoothness = 0.45f;
	sphere->Metallic = 0.0f;
	sphere->Occlusion = 1.0f;
	sphere->EmissionColor = Vector4(0, 0, 0, 1);
	sphere->EmissionStrength = 0.0f;
	sphere->ReceiveShadow = 0;



	mMaterials["plane"] = std::move(plane);
	mMaterials["sphere"] = std::move(sphere);
}

void RenderCore::PBRDemo_BuildRenderItems(
	std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count],
	std::unordered_map<std::string, std::unique_ptr<Material>>& mMaterials,
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& mGeometries,
	std::vector<std::unique_ptr<RenderItem>>& mAllRitems)
{
	auto sphereRitem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&sphereRitem->World, XMMatrixScaling(2, 2, 2) * XMMatrixTranslation(0, 1, 0));

	XMStoreFloat4x4(&sphereRitem->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	sphereRitem->ObjCBIndex = 1;
	sphereRitem->Mat = mMaterials["sphere"].get();
	sphereRitem->Geo = mGeometries["shapeGeo"].get();
	sphereRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	sphereRitem->IndexCount = sphereRitem->Geo->DrawArgs["sphere"].IndexCount;
	sphereRitem->StartIndexLocation = sphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
	sphereRitem->BaseVertexLocation = sphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::Opaque].push_back(sphereRitem.get());
	mAllRitems.push_back(std::move(sphereRitem));

	auto gridRitem = std::make_unique<RenderItem>();
	gridRitem->World = Mathf::Identity4x4();

	XMStoreFloat4x4(&gridRitem->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	gridRitem->ObjCBIndex = 2;
	gridRitem->Mat = mMaterials["plane"].get();
	gridRitem->Geo = mGeometries["shapeGeo"].get();
	gridRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gridRitem->IndexCount = gridRitem->Geo->DrawArgs["grid"].IndexCount;
	gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::Opaque].push_back(gridRitem.get());
	mAllRitems.push_back(std::move(gridRitem));
}

void RenderCore::PBRDemo_UpdateObjectBuffer()
{

}