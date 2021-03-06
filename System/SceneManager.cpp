#include "SceneManager.h"
#include "ResourcesPath.h"

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
	mMainLight.position = Vector3(-20, 30, 20);
	//	设置灯光欧拉角
	mMainLight.eulerangle = Vector3(30, -60, -50);
	//	设置forward
	mMainLight.forward = Vector3(0.57f, -0.57f, 0.57f);
	//	主光源名字
	mMainLight.name = "Direction Light";

	//	天空球设置
	mSkyBoxSetting.Tint = Color(1.0f, 1.0f, 1.0f, 1.0f);
	mSkyBoxSetting.Exposure = 1.0f;
	mSkyBoxSetting.Rotation = 0.0f;
	mSkyBoxSetting.ACES = 0.0f;
	mSkyBoxSetting.EnableScatteringSky = 1;
	mSkyBoxSetting.SunHeight = 0.45f;
	mSkyBoxSetting.SkyColor = Vector4(0.39f, 0.57f, 1.0f, 1.0f);
	mSkyBoxSetting.ZenithOffset = 0.1f;
	mSkyBoxSetting.Density = 0.7f;
	mSkyBoxSetting.MultiScatterPhase = 0.1f;
	mSkyBoxSetting.AnisotropicIntensity = 0;

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
	mCameraSetting.mCamClipF = 1000;
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
	sky->bound.aabb = BoundingAABB(sky->mesh.data);
	sky->Enable = true;
	mMeshRender.push_back(std::move(sky));


	//------------------------------
	//	物体的MeshRender
	//------------------------------
	// 
	//	存储类型matCB开始值
	matCBIndexUtils.getInstance().SaveTypeIndex("Opaque", matCBIndexUtils.getInstance().GetIndex());

	auto model_ak47 = std::make_unique<MeshRender>();
	model_ak47->name = "ak47_render";

	//构建材质
	model_ak47->material.Name = "ak47";
	model_ak47->material.MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	model_ak47->material.DiffuseColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
	model_ak47->material.Smoothness = 1.0f;
	model_ak47->material.Metallic = 1.0f;
	model_ak47->material.Occlusion = 1.0f;
	model_ak47->material.EmissionColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
	model_ak47->material.EmissionStrength = 0.0f;
	model_ak47->material.DiffuseMapIndex = mResourcesTextures["AK47Albedo"]->TexIndex;
	model_ak47->material.NormalMapIndex =  mResourcesTextures["AK47Normal"]->TexIndex;
	model_ak47->material.MsoMapIndex = mResourcesTextures["AK47MSO"]->TexIndex;
	model_ak47->material.EmissionMapIndex = mResourcesTextures["white"]->TexIndex;
	model_ak47->material.LUTMapIndex = mResourcesTextures["sampleLUT"]->TexIndex;

	//	创建球网格
	ObjLoader::LoadObj(model_ak47->mesh.data, (char*)mAk47ObjPath.c_str());

	//	设置坐标
	model_ak47->position = Vector3(0, 0, 0);
	model_ak47->eulerangle = Vector3(0, 0, 0);
	model_ak47->scale = Vector3(1, 1, 1);

	//	创建碰撞盒
	model_ak47->bound.aabb = BoundingAABB(model_ak47->mesh.data);
	model_ak47->Enable = true;
	//	设置AABB的世界坐标
	model_ak47->bound.aabb.m_min = model_ak47->GetWorldMatrix() * model_ak47->bound.aabb.m_min;
	model_ak47->bound.aabb.m_max = model_ak47->GetWorldMatrix() * model_ak47->bound.aabb.m_max;
	mMeshRender.push_back(std::move(model_ak47));
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
		//	创建渲染项
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
		Ritem->Bound = mMeshRender[i]->bound.aabb.ToBoundBox();
		Ritem->Enable = mMeshRender[i]->Enable;

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

/// <summary>
/// 更新CBuffer
/// </summary>
void SceneManager::UpdateObjectBuffer(std::vector<std::unique_ptr<RenderItem>>& mAllRitems, UINT CurrentSize)
{
	for (size_t i = 0; i < mAllRitems.size(); i++)
	{
		for (size_t j = 0; j < mMeshRender.size(); j++)
		{
			if (mAllRitems[i]->ObjCBIndex == (CurrentSize + j))
			{
				mAllRitems[i]->World = mMeshRender[j]->GetWorldXMMatrix();
				mAllRitems[i]->Enable = mMeshRender[j]->Enable;
			}
		}
	}
}