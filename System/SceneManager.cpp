#include "SceneManager.h"
const int ProjectTexStart = 14;

SceneManager::SceneManager(SceneType tp)
{
	Type = tp;
}


//	构建场景
void SceneManager::BuildScene(
	std::unordered_map<std::string, std::unique_ptr<Texture2D>>& mResourcesTextures,
	MaterialIndexUtils& matCBIndexUtils
)
{
	switch (Type)
	{
	case SceneType::Default:BuildDefaultScene(mResourcesTextures, matCBIndexUtils);
		break;
	case SceneType::ConelBox:BuildConelBoxScene(mResourcesTextures, matCBIndexUtils);
		break;
	case SceneType::Toon:BuildToonScene(mResourcesTextures, matCBIndexUtils);
		break;
	case SceneType::Water:BuildWaterScene(mResourcesTextures, matCBIndexUtils);
		break;
	default:BuildDefaultScene(mResourcesTextures, matCBIndexUtils);
		break;
	}
}

void SceneManager::BuildDefaultScene(
	std::unordered_map<std::string, std::unique_ptr<Texture2D>>& mResourcesTextures,
	MaterialIndexUtils& matCBIndexUtils
)
{
	//	设为主光源
	mMainLight.isMainLight = true;
	//	灯光颜色
	mMainLight.color = Color(1, 0.9568627f, 0.8392157f, 1);
	//	设置灯光位置
	mMainLight.position = Vector3(0, 3, 0);
	//	设置灯光欧拉角
	mMainLight.eulerangle = Vector3(50, -30, 0);
	//	主光源名字
	mMainLight.name = "Direction Light";

	//	天空球设置
	mSkyBoxSetting.Tint = Color(1.0f, 1.0f, 1.0f, 1.0f);
	mSkyBoxSetting.Exposure = 1.0f;
	mSkyBoxSetting.Rotation = 0.0f;
	mSkyBoxSetting.ACES = 1.0f;

	//	灯光设置 
	mLightSetting.ShadowColor = Color(0.3f, 0.3f, 0.3f, 1.0f);
	mLightSetting.AmbientColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
	mLightSetting.UseLinearFog = 0;
	mLightSetting.Start = 0;
	mLightSetting.End = 0;
	mLightSetting.UseHieghtFog = 0;
	mLightSetting.HeightMin = 0;
	mLightSetting.HeightMax = 0;
	mLightSetting.Feather = 0;
	mLightSetting.Step = 0;

	//	相机设置
	mCameraSetting.mCamFov = 45;
	mCameraSetting.mCamClipN = 0.3;
	mCameraSetting.mCamClipF = 3000;
	mCameraSetting.renderSkyBox = true;


	//------------------------------
	//	SkyBox
	//------------------------------
	// 
	//	存储类型matCB开始值
	matCBIndexUtils.getInstance().SaveTypeIndex("Sky", matCBIndexUtils.getInstance().GetIndex());

	auto sky = std::make_unique<MeshRender>();
	sky->name = "DGarden";
	//构建材质
	sky->material.Name = "DGarden_mat";
	sky->material.MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	sky->material.Tint = mSkyBoxSetting.Tint;
	sky->material.Exposure = mSkyBoxSetting.Exposure;
	sky->material.Rotation = mSkyBoxSetting.Rotation;
	sky->material.ACES = mSkyBoxSetting.ACES;

	//	创建平面网格
	sky->mesh.CreateSphere(0.5f, 20, 20);

	//	设置坐标
	sky->position = Vector3(0, 0, 0);
	sky->eulerangle = Vector3(0, 0, 0);
	sky->scale = Vector3(5000, 5000, 5000);

	//	创建碰撞盒
	sky->bound.aabb = BoundingAABB();

	mMeshRender.push_back(std::move(sky));


	//------------------------------
	//	物体的MeshRender
	//------------------------------
	// 
	//	存储类型matCB开始值
	matCBIndexUtils.getInstance().SaveTypeIndex("Opaque", matCBIndexUtils.getInstance().GetIndex());

	auto sphere = std::make_unique<MeshRender>();
	sphere->name = "ak47_render";

	//构建材质
	sphere->material.Name = "ak47";
	sphere->material.MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	sphere->material.DiffuseColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
	sphere->material.Smoothness = 1.0f;
	sphere->material.Metallic = 1.0f;
	sphere->material.Occlusion = 1.0f;
	sphere->material.EmissionColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
	sphere->material.EmissionStrength = 0.0f;
	sphere->material.DiffuseMapIndex = mResourcesTextures["AK47Albedo"]->TexIndex;
	sphere->material.NormalMapIndex =  mResourcesTextures["AK47Normal"]->TexIndex;
	sphere->material.MsoMapIndex = mResourcesTextures["AK47MSO"]->TexIndex;
	sphere->material.EmissionMapIndex = mResourcesTextures["black"]->TexIndex;
	sphere->material.LUTMapIndex = mResourcesTextures["sampleLUT"]->TexIndex;

	//	创建球网格
	ObjLoader::LoadObj(sphere->mesh.data, "F:/Engine/DistEngine/DistEngine/Asset/Mesh/ak47.obj");

	//	设置坐标
	sphere->position = Vector3(0, 0, 0);
	sphere->eulerangle = Vector3(0, 0, 0);
	sphere->scale = Vector3(3, 3, 3);

	//	创建碰撞盒
	sphere->bound.aabb = BoundingAABB(sphere->mesh.data);

	//	设置AABB的世界坐标
	sphere->bound.aabb.m_min = sphere->GetWorldMatrix() * sphere->bound.aabb.m_min;
	sphere->bound.aabb.m_max = sphere->GetWorldMatrix() * sphere->bound.aabb.m_max;
	mMeshRender.push_back(std::move(sphere));
}

void SceneManager::BuildConelBoxScene(
	std::unordered_map<std::string, std::unique_ptr<Texture2D>>& mResourcesTextures,
	MaterialIndexUtils& matCBIndexUtils
)
{

}

void SceneManager::BuildToonScene(
	std::unordered_map<std::string, std::unique_ptr<Texture2D>>& mResourcesTextures,
	MaterialIndexUtils& matCBIndexUtils
)
{

}

void SceneManager::BuildWaterScene(
	std::unordered_map<std::string, std::unique_ptr<Texture2D>>& mResourcesTextures,
	MaterialIndexUtils& matCBIndexUtils
)
{

}


//	更新场景材质
void SceneManager::UpdateSceneMaterialBuffer(
	UploadBuffer<PBRMaterialData>* PBRMaterialBuffer, 
	UploadBuffer<SkyBoxMaterialData>* SkyMaterialBuffer,
	MaterialIndexUtils& matCBIndexUtils
)
{
	switch (Type)
	{
	case SceneType::Default:UpdateDefaultSceneMaterialBuffer(PBRMaterialBuffer, SkyMaterialBuffer, matCBIndexUtils);
		break;
	case SceneType::ConelBox:UpdateConelBoxSceneMaterialBuffer(PBRMaterialBuffer, SkyMaterialBuffer, matCBIndexUtils);
		break;
	case SceneType::Toon:UpdateToonSceneMaterialBuffer(PBRMaterialBuffer, SkyMaterialBuffer, matCBIndexUtils);
		break;
	case SceneType::Water:UpdateWaterSceneMaterialBuffer(PBRMaterialBuffer, SkyMaterialBuffer, matCBIndexUtils);
		break;
	default:UpdateDefaultSceneMaterialBuffer(PBRMaterialBuffer, SkyMaterialBuffer, matCBIndexUtils);
		break;
	}
}

void SceneManager::UpdateDefaultSceneMaterialBuffer(
	UploadBuffer<PBRMaterialData>* PBRMaterialBuffer, 
	UploadBuffer<SkyBoxMaterialData>* SkyMaterialBuffer,
	MaterialIndexUtils& matCBIndexUtils
)
{
	for (size_t i = 0; i < mMeshRender.size(); i++)
	{
		Material* mat = &(mMeshRender[i]->material);
		if (mMeshRender[i]->material.MatCBIndex <= matCBIndexUtils.getInstance().GetTypeIndexStart("Sky"))
		{
			SkyBoxMaterialData matData;
			matData.Tint = Vector4(mSkyBoxSetting.Tint);
			matData.Exposure = mSkyBoxSetting.Exposure;
			matData.Rotation = mSkyBoxSetting.Rotation;
			matData.ACES = mSkyBoxSetting.ACES;
			SkyMaterialBuffer->CopyData(mat->MatCBIndex, matData);

			// Next FrameResource need to be updated too.
			mat->NumFramesDirty--;
		}
		else
		{
			XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);

			PBRMaterialData matData;
			matData.DiffuseColor = Vector4(mat->DiffuseColor);
			matData.Smoothness = mat->Smoothness;
			matData.Metallic = mat->Metallic;
			matData.Occlusion = mat->Occlusion;
			matData.EmissionColor = Vector4(mat->EmissionColor);
			matData.EmissionStrength = mat->EmissionStrength;
			XMStoreFloat4x4(&matData.MatTransform, XMMatrixTranspose(matTransform));
			matData.DiffuseMapIndex = mat->DiffuseMapIndex;
			matData.NormalMapIndex = mat->NormalMapIndex;
			matData.MsoMapIndex = mat->MsoMapIndex;
			matData.EmissionMapIndex = mat->EmissionMapIndex;
			matData.LUTMapIndex = mat->LUTMapIndex;
			PBRMaterialBuffer->CopyData(mat->MatCBIndex, matData);

			// Next FrameResource need to be updated too.
			mat->NumFramesDirty--;
		}
	}
}

void SceneManager::UpdateConelBoxSceneMaterialBuffer(
	UploadBuffer<PBRMaterialData>* PBRMaterialBuffer, 
	UploadBuffer<SkyBoxMaterialData>* SkyMaterialBuffer,
	MaterialIndexUtils& matCBIndexUtils
)
{

}

void SceneManager::UpdateToonSceneMaterialBuffer(
	UploadBuffer<PBRMaterialData>* PBRMaterialBuffer, 
	UploadBuffer<SkyBoxMaterialData>* SkyMaterialBuffer,
	MaterialIndexUtils& matCBIndexUtils
)
{

}

void SceneManager::UpdateWaterSceneMaterialBuffer(UploadBuffer<PBRMaterialData>* PBRMaterialBuffer, 
	UploadBuffer<SkyBoxMaterialData>* SkyMaterialBuffer,
	MaterialIndexUtils& matCBIndexUtils
)
{

}




//	创建RenderItem
void SceneManager::BuildRenderItem(
	std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count], 
	std::vector<std::unique_ptr<RenderItem>>& mAllRitems, 
	Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, 
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList, 
	UINT CurrentSize,
	MaterialIndexUtils& matCBIndexUtils
)
{
	for (size_t i = 0; i < mMeshRender.size(); i++)
	{
		auto Ritem = std::make_unique<RenderItem>();
		Ritem->World = mMeshRender[i]->GetWorldXMMatrix();
		Ritem->TexTransform = Mathf::Identity4x4();
		Ritem->ObjCBIndex = CurrentSize + i;
		Ritem->Mat = &mMeshRender[i]->material;
		Ritem->Geo = GraphicsUtils::BuidlMeshGeometryFromMeshData(mMeshRender[i]->name, mMeshRender[i]->mesh.data, md3dDevice, mCommandList);
		Ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		Ritem->IndexCount = Ritem->Geo->DrawArgs["mesh"].IndexCount;
		Ritem->StartIndexLocation = Ritem->Geo->DrawArgs["mesh"].StartIndexLocation;
		Ritem->BaseVertexLocation = Ritem->Geo->DrawArgs["mesh"].BaseVertexLocation;

		if (mMeshRender[i]->material.MatCBIndex <= matCBIndexUtils.getInstance().GetTypeIndexStart("Sky"))
		{
			mRitemLayer[(int)RenderLayer::Sky].push_back(Ritem.get());
		}
		else
		{
			mRitemLayer[(int)RenderLayer::Opaque].push_back(Ritem.get());
		}
		mAllRitems.push_back(std::move(Ritem));
	}
}