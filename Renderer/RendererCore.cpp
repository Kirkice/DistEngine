#include "UploadBuffer.h"
#include "GeometryGenerator.h"
#include "RendererCore.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace ModelLoader;

const int GizmoCount = 6;

RenderCore::RenderCore(HINSTANCE hInstance) : PhysicsCore(hInstance)
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

		"defaultDiffuseMap",
		"defaultNormalMap",
		"defaultMsoMap",
		"defaultEmissionMap",
		"defaultLUTMap",

		"WoodenDiffuse",
		"WoodenNormal",

		"MetallicDiffuse",
		"MetallicNormal",

		"BricsDiffuse",
		"BricsNormal",

		"OakDiffuse",
		"OakNormal",

		"WoodenDiffuse2",
		"WoodenNormal2",

		"WoodenDiffuse3",
		"WoodenNormal3",

		"WoodenDiffuse4",
		"WoodenNormal4",
	};

	std::vector<std::wstring> texFilenames =
	{
		L"Textures/PlaneDiffuse.dds",
		L"Textures/PlaneNormal.dds",
		L"Textures/mso.dds",
		L"Textures/black1x1.dds",
		L"Textures/sampleLUT.dds",

		L"Textures/WoodenDiffuse.dds",
		L"Textures/WoodenNormal.dds",

		L"Textures/white1x1.dds",
		L"Textures/default_nmap.dds",

		L"Textures/BricsDiffuse.dds",
		L"Textures/BricsNormal.dds",

		L"Textures/OakDiffuse.dds",
		L"Textures/OakNormal.dds",

		L"Textures/WoodenDiffuse2.dds",
		L"Textures/WoodenNormal2.dds",

		L"Textures/WoodenDiffuse3.dds",
		L"Textures/WoodenNormal3.dds",

		L"Textures/WoodenDiffuse4.dds",
		L"Textures/WoodenNormal4.dds",
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
		mTextures["defaultDiffuseMap"]->Resource,
		mTextures["defaultNormalMap"]->Resource,
		mTextures["defaultMsoMap"]->Resource,
		mTextures["defaultEmissionMap"]->Resource,
		mTextures["defaultLUTMap"]->Resource,

		mTextures["WoodenDiffuse"]->Resource,
		mTextures["WoodenNormal"]->Resource,

		mTextures["MetallicDiffuse"]->Resource,
		mTextures["MetallicNormal"]->Resource,

		mTextures["BricsDiffuse"]->Resource,
		mTextures["BricsNormal"]->Resource,

		mTextures["OakDiffuse"]->Resource,
		mTextures["OakNormal"]->Resource,

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
	plane->MsoSrvHeapIndex = GizmoCount + 2;
	plane->EmissionSrvHeapIndex = GizmoCount + 3;
	plane->LUTSrvHeapIndex = GizmoCount + 4;
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
	sphere->DiffuseSrvHeapIndex = GizmoCount + 5;
	sphere->NormalSrvHeapIndex = GizmoCount + 6;
	sphere->MsoSrvHeapIndex = GizmoCount + 2;
	sphere->EmissionSrvHeapIndex = GizmoCount + 3;
	sphere->LUTSrvHeapIndex = GizmoCount + 4;
	sphere->BaseColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	sphere->Smoothness = 0.45f;
	sphere->Metallic = 0.0f;
	sphere->Occlusion = 1.0f;
	sphere->EmissionColor = Vector4(0, 0, 0, 1);
	sphere->EmissionStrength = 0.0f;
	sphere->ReceiveShadow = 0;

	auto SphereAg = std::make_unique<Material>();
	SphereAg->Name = "sphereAg";
	SphereAg->MatCBIndex = 3;
	SphereAg->DiffuseSrvHeapIndex = GizmoCount + 7;
	SphereAg->NormalSrvHeapIndex = GizmoCount + 8;
	SphereAg->MsoSrvHeapIndex = GizmoCount + 2;
	SphereAg->EmissionSrvHeapIndex = GizmoCount + 3;
	SphereAg->LUTSrvHeapIndex = GizmoCount + 4;
	SphereAg->BaseColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	SphereAg->Smoothness = 0.25f;
	SphereAg->Metallic = 0.91f;
	SphereAg->Occlusion = 1.0f;
	SphereAg->EmissionColor = Vector4(0, 0, 0, 1);
	SphereAg->EmissionStrength = 0.0f;
	SphereAg->ReceiveShadow = 0;

	auto SphereAu = std::make_unique<Material>();
	SphereAu->Name = "sphereAu";
	SphereAu->MatCBIndex = 4;
	SphereAu->DiffuseSrvHeapIndex = GizmoCount + 7;
	SphereAu->NormalSrvHeapIndex = GizmoCount + 8;
	SphereAu->MsoSrvHeapIndex = GizmoCount + 2;
	SphereAu->EmissionSrvHeapIndex = GizmoCount + 3;
	SphereAu->LUTSrvHeapIndex = GizmoCount + 4;
	SphereAu->BaseColor = Vector4(1.0f, 0.77f, 0.44f, 1.0f);
	SphereAu->Smoothness = 0.25f;
	SphereAu->Metallic = 0.91f;
	SphereAu->Occlusion = 1.0f;
	SphereAu->EmissionColor = Vector4(0, 0, 0, 1);
	SphereAu->EmissionStrength = 0.0f;
	SphereAu->ReceiveShadow = 0;

	auto SphereCu = std::make_unique<Material>();
	SphereCu->Name = "sphereCu";
	SphereCu->MatCBIndex = 5;
	SphereCu->DiffuseSrvHeapIndex = GizmoCount + 7;
	SphereCu->NormalSrvHeapIndex = GizmoCount + 8;
	SphereCu->MsoSrvHeapIndex = GizmoCount + 2;
	SphereCu->EmissionSrvHeapIndex = GizmoCount + 3;
	SphereCu->LUTSrvHeapIndex = GizmoCount + 4;
	SphereCu->BaseColor = Vector4(0.95f, 0.63f, 0.54f, 1.0f);
	SphereCu->Smoothness = 0.25f;
	SphereCu->Metallic = 0.91f;
	SphereCu->Occlusion = 1.0f;
	SphereCu->EmissionColor = Vector4(0, 0, 0, 1);
	SphereCu->EmissionStrength = 0.0f;
	SphereCu->ReceiveShadow = 0;


	auto Brics = std::make_unique<Material>();
	Brics->Name = "Brics";
	Brics->MatCBIndex = 6;
	Brics->DiffuseSrvHeapIndex = GizmoCount + 9;
	Brics->NormalSrvHeapIndex = GizmoCount + 10;
	Brics->MsoSrvHeapIndex = GizmoCount + 2;
	Brics->EmissionSrvHeapIndex = GizmoCount + 3;
	Brics->LUTSrvHeapIndex = GizmoCount + 4;
	Brics->BaseColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	Brics->Smoothness = 0.1f;
	Brics->Metallic = 0.0f;
	Brics->Occlusion = 1.0f;
	Brics->EmissionColor = Vector4(0, 0, 0, 1);
	Brics->EmissionStrength = 0.0f;
	Brics->ReceiveShadow = 0;

	auto Oak = std::make_unique<Material>();
	Oak->Name = "Oak";
	Oak->MatCBIndex = 7;
	Oak->DiffuseSrvHeapIndex = GizmoCount + 11;
	Oak->NormalSrvHeapIndex = GizmoCount + 12;
	Oak->MsoSrvHeapIndex = GizmoCount + 2;
	Oak->EmissionSrvHeapIndex = GizmoCount + 3;
	Oak->LUTSrvHeapIndex = GizmoCount + 4;
	Oak->BaseColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	Oak->Smoothness = 0.35f;
	Oak->Metallic = 0.0f;
	Oak->Occlusion = 1.0f;
	Oak->EmissionColor = Vector4(0, 0, 0, 1);
	Oak->EmissionStrength = 0.0f;
	Oak->ReceiveShadow = 0;

	auto Wooden2 = std::make_unique<Material>();
	Wooden2->Name = "Wooden2";
	Wooden2->MatCBIndex = 8;
	Wooden2->DiffuseSrvHeapIndex = GizmoCount + 13;
	Wooden2->NormalSrvHeapIndex = GizmoCount + 14;
	Wooden2->MsoSrvHeapIndex = GizmoCount + 2;
	Wooden2->EmissionSrvHeapIndex = GizmoCount + 3;
	Wooden2->LUTSrvHeapIndex = GizmoCount + 4;
	Wooden2->BaseColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	Wooden2->Smoothness = 0.15f;
	Wooden2->Metallic = 0.0f;
	Wooden2->Occlusion = 1.0f;
	Wooden2->EmissionColor = Vector4(0, 0, 0, 1);
	Wooden2->EmissionStrength = 0.0f;
	Wooden2->ReceiveShadow = 0;

	auto Wooden3 = std::make_unique<Material>();
	Wooden3->Name = "Wooden3";
	Wooden3->MatCBIndex = 9;
	Wooden3->DiffuseSrvHeapIndex = GizmoCount + 15;
	Wooden3->NormalSrvHeapIndex = GizmoCount + 16;
	Wooden3->MsoSrvHeapIndex = GizmoCount + 2;
	Wooden3->EmissionSrvHeapIndex = GizmoCount + 3;
	Wooden3->LUTSrvHeapIndex = GizmoCount + 4;
	Wooden3->BaseColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	Wooden3->Smoothness = 0.15f;
	Wooden3->Metallic = 0.0f;
	Wooden3->Occlusion = 1.0f;
	Wooden3->EmissionColor = Vector4(0, 0, 0, 1);
	Wooden3->EmissionStrength = 0.0f;
	Wooden3->ReceiveShadow = 0;

	auto Wooden4 = std::make_unique<Material>();
	Wooden4->Name = "Wooden4";
	Wooden4->MatCBIndex = GizmoCount + 10;
	Wooden4->DiffuseSrvHeapIndex = GizmoCount + 17;
	Wooden4->NormalSrvHeapIndex = GizmoCount + 18;
	Wooden4->MsoSrvHeapIndex = GizmoCount + 2;
	Wooden4->EmissionSrvHeapIndex = GizmoCount + 3;
	Wooden4->LUTSrvHeapIndex = GizmoCount + 4;
	Wooden4->BaseColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	Wooden4->Smoothness = 0.9f;
	Wooden4->Metallic = 0.01f;
	Wooden4->Occlusion = 1.0f;
	Wooden4->EmissionColor = Vector4(0, 0, 0, 1);
	Wooden4->EmissionStrength = 0.0f;
	Wooden4->ReceiveShadow = 0;

	mMaterials["plane"] = std::move(plane);
	mMaterials["sphere"] = std::move(sphere);
	mMaterials["sphereAg"] = std::move(SphereAg);
	mMaterials["sphereAu"] = std::move(SphereAu);
	mMaterials["sphereCu"] = std::move(SphereCu);
	mMaterials["Brics"] = std::move(Brics);
	mMaterials["Oak"] = std::move(Oak);
	mMaterials["Wooden2"] = std::move(Wooden2);
	mMaterials["Wooden3"] = std::move(Wooden3);
	mMaterials["Wooden4"] = std::move(Wooden4);
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


	//Ag
	auto sphereAgRitem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&sphereAgRitem->World, XMMatrixScaling(2, 2, 2) * XMMatrixTranslation(3, 1, 3));

	XMStoreFloat4x4(&sphereAgRitem->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	sphereAgRitem->ObjCBIndex = 3;
	sphereAgRitem->Mat = mMaterials["sphereAg"].get();
	sphereAgRitem->Geo = mGeometries["shapeGeo"].get();
	sphereAgRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	sphereAgRitem->IndexCount = sphereAgRitem->Geo->DrawArgs["sphere"].IndexCount;
	sphereAgRitem->StartIndexLocation = sphereAgRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
	sphereAgRitem->BaseVertexLocation = sphereAgRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::Opaque].push_back(sphereAgRitem.get());
	mAllRitems.push_back(std::move(sphereAgRitem));

	//Au
	auto sphereAuRitem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&sphereAuRitem->World, XMMatrixScaling(2, 2, 2) * XMMatrixTranslation(-3, 1, 3));

	XMStoreFloat4x4(&sphereAuRitem->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	sphereAuRitem->ObjCBIndex = 4;
	sphereAuRitem->Mat = mMaterials["sphereAu"].get();
	sphereAuRitem->Geo = mGeometries["shapeGeo"].get();
	sphereAuRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	sphereAuRitem->IndexCount = sphereAuRitem->Geo->DrawArgs["sphere"].IndexCount;
	sphereAuRitem->StartIndexLocation = sphereAuRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
	sphereAuRitem->BaseVertexLocation = sphereAuRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::Opaque].push_back(sphereAuRitem.get());
	mAllRitems.push_back(std::move(sphereAuRitem));

	//Cu
	auto sphereCuRitem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&sphereCuRitem->World, XMMatrixScaling(2, 2, 2) * XMMatrixTranslation(0, 1, 3));

	XMStoreFloat4x4(&sphereCuRitem->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	sphereCuRitem->ObjCBIndex = 5;
	sphereCuRitem->Mat = mMaterials["sphereCu"].get();
	sphereCuRitem->Geo = mGeometries["shapeGeo"].get();
	sphereCuRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	sphereCuRitem->IndexCount = sphereCuRitem->Geo->DrawArgs["sphere"].IndexCount;
	sphereCuRitem->StartIndexLocation = sphereCuRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
	sphereCuRitem->BaseVertexLocation = sphereCuRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::Opaque].push_back(sphereCuRitem.get());
	mAllRitems.push_back(std::move(sphereCuRitem));

	//Brics
	auto BricsRitem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&BricsRitem->World, XMMatrixScaling(2, 2, 2) * XMMatrixTranslation(3, 1, 0));

	XMStoreFloat4x4(&BricsRitem->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	BricsRitem->ObjCBIndex = 6;
	BricsRitem->Mat = mMaterials["Brics"].get();
	BricsRitem->Geo = mGeometries["shapeGeo"].get();
	BricsRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	BricsRitem->IndexCount = BricsRitem->Geo->DrawArgs["sphere"].IndexCount;
	BricsRitem->StartIndexLocation = BricsRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
	BricsRitem->BaseVertexLocation = BricsRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::Opaque].push_back(BricsRitem.get());
	mAllRitems.push_back(std::move(BricsRitem));

	//Oak
	auto OakRitem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&OakRitem->World, XMMatrixScaling(2, 2, 2) * XMMatrixTranslation(-3, 1, 0));

	XMStoreFloat4x4(&OakRitem->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	OakRitem->ObjCBIndex = 7;
	OakRitem->Mat = mMaterials["Oak"].get();
	OakRitem->Geo = mGeometries["shapeGeo"].get();
	OakRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	OakRitem->IndexCount = OakRitem->Geo->DrawArgs["sphere"].IndexCount;
	OakRitem->StartIndexLocation = OakRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
	OakRitem->BaseVertexLocation = OakRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::Opaque].push_back(OakRitem.get());
	mAllRitems.push_back(std::move(OakRitem));

	//Wooden2
	auto Wooden2Ritem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&Wooden2Ritem->World, XMMatrixScaling(2, 2, 2) * XMMatrixTranslation(0, 1, -3));

	XMStoreFloat4x4(&Wooden2Ritem->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	Wooden2Ritem->ObjCBIndex = 8;
	Wooden2Ritem->Mat = mMaterials["Wooden2"].get();
	Wooden2Ritem->Geo = mGeometries["shapeGeo"].get();
	Wooden2Ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	Wooden2Ritem->IndexCount = Wooden2Ritem->Geo->DrawArgs["sphere"].IndexCount;
	Wooden2Ritem->StartIndexLocation = Wooden2Ritem->Geo->DrawArgs["sphere"].StartIndexLocation;
	Wooden2Ritem->BaseVertexLocation = Wooden2Ritem->Geo->DrawArgs["sphere"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::Opaque].push_back(Wooden2Ritem.get());
	mAllRitems.push_back(std::move(Wooden2Ritem));

	//Wooden3
	auto Wooden3Ritem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&Wooden3Ritem->World, XMMatrixScaling(2, 2, 2) * XMMatrixTranslation(3, 1, -3));

	XMStoreFloat4x4(&Wooden3Ritem->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	Wooden3Ritem->ObjCBIndex = 9;
	Wooden3Ritem->Mat = mMaterials["Wooden3"].get();
	Wooden3Ritem->Geo = mGeometries["shapeGeo"].get();
	Wooden3Ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	Wooden3Ritem->IndexCount = Wooden3Ritem->Geo->DrawArgs["sphere"].IndexCount;
	Wooden3Ritem->StartIndexLocation = Wooden3Ritem->Geo->DrawArgs["sphere"].StartIndexLocation;
	Wooden3Ritem->BaseVertexLocation = Wooden3Ritem->Geo->DrawArgs["sphere"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::Opaque].push_back(Wooden3Ritem.get());
	mAllRitems.push_back(std::move(Wooden3Ritem));

	//Wooden4
	auto Wooden4Ritem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&Wooden4Ritem->World, XMMatrixScaling(2, 2, 2) * XMMatrixTranslation(-3, 1, -3));

	XMStoreFloat4x4(&Wooden4Ritem->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	Wooden4Ritem->ObjCBIndex = 10;
	Wooden4Ritem->Mat = mMaterials["Wooden4"].get();
	Wooden4Ritem->Geo = mGeometries["shapeGeo"].get();
	Wooden4Ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	Wooden4Ritem->IndexCount = Wooden4Ritem->Geo->DrawArgs["sphere"].IndexCount;
	Wooden4Ritem->StartIndexLocation = Wooden4Ritem->Geo->DrawArgs["sphere"].StartIndexLocation;
	Wooden4Ritem->BaseVertexLocation = Wooden4Ritem->Geo->DrawArgs["sphere"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::Opaque].push_back(Wooden4Ritem.get());
	mAllRitems.push_back(std::move(Wooden4Ritem));
}

void RenderCore::PBRDemo_UpdateObjectBuffer()
{

}

//--------------------	Gizmos	--------------------
void RenderCore::EditorGizmo_LoadTextures(std::unordered_map<std::string, std::unique_ptr<Texture>>& mGizmoTextures)
{
	std::vector<std::string> texNames =
	{
		"AreaLightGizmo",
		"DirectionalLightGizmo",
		"ParticleSystemGizmo",
		"PointLightGizmo",
		"SpotLightGizmo",
		"WirePlane",
	};

	std::vector<std::wstring> texFilenames =
	{
		L"Icons/AreaLightGizmo.dds",
		L"Icons/DirectionalLightGizmo.dds",
		L"Icons/ParticleSystemGizmo.dds",
		L"Icons/PointLightGizmo.dds",
		L"Icons/SpotLightGizmo.dds",
		L"Icons/WirePlane.dds",
	};

	for (int i = 0; i < (int)texNames.size(); ++i)
	{
		// Don't create duplicates.
		if (mGizmoTextures.find(texNames[i]) == std::end(mGizmoTextures))
		{
			auto texMap = std::make_unique<Texture>();
			texMap->Name = texNames[i];
			texMap->Filename = texFilenames[i];
			ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(),
				mCommandList.Get(), texMap->Filename.c_str(),
				texMap->Resource, texMap->UploadHeap));

			mGizmoTextures[texMap->Name] = std::move(texMap);
		}
	}
}

void RenderCore::EditorGizmo_BuildDescriptorHeaps(std::vector<ComPtr<ID3D12Resource>>& tex2DList, std::unordered_map<std::string, std::unique_ptr<Texture>>& mTextures)
{
	tex2DList =
	{
		mTextures["AreaLightGizmo"]->Resource,
		mTextures["DirectionalLightGizmo"]->Resource,
		mTextures["ParticleSystemGizmo"]->Resource,
		mTextures["PointLightGizmo"]->Resource,
		mTextures["SpotLightGizmo"]->Resource,
		mTextures["WirePlane"]->Resource,
	};
}

void RenderCore::EditorGizmo_BuildMaterials(std::unordered_map<std::string, std::unique_ptr<Material>>& mMaterials)
{
	auto AreaLightGizmo = std::make_unique<Material>();
	AreaLightGizmo->Name = "AreaLightGizmo";
	AreaLightGizmo->MatCBIndex = 10;
	AreaLightGizmo->DiffuseSrvHeapIndex = 0;

	auto DirectionalLightGizmo = std::make_unique<Material>();
	DirectionalLightGizmo->Name = "DirectionalLightGizmo";
	DirectionalLightGizmo->MatCBIndex = 11;
	DirectionalLightGizmo->DiffuseSrvHeapIndex = 1;

	auto ParticleSystemGizmo = std::make_unique<Material>();
	ParticleSystemGizmo->Name = "ParticleSystemGizmo";
	ParticleSystemGizmo->MatCBIndex = 12;
	ParticleSystemGizmo->DiffuseSrvHeapIndex = 2;

	auto PointLightGizmo = std::make_unique<Material>();
	PointLightGizmo->Name = "PointLightGizmo";
	PointLightGizmo->MatCBIndex = 13;
	PointLightGizmo->DiffuseSrvHeapIndex = 3;

	auto SpotLightGizmo = std::make_unique<Material>();
	SpotLightGizmo->Name = "SpotLightGizmo";
	SpotLightGizmo->MatCBIndex = 14;
	SpotLightGizmo->DiffuseSrvHeapIndex = 4;

	auto WirePlane = std::make_unique<Material>();
	WirePlane->Name = "WirePlane";
	WirePlane->MatCBIndex = 15;
	WirePlane->DiffuseSrvHeapIndex = 5;

	mMaterials["AreaLightGizmo"] = std::move(AreaLightGizmo);
	mMaterials["DirectionalLightGizmo"] = std::move(DirectionalLightGizmo);
	mMaterials["ParticleSystemGizmo"] = std::move(ParticleSystemGizmo);
	mMaterials["PointLightGizmo"] = std::move(PointLightGizmo);
	mMaterials["SpotLightGizmo"] = std::move(SpotLightGizmo);
	mMaterials["WirePlane"] = std::move(WirePlane);
}

void RenderCore::EditorGizmo_BuildRenderItems(
	std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count],
	std::unordered_map<std::string, std::unique_ptr<Material>>& mMaterials,
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& mGeometries,
	std::vector<std::unique_ptr<RenderItem>>& mAllRitems)
{
	//Direction Light Gizmo
	auto DirGizmoRitem = std::make_unique<RenderItem>();;
	XMStoreFloat4x4(&DirGizmoRitem->World, XMMatrixScaling(0.03, 0.03, 0.02) * XMMatrixTranslation(mDirectionLightsPos.x, mDirectionLightsPos.y,mDirectionLightsPos.z));

	XMStoreFloat4x4(&DirGizmoRitem->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	DirGizmoRitem->ObjCBIndex = 11;
	DirGizmoRitem->Mat = mMaterials["DirectionalLightGizmo"].get();
	DirGizmoRitem->Geo = mGeometries["shapeGeo"].get();
	DirGizmoRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	DirGizmoRitem->IndexCount = DirGizmoRitem->Geo->DrawArgs["grid"].IndexCount;
	DirGizmoRitem->StartIndexLocation = DirGizmoRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	DirGizmoRitem->BaseVertexLocation = DirGizmoRitem->Geo->DrawArgs["grid"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::Gizmo].push_back(DirGizmoRitem.get());
	mAllRitems.push_back(std::move(DirGizmoRitem));

	//Point Light Gizmo
	auto PointGizmoRitem = std::make_unique<RenderItem>();;
	XMStoreFloat4x4(&PointGizmoRitem->World, XMMatrixScaling(0.03, 0.03, 0.02) * XMMatrixTranslation(mPointLightsPos.x, mPointLightsPos.y, mPointLightsPos.z));

	XMStoreFloat4x4(&PointGizmoRitem->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	PointGizmoRitem->ObjCBIndex = 12;
	PointGizmoRitem->Mat = mMaterials["PointLightGizmo"].get();
	PointGizmoRitem->Geo = mGeometries["shapeGeo"].get();
	PointGizmoRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	PointGizmoRitem->IndexCount = PointGizmoRitem->Geo->DrawArgs["grid"].IndexCount;
	PointGizmoRitem->StartIndexLocation = PointGizmoRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	PointGizmoRitem->BaseVertexLocation = PointGizmoRitem->Geo->DrawArgs["grid"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::Gizmo].push_back(PointGizmoRitem.get());
	mAllRitems.push_back(std::move(PointGizmoRitem));


	//Wire Gizmo
	auto wireGizmoRitem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&wireGizmoRitem->World, XMMatrixScaling(300, 1, 200) * XMMatrixTranslation(0, -0.01, 0));

	XMStoreFloat4x4(&wireGizmoRitem->TexTransform, XMMatrixScaling(2000, 2000, 1.0f));
	wireGizmoRitem->ObjCBIndex = 13;
	wireGizmoRitem->Mat = mMaterials["WirePlane"].get();
	wireGizmoRitem->Geo = mGeometries["shapeGeo"].get();
	wireGizmoRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	wireGizmoRitem->IndexCount = wireGizmoRitem->Geo->DrawArgs["grid"].IndexCount;
	wireGizmoRitem->StartIndexLocation = wireGizmoRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	wireGizmoRitem->BaseVertexLocation = wireGizmoRitem->Geo->DrawArgs["grid"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::Unlit].push_back(wireGizmoRitem.get());
	mAllRitems.push_back(std::move(wireGizmoRitem));
}

void RenderCore::EditorGizmo_UpdateObjectBuffer(UINT ObjCBIndex, XMFLOAT4X4* eWorldMatrix)
{
	switch (ObjCBIndex)
	{
		case 11:XMStoreFloat4x4(eWorldMatrix, XMMatrixScaling(0.03, 0.03, 0.02) * XMMatrixTranslation(mDirectionLightsPos.x, mDirectionLightsPos.y, mDirectionLightsPos.z));break;
		case 12:XMStoreFloat4x4(eWorldMatrix, XMMatrixScaling(0.03, 0.03, 0.02) * XMMatrixTranslation(mPointLightsPos.x, mPointLightsPos.y, mPointLightsPos.z)); break;
	}
}


//--------------------	Bounding	--------------------
void RenderCore::Bounding_BuildMaterials(std::unordered_map<std::string, std::unique_ptr<Material>>& mMaterials)
{
	auto BoundingMat = std::make_unique<Material>();
	BoundingMat->Name = "Bounding";
	BoundingMat->MatCBIndex = 17;
	mMaterials["Bounding"] = std::move(BoundingMat);
}

void RenderCore::Bounding_BuildRenderItems(
	std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count],
	std::unordered_map<std::string, std::unique_ptr<Material>>& mMaterials,
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& mGeometries,
	std::vector<std::unique_ptr<RenderItem>>& mAllRitems)
{
	auto AABBItem_Sphere = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&AABBItem_Sphere->World, XMMatrixScaling(2, 2, 2) * XMMatrixTranslation(0, 1, 0));
	XMStoreFloat4x4(&AABBItem_Sphere->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	AABBItem_Sphere->ObjCBIndex = 14;
	AABBItem_Sphere->Mat = mMaterials["Bounding"].get();
	AABBItem_Sphere->Geo = mGeometries["shapeGeo"].get();
	AABBItem_Sphere->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	AABBItem_Sphere->IndexCount = AABBItem_Sphere->Geo->DrawArgs["aabb"].IndexCount;
	AABBItem_Sphere->StartIndexLocation = AABBItem_Sphere->Geo->DrawArgs["aabb"].StartIndexLocation;
	AABBItem_Sphere->BaseVertexLocation = AABBItem_Sphere->Geo->DrawArgs["aabb"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::Bounding].push_back(AABBItem_Sphere.get());
	mAllRitems.push_back(std::move(AABBItem_Sphere));


	//Ag
	auto AABBItem_Ag = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&AABBItem_Ag->World, XMMatrixScaling(2, 2, 2) * XMMatrixTranslation(3, 1, 3));

	XMStoreFloat4x4(&AABBItem_Ag->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	AABBItem_Ag->ObjCBIndex = 15;
	AABBItem_Ag->Mat = mMaterials["Bounding"].get();
	AABBItem_Ag->Geo = mGeometries["shapeGeo"].get();
	AABBItem_Ag->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	AABBItem_Ag->IndexCount = AABBItem_Ag->Geo->DrawArgs["aabb"].IndexCount;
	AABBItem_Ag->StartIndexLocation = AABBItem_Ag->Geo->DrawArgs["aabb"].StartIndexLocation;
	AABBItem_Ag->BaseVertexLocation = AABBItem_Ag->Geo->DrawArgs["aabb"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::Bounding].push_back(AABBItem_Ag.get());
	mAllRitems.push_back(std::move(AABBItem_Ag));

	//Au
	auto AABBItem_Au = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&AABBItem_Au->World, XMMatrixScaling(2, 2, 2) * XMMatrixTranslation(-3, 1, 3));

	XMStoreFloat4x4(&AABBItem_Au->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	AABBItem_Au->ObjCBIndex = 16;
	AABBItem_Au->Mat = mMaterials["Bounding"].get();
	AABBItem_Au->Geo = mGeometries["shapeGeo"].get();
	AABBItem_Au->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	AABBItem_Au->IndexCount = AABBItem_Au->Geo->DrawArgs["aabb"].IndexCount;
	AABBItem_Au->StartIndexLocation = AABBItem_Au->Geo->DrawArgs["aabb"].StartIndexLocation;
	AABBItem_Au->BaseVertexLocation = AABBItem_Au->Geo->DrawArgs["aabb"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::Bounding].push_back(AABBItem_Au.get());
	mAllRitems.push_back(std::move(AABBItem_Au));

	//Cu
	auto AABBItem_Cu = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&AABBItem_Cu->World, XMMatrixScaling(2, 2, 2) * XMMatrixTranslation(0, 1, 3));

	XMStoreFloat4x4(&AABBItem_Cu->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	AABBItem_Cu->ObjCBIndex = 17;
	AABBItem_Cu->Mat = mMaterials["Bounding"].get();
	AABBItem_Cu->Geo = mGeometries["shapeGeo"].get();
	AABBItem_Cu->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	AABBItem_Cu->IndexCount = AABBItem_Cu->Geo->DrawArgs["aabb"].IndexCount;
	AABBItem_Cu->StartIndexLocation = AABBItem_Cu->Geo->DrawArgs["aabb"].StartIndexLocation;
	AABBItem_Cu->BaseVertexLocation = AABBItem_Cu->Geo->DrawArgs["aabb"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::Bounding].push_back(AABBItem_Cu.get());
	mAllRitems.push_back(std::move(AABBItem_Cu));

	//Brics
	auto AABBItem_Brics = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&AABBItem_Brics->World, XMMatrixScaling(2, 2, 2) * XMMatrixTranslation(3, 1, 0));

	XMStoreFloat4x4(&AABBItem_Brics->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	AABBItem_Brics->ObjCBIndex = 18;
	AABBItem_Brics->Mat = mMaterials["Bounding"].get();
	AABBItem_Brics->Geo = mGeometries["shapeGeo"].get();
	AABBItem_Brics->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	AABBItem_Brics->IndexCount = AABBItem_Brics->Geo->DrawArgs["aabb"].IndexCount;
	AABBItem_Brics->StartIndexLocation = AABBItem_Brics->Geo->DrawArgs["aabb"].StartIndexLocation;
	AABBItem_Brics->BaseVertexLocation = AABBItem_Brics->Geo->DrawArgs["aabb"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::Bounding].push_back(AABBItem_Brics.get());
	mAllRitems.push_back(std::move(AABBItem_Brics));

	//Oak
	auto AABBItem_Oak = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&AABBItem_Oak->World, XMMatrixScaling(2, 2, 2) * XMMatrixTranslation(-3, 1, 0));

	XMStoreFloat4x4(&AABBItem_Oak->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	AABBItem_Oak->ObjCBIndex = 19;
	AABBItem_Oak->Mat = mMaterials["Bounding"].get();
	AABBItem_Oak->Geo = mGeometries["shapeGeo"].get();
	AABBItem_Oak->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	AABBItem_Oak->IndexCount = AABBItem_Oak->Geo->DrawArgs["aabb"].IndexCount;
	AABBItem_Oak->StartIndexLocation = AABBItem_Oak->Geo->DrawArgs["aabb"].StartIndexLocation;
	AABBItem_Oak->BaseVertexLocation = AABBItem_Oak->Geo->DrawArgs["aabb"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::Bounding].push_back(AABBItem_Oak.get());
	mAllRitems.push_back(std::move(AABBItem_Oak));

	//Wooden2
	auto AABBItem_Wooden2 = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&AABBItem_Wooden2->World, XMMatrixScaling(2, 2, 2) * XMMatrixTranslation(0, 1, -3));

	XMStoreFloat4x4(&AABBItem_Wooden2->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	AABBItem_Wooden2->ObjCBIndex = 20;
	AABBItem_Wooden2->Mat = mMaterials["Bounding"].get();
	AABBItem_Wooden2->Geo = mGeometries["shapeGeo"].get();
	AABBItem_Wooden2->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	AABBItem_Wooden2->IndexCount = AABBItem_Wooden2->Geo->DrawArgs["aabb"].IndexCount;
	AABBItem_Wooden2->StartIndexLocation = AABBItem_Wooden2->Geo->DrawArgs["aabb"].StartIndexLocation;
	AABBItem_Wooden2->BaseVertexLocation = AABBItem_Wooden2->Geo->DrawArgs["aabb"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::Bounding].push_back(AABBItem_Wooden2.get());
	mAllRitems.push_back(std::move(AABBItem_Wooden2));

	//Wooden3
	auto AABBItem_Wooden3 = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&AABBItem_Wooden3->World, XMMatrixScaling(2, 2, 2) * XMMatrixTranslation(3, 1, -3));

	XMStoreFloat4x4(&AABBItem_Wooden3->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	AABBItem_Wooden3->ObjCBIndex = 21;
	AABBItem_Wooden3->Mat = mMaterials["Bounding"].get();
	AABBItem_Wooden3->Geo = mGeometries["shapeGeo"].get();
	AABBItem_Wooden3->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	AABBItem_Wooden3->IndexCount = AABBItem_Wooden3->Geo->DrawArgs["aabb"].IndexCount;
	AABBItem_Wooden3->StartIndexLocation = AABBItem_Wooden3->Geo->DrawArgs["aabb"].StartIndexLocation;
	AABBItem_Wooden3->BaseVertexLocation = AABBItem_Wooden3->Geo->DrawArgs["aabb"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::Bounding].push_back(AABBItem_Wooden3.get());
	mAllRitems.push_back(std::move(AABBItem_Wooden3));

	//Wooden4
	auto AABBItem_Wooden4 = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&AABBItem_Wooden4->World, XMMatrixScaling(2, 2, 2) * XMMatrixTranslation(-3, 1, -3));

	XMStoreFloat4x4(&AABBItem_Wooden4->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	AABBItem_Wooden4->ObjCBIndex = 22;
	AABBItem_Wooden4->Mat = mMaterials["Bounding"].get();
	AABBItem_Wooden4->Geo = mGeometries["shapeGeo"].get();
	AABBItem_Wooden4->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	AABBItem_Wooden4->IndexCount = AABBItem_Wooden4->Geo->DrawArgs["aabb"].IndexCount;
	AABBItem_Wooden4->StartIndexLocation = AABBItem_Wooden4->Geo->DrawArgs["aabb"].StartIndexLocation;
	AABBItem_Wooden4->BaseVertexLocation = AABBItem_Wooden4->Geo->DrawArgs["aabb"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::Bounding].push_back(AABBItem_Wooden4.get());
	mAllRitems.push_back(std::move(AABBItem_Wooden4));
}

void RenderCore::Bounding_UpdateObjectBuffer(UINT ObjCBIndex, XMFLOAT4X4* eWorldMatrix)
{

}


// Post process
void RenderCore::Post_UpdateMaterialBuffer(Material* mat, UploadBuffer<PostprocessingData>* currMaterialBuffer)
{
	XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);

	PostprocessingData matData;
	matData.RGBSplitStrength = mat->RGBSplitStrength;
	matData.DecolorStrength = mat->DecolorStrength;
	matData.Threshold = mat->Threshold;
	matData.SoftKnee = mat->SoftKnee;
	matData.Radius = mat->Radius;
	matData.Intensity = mat->Intensity;
	matData.Brightness = mat->Brightness;
	matData.Contrast = mat->Contrast;
	matData.Hue = mat->Hue;
	matData.Saturation = mat->Saturation;
	matData.Value = mat->Value;
	matData.PixelSize = mat->PixelSize;
	matData.BlurFactory = mat->BlurFactory;

	matData.EdgeWdith = mat->EdgeWdith;
	matData.EdgeNeonFade = mat->EdgeNeonFade;
	matData.OutLineColor = mat->OutLineColor;
	matData.BackgroundFade = mat->BackgroundFade;
	matData.BackgroundColor = mat->BackgroundColor;
	matData.Temperature = mat->Temperature;
	matData.Tint = mat->Tint;
	matData.VignetteColor = mat->VignetteColor;
	matData.Width = mat->Width;
	matData.Height = mat->Height;
	matData.Smooth = mat->Smooth;
	matData.Alpha = mat->Alpha;
	matData.SharpenStrength = mat->SharpenStrength;
	matData.SharpenThreshold = mat->SharpenThreshold;
	matData.Spherify = mat->Spherify;
	matData.OilPaintRadius = mat->OilPaintRadius;
	matData.ResoultionValue = mat->ResoultionValue;
	matData.MotionBlurAmount = mat->MotionBlurAmount;
	matData.Aperture = mat->Aperture;
	matData.FocalLength - mat->FocalLength;
	matData.Distance = mat->Distance;
	matData.ResoultionLevel = mat->ResoultionLevel;
	matData.FSRSharpen = matData.FSRSharpen;


	currMaterialBuffer->CopyData(mat->MatCBIndex, matData);

	// Next FrameResource need to be updated too.
	mat->NumFramesDirty--;
}

void RenderCore::Post_BuildMaterials(std::unordered_map<std::string, std::unique_ptr<Material>>& mMaterials)
{
	auto CopyColorMat = std::make_unique<Material>();
	CopyColorMat->Name = "CopyColor";
	CopyColorMat->MatCBIndex = 18;
	mMaterials["CopyColor"] = std::move(CopyColorMat);

	auto RGBSplitMat = std::make_unique<Material>();
	RGBSplitMat->Name = "RGBSplit";
	RGBSplitMat->MatCBIndex = 19;
	mMaterials["RGBSplit"] = std::move(RGBSplitMat);

	auto RadialBlurMat = std::make_unique<Material>();
	RadialBlurMat->Name = "RadialBlur";
	RadialBlurMat->MatCBIndex = 20;
	mMaterials["RadialBlur"] = std::move(RadialBlurMat);

	auto VignetteMat = std::make_unique<Material>();
	VignetteMat->Name = "Vignette";
	VignetteMat->MatCBIndex = 21;
	mMaterials["Vignette"] = std::move(VignetteMat);

	auto DecolorMat = std::make_unique<Material>();
	DecolorMat->Name = "Decolor";
	DecolorMat->MatCBIndex = 22;
	mMaterials["Decolor"] = std::move(DecolorMat);

	auto BrightnessMat = std::make_unique<Material>();
	BrightnessMat->Name = "Brightness";
	BrightnessMat->MatCBIndex = 23;
	mMaterials["Brightness"] = std::move(BrightnessMat);

	auto HSVMat = std::make_unique<Material>();
	HSVMat->Name = "HSV";
	HSVMat->MatCBIndex = 24;
	mMaterials["HSV"] = std::move(HSVMat);

	auto MosaicMat = std::make_unique<Material>();
	MosaicMat->Name = "Mosaic";
	MosaicMat->MatCBIndex = 25;
	mMaterials["Mosaic"] = std::move(MosaicMat);

	auto SharpenMat = std::make_unique<Material>();
	SharpenMat->Name = "Sharpen";
	SharpenMat->MatCBIndex = 26;
	mMaterials["Sharpen"] = std::move(SharpenMat);

	auto SpherizeMat = std::make_unique<Material>();
	SpherizeMat->Name = "Spherize";
	SpherizeMat->MatCBIndex = 27;
	mMaterials["Spherize"] = std::move(SpherizeMat);

	auto WhiteBalanceMat = std::make_unique<Material>();
	WhiteBalanceMat->Name = "WhiteBalance";
	WhiteBalanceMat->MatCBIndex = 28;
	mMaterials["WhiteBalance"] = std::move(WhiteBalanceMat);

	auto OilPaintMat = std::make_unique<Material>();
	OilPaintMat->Name = "OilPaint";
	OilPaintMat->MatCBIndex = 29;
	mMaterials["OilPaint"] = std::move(OilPaintMat);

	auto ReliefMat = std::make_unique<Material>();
	ReliefMat->Name = "Relief";
	ReliefMat->MatCBIndex = 30;
	mMaterials["Relief"] = std::move(ReliefMat);

	auto EdgeDetectionMat = std::make_unique<Material>();
	EdgeDetectionMat->Name = "EdgeDetection";
	EdgeDetectionMat->MatCBIndex = 31;
	mMaterials["EdgeDetection"] = std::move(EdgeDetectionMat);

	auto BloomMat = std::make_unique<Material>();
	BloomMat->Name = "Bloom";
	BloomMat->MatCBIndex = 32;
	mMaterials["Bloom"] = std::move(BloomMat);
}

void RenderCore::Post_BuildRenderItems(
	std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count],
	std::unordered_map<std::string, std::unique_ptr<Material>>& mMaterials,
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& mGeometries,
	std::vector<std::unique_ptr<RenderItem>>& mAllRitems)
{
	auto CopyColorItem = std::make_unique<RenderItem>();
	CopyColorItem->World = Mathf::Identity4x4();
	CopyColorItem->TexTransform = Mathf::Identity4x4();
	CopyColorItem->ObjCBIndex = 23;
	CopyColorItem->Mat = mMaterials["CopyColor"].get();
	CopyColorItem->Geo = mGeometries["shapeGeo"].get();
	CopyColorItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	CopyColorItem->IndexCount = CopyColorItem->Geo->DrawArgs["screenGrid"].IndexCount;
	CopyColorItem->StartIndexLocation = CopyColorItem->Geo->DrawArgs["screenGrid"].StartIndexLocation;
	CopyColorItem->BaseVertexLocation = CopyColorItem->Geo->DrawArgs["screenGrid"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::PostProcessing].push_back(CopyColorItem.get());
	mAllRitems.push_back(std::move(CopyColorItem));


	     

	auto RGBSplitItem = std::make_unique<RenderItem>();
	RGBSplitItem->World = Mathf::Identity4x4();
	RGBSplitItem->TexTransform = Mathf::Identity4x4();
	RGBSplitItem->ObjCBIndex = 24;
	RGBSplitItem->Mat = mMaterials["RGBSplit"].get();
	RGBSplitItem->Geo = mGeometries["shapeGeo"].get();
	RGBSplitItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	RGBSplitItem->IndexCount = RGBSplitItem->Geo->DrawArgs["screenGrid"].IndexCount;
	RGBSplitItem->StartIndexLocation = RGBSplitItem->Geo->DrawArgs["screenGrid"].StartIndexLocation;
	RGBSplitItem->BaseVertexLocation = RGBSplitItem->Geo->DrawArgs["screenGrid"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::PostProcessing].push_back(RGBSplitItem.get());
	mAllRitems.push_back(std::move(RGBSplitItem));




	auto RadialBlurItem = std::make_unique<RenderItem>();
	RadialBlurItem->World = Mathf::Identity4x4();
	RadialBlurItem->TexTransform = Mathf::Identity4x4();
	RadialBlurItem->ObjCBIndex = 25;
	RadialBlurItem->Mat = mMaterials["RadialBlur"].get();
	RadialBlurItem->Geo = mGeometries["shapeGeo"].get();
	RadialBlurItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	RadialBlurItem->IndexCount = RadialBlurItem->Geo->DrawArgs["screenGrid"].IndexCount;
	RadialBlurItem->StartIndexLocation = RadialBlurItem->Geo->DrawArgs["screenGrid"].StartIndexLocation;
	RadialBlurItem->BaseVertexLocation = RadialBlurItem->Geo->DrawArgs["screenGrid"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::PostProcessing].push_back(RadialBlurItem.get());
	mAllRitems.push_back(std::move(RadialBlurItem));




	auto VignetteItem = std::make_unique<RenderItem>();
	VignetteItem->World = Mathf::Identity4x4();
	VignetteItem->TexTransform = Mathf::Identity4x4();
	VignetteItem->ObjCBIndex = 26;
	VignetteItem->Mat = mMaterials["Vignette"].get();
	VignetteItem->Geo = mGeometries["shapeGeo"].get();
	VignetteItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	VignetteItem->IndexCount = VignetteItem->Geo->DrawArgs["screenGrid"].IndexCount;
	VignetteItem->StartIndexLocation = VignetteItem->Geo->DrawArgs["screenGrid"].StartIndexLocation;
	VignetteItem->BaseVertexLocation = VignetteItem->Geo->DrawArgs["screenGrid"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::PostProcessing].push_back(VignetteItem.get());
	mAllRitems.push_back(std::move(VignetteItem));



	auto DecolorItem = std::make_unique<RenderItem>();
	DecolorItem->World = Mathf::Identity4x4();
	DecolorItem->TexTransform = Mathf::Identity4x4();
	DecolorItem->ObjCBIndex = 27;
	DecolorItem->Mat = mMaterials["Decolor"].get();
	DecolorItem->Geo = mGeometries["shapeGeo"].get();
	DecolorItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	DecolorItem->IndexCount = DecolorItem->Geo->DrawArgs["screenGrid"].IndexCount;
	DecolorItem->StartIndexLocation = DecolorItem->Geo->DrawArgs["screenGrid"].StartIndexLocation;
	DecolorItem->BaseVertexLocation = DecolorItem->Geo->DrawArgs["screenGrid"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::PostProcessing].push_back(DecolorItem.get());
	mAllRitems.push_back(std::move(DecolorItem));



	auto BrightnessItem = std::make_unique<RenderItem>();
	BrightnessItem->World = Mathf::Identity4x4();
	BrightnessItem->TexTransform = Mathf::Identity4x4();
	BrightnessItem->ObjCBIndex = 28;
	BrightnessItem->Mat = mMaterials["Brightness"].get();
	BrightnessItem->Geo = mGeometries["shapeGeo"].get();
	BrightnessItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	BrightnessItem->IndexCount = BrightnessItem->Geo->DrawArgs["screenGrid"].IndexCount;
	BrightnessItem->StartIndexLocation = BrightnessItem->Geo->DrawArgs["screenGrid"].StartIndexLocation;
	BrightnessItem->BaseVertexLocation = BrightnessItem->Geo->DrawArgs["screenGrid"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::PostProcessing].push_back(BrightnessItem.get());
	mAllRitems.push_back(std::move(BrightnessItem));



	auto HSVItem = std::make_unique<RenderItem>();
	HSVItem->World = Mathf::Identity4x4();
	HSVItem->TexTransform = Mathf::Identity4x4();
	HSVItem->ObjCBIndex = 29;
	HSVItem->Mat = mMaterials["HSV"].get();
	HSVItem->Geo = mGeometries["shapeGeo"].get();
	HSVItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	HSVItem->IndexCount = HSVItem->Geo->DrawArgs["screenGrid"].IndexCount;
	HSVItem->StartIndexLocation = HSVItem->Geo->DrawArgs["screenGrid"].StartIndexLocation;
	HSVItem->BaseVertexLocation = HSVItem->Geo->DrawArgs["screenGrid"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::PostProcessing].push_back(HSVItem.get());
	mAllRitems.push_back(std::move(HSVItem));



	auto MosaicItem = std::make_unique<RenderItem>();
	MosaicItem->World = Mathf::Identity4x4();
	MosaicItem->TexTransform = Mathf::Identity4x4();
	MosaicItem->ObjCBIndex = 30;
	MosaicItem->Mat = mMaterials["Mosaic"].get();
	MosaicItem->Geo = mGeometries["shapeGeo"].get();
	MosaicItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	MosaicItem->IndexCount = MosaicItem->Geo->DrawArgs["screenGrid"].IndexCount;
	MosaicItem->StartIndexLocation = MosaicItem->Geo->DrawArgs["screenGrid"].StartIndexLocation;
	MosaicItem->BaseVertexLocation = MosaicItem->Geo->DrawArgs["screenGrid"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::PostProcessing].push_back(MosaicItem.get());
	mAllRitems.push_back(std::move(MosaicItem));



	auto SharpenItem = std::make_unique<RenderItem>();
	SharpenItem->World = Mathf::Identity4x4();
	SharpenItem->TexTransform = Mathf::Identity4x4();
	SharpenItem->ObjCBIndex = 31;
	SharpenItem->Mat = mMaterials["Sharpen"].get();
	SharpenItem->Geo = mGeometries["shapeGeo"].get();
	SharpenItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	SharpenItem->IndexCount = SharpenItem->Geo->DrawArgs["screenGrid"].IndexCount;
	SharpenItem->StartIndexLocation = SharpenItem->Geo->DrawArgs["screenGrid"].StartIndexLocation;
	SharpenItem->BaseVertexLocation = SharpenItem->Geo->DrawArgs["screenGrid"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::PostProcessing].push_back(SharpenItem.get());
	mAllRitems.push_back(std::move(SharpenItem));



	auto WhiteBalanceItem = std::make_unique<RenderItem>();
	WhiteBalanceItem->World = Mathf::Identity4x4();
	WhiteBalanceItem->TexTransform = Mathf::Identity4x4();
	WhiteBalanceItem->ObjCBIndex = 31;
	WhiteBalanceItem->Mat = mMaterials["WhiteBalance"].get();
	WhiteBalanceItem->Geo = mGeometries["shapeGeo"].get();
	WhiteBalanceItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	WhiteBalanceItem->IndexCount = WhiteBalanceItem->Geo->DrawArgs["screenGrid"].IndexCount;
	WhiteBalanceItem->StartIndexLocation = WhiteBalanceItem->Geo->DrawArgs["screenGrid"].StartIndexLocation;
	WhiteBalanceItem->BaseVertexLocation = WhiteBalanceItem->Geo->DrawArgs["screenGrid"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::PostProcessing].push_back(WhiteBalanceItem.get());
	mAllRitems.push_back(std::move(WhiteBalanceItem));




	auto OilPaintItem = std::make_unique<RenderItem>();
	OilPaintItem->World = Mathf::Identity4x4();
	OilPaintItem->TexTransform = Mathf::Identity4x4();
	OilPaintItem->ObjCBIndex = 32;
	OilPaintItem->Mat = mMaterials["OilPaint"].get();
	OilPaintItem->Geo = mGeometries["shapeGeo"].get();
	OilPaintItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	OilPaintItem->IndexCount = OilPaintItem->Geo->DrawArgs["screenGrid"].IndexCount;
	OilPaintItem->StartIndexLocation = OilPaintItem->Geo->DrawArgs["screenGrid"].StartIndexLocation;
	OilPaintItem->BaseVertexLocation = OilPaintItem->Geo->DrawArgs["screenGrid"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::PostProcessing].push_back(OilPaintItem.get());
	mAllRitems.push_back(std::move(OilPaintItem));




	auto ReliefItem = std::make_unique<RenderItem>();
	ReliefItem->World = Mathf::Identity4x4();
	ReliefItem->TexTransform = Mathf::Identity4x4();
	ReliefItem->ObjCBIndex = 33;
	ReliefItem->Mat = mMaterials["Relief"].get();
	ReliefItem->Geo = mGeometries["shapeGeo"].get();
	ReliefItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	ReliefItem->IndexCount = ReliefItem->Geo->DrawArgs["screenGrid"].IndexCount;
	ReliefItem->StartIndexLocation = ReliefItem->Geo->DrawArgs["screenGrid"].StartIndexLocation;
	ReliefItem->BaseVertexLocation = ReliefItem->Geo->DrawArgs["screenGrid"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::PostProcessing].push_back(ReliefItem.get());
	mAllRitems.push_back(std::move(ReliefItem));




	auto EdgeDetectionItem = std::make_unique<RenderItem>();
	EdgeDetectionItem->World = Mathf::Identity4x4();
	EdgeDetectionItem->TexTransform = Mathf::Identity4x4();
	EdgeDetectionItem->ObjCBIndex = 34;
	EdgeDetectionItem->Mat = mMaterials["EdgeDetection"].get();
	EdgeDetectionItem->Geo = mGeometries["shapeGeo"].get();
	EdgeDetectionItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	EdgeDetectionItem->IndexCount = EdgeDetectionItem->Geo->DrawArgs["screenGrid"].IndexCount;
	EdgeDetectionItem->StartIndexLocation = EdgeDetectionItem->Geo->DrawArgs["screenGrid"].StartIndexLocation;
	EdgeDetectionItem->BaseVertexLocation = EdgeDetectionItem->Geo->DrawArgs["screenGrid"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::PostProcessing].push_back(EdgeDetectionItem.get());
	mAllRitems.push_back(std::move(EdgeDetectionItem));




	auto FinalBlitItem = std::make_unique<RenderItem>();
	FinalBlitItem->World = Mathf::Identity4x4();
	FinalBlitItem->TexTransform = Mathf::Identity4x4();
	FinalBlitItem->ObjCBIndex = 35;
	FinalBlitItem->Mat = mMaterials["CopyColor"].get();
	FinalBlitItem->Geo = mGeometries["shapeGeo"].get();
	FinalBlitItem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	FinalBlitItem->IndexCount = FinalBlitItem->Geo->DrawArgs["screenGrid"].IndexCount;
	FinalBlitItem->StartIndexLocation = FinalBlitItem->Geo->DrawArgs["screenGrid"].StartIndexLocation;
	FinalBlitItem->BaseVertexLocation = FinalBlitItem->Geo->DrawArgs["screenGrid"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::PostProcessing].push_back(FinalBlitItem.get());
	mAllRitems.push_back(std::move(FinalBlitItem));
}