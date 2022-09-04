#include "SceneManager.h"
#include "../File/ResourcesPath.h"

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
	//	创建设置全局设置
	BuildGlobalConfig();

	//	创建主光源
	BuildMainLight();

	//	创建天空盒
	BuildSkyBox(matCBIndexUtils);

	//	渲染项
	BuildRenderObejct(mResourcesTextures, matCBIndexUtils);
}

/// <summary>
/// 全局设置
/// </summary>
void SceneManager::BuildGlobalConfig()
{
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
}

void SceneManager::BuildMainLight()
{
	//	读取JSON
	//nl::json jsonValue;
	//ifstream jfile(mScenePath[0]);
	//jfile >> jsonValue;
	//SceneData data;
	//nl::from_json(jsonValue, data);

	Transform* mTransform = new Transform("Transform");
	//	设置灯光位置
	mTransform->position = Vector3(-20, 30, 20);
	//	设置灯光欧拉角
	mTransform->eulerangle = Vector3(30, -60, -50);
	//	设置forward
	mTransform->forward = Vector3(0.57f, -0.57f, 0.57f);
	MainLight->AddComponent(mTransform);

	DirectionLight* mDirectionLight = new DirectionLight("DirectionLight");
	//	设为主光源
	mDirectionLight->isMainLight = true;
	//	灯光颜色
	mDirectionLight->color = Color(1, 0.9568627f, 0.8392157f, 1);
	//	灯光强度
	mDirectionLight->intensity = 1.0f;

	MainLight->AddComponent(mDirectionLight);

	//	主光源名字
	MainLight->name = "Direction Light";
}

/// <summary>
/// 天空球
/// </summary>
/// <param name="matCBIndexUtils"></param>
void SceneManager::BuildSkyBox(MaterialIndexUtils& matCBIndexUtils)
{
	//	存储类型matCB开始值
	matCBIndexUtils.getInstance().SaveTypeIndex("Sky", matCBIndexUtils.getInstance().GetIndex());

	auto skyRenderObject = std::make_unique<GameObject>("sky");

	//	Build Material
	Material* mat_sky = new Material();
	mat_sky->Name = "SkyBoxMaterial";
	mat_sky->MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	mat_sky->Tint = mSkyBoxSetting.Tint;
	mat_sky->Exposure = mSkyBoxSetting.Exposure;
	mat_sky->Rotation = mSkyBoxSetting.Rotation;
	mat_sky->ACES = mSkyBoxSetting.ACES;

	//	加载模型
	MeshFliter* mesh_sky = new MeshFliter("MeshFliter");
	mesh_sky->CreateSphere(0.5f, 20.0f, 20.0f);
	MeshRender* meshRender_sky = new MeshRender(mesh_sky, mat_sky, "MeshRender");

	//	设置坐标
	Transform* transform_sky = new Transform("Transform");
	transform_sky->position = Vector3(0, 0, 0);
	transform_sky->eulerangle = Vector3(0, 0, 0);
	transform_sky->scale = Vector3(5000, 5000, 5000);

	//	创建碰撞盒子
	DistBound::BoundingBox* bound_sky = new DistBound::BoundingBox("BoundingBox");
	bound_sky->aabb = BoundingAABB(mesh_sky->data);

	skyRenderObject->AddComponent(transform_sky);
	skyRenderObject->AddComponent(meshRender_sky);
	skyRenderObject->AddComponent(bound_sky);

	skyRenderObject->Enable = true;

	mRenderObjects.push_back(std::move(skyRenderObject));
}

/// <summary>
/// 渲染Item
/// </summary>
/// <param name="mResourcesTextures"></param>
/// <param name="matCBIndexUtils"></param>
void SceneManager::BuildRenderObejct(
	std::unordered_map<std::string, std::unique_ptr<Texture2D>>& mResourcesTextures,
	MaterialIndexUtils& matCBIndexUtils
)
{
	////	读取场景JSON
	//nl::json sceneData;
	//ifstream sceneFile("Assets/Scene/default.scene");
	//sceneFile >> sceneData;
	//SceneData scene_data;
	//nl::from_json(sceneData, scene_data);

	////	读取材质JSON
	//nl::json matData;
	//ifstream matFile(scene_data.MaterialPath);
	//matFile >> matData;
	//MaterialLitData mat_data;
	//nl::from_json(matData, mat_data);



	matCBIndexUtils.getInstance().SaveTypeIndex("Opaque", matCBIndexUtils.getInstance().GetIndex());

	auto RenderObject = std::make_unique<GameObject>("ak47");

	// Build Material
	Material* mat_ak47 = new Material();
	mat_ak47->Name = "ak47";
	mat_ak47->MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	mat_ak47->DiffuseColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
	mat_ak47->Smoothness = 1.0f;
	mat_ak47->Metallic = 1.0f;
	mat_ak47->Occlusion = 1.0f;
	mat_ak47->EmissionColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
	mat_ak47->EmissionStrength =0.0f;
	mat_ak47->DiffuseMapIndex = mResourcesTextures["AK47Albedo"]->TexIndex;
	mat_ak47->NormalMapIndex = mResourcesTextures["AK47Normal"]->TexIndex;
	mat_ak47->MsoMapIndex = mResourcesTextures["AK47MSO"]->TexIndex;
	mat_ak47->EmissionMapIndex = mResourcesTextures["white"]->TexIndex;
	mat_ak47->LUTMapIndex = mResourcesTextures["sampleLUT"]->TexIndex;

	//	加载模型
	MeshFliter* mesh_ak47 = new MeshFliter("MeshFliter");
	ObjLoader::LoadObj(mesh_ak47->data, (char*)mAk47ObjPath.c_str());
	MeshRender* meshRender_ak47 = new MeshRender(mesh_ak47, mat_ak47, "MeshRender");

	//	设置坐标
	Transform* transform_ak47 = new Transform("Transform");
	transform_ak47->position = Vector3(0, 0, 0);
	transform_ak47->eulerangle = Vector3(0, 0, 0);
	transform_ak47->scale = Vector3(1, 1, 1);

	//	创建碰撞盒子
	DistBound::BoundingBox* bound_ak47 = new DistBound::BoundingBox("BoundingBox");
	bound_ak47->aabb = BoundingAABB(mesh_ak47->data);

	RenderObject->AddComponent(transform_ak47);
	RenderObject->AddComponent(meshRender_ak47);
	RenderObject->AddComponent(bound_ak47);
	RenderObject->Enable = true;

	mRenderObjects.push_back(std::move(RenderObject));
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
	for (size_t i = 0; i < mRenderObjects.size(); i++)
	{
		Material* mat = mRenderObjects[i]->GetComponent<MeshRender>(2)->mat;
		if (mat->MatCBIndex <= matCBIndexUtils.getInstance().GetTypeIndexStart("Sky"))
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
	for (size_t i = 0; i < mRenderObjects.size(); i++)
	{
		//	创建渲染项
		auto Ritem = std::make_unique<RenderItem>();
		Ritem->World = mRenderObjects[i]->GetComponent<Transform>(1)->GetWorldXMMatrix();
		Ritem->TexTransform = Mathf::Identity4x4();
		Ritem->ObjCBIndex = CurrentSize + i;
		Ritem->Mat = mRenderObjects[i]->GetComponent<MeshRender>(2)->mat;
		Ritem->Geo = GraphicsUtils::BuidlMeshGeometryFromMeshData(mRenderObjects[i]->name, mRenderObjects[i]->GetComponent<MeshRender>(2)->mesh->data, md3dDevice, mCommandList);
		Ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		Ritem->IndexCount = Ritem->Geo->DrawArgs["mesh"].IndexCount;
		Ritem->StartIndexLocation = Ritem->Geo->DrawArgs["mesh"].StartIndexLocation;
		Ritem->BaseVertexLocation = Ritem->Geo->DrawArgs["mesh"].BaseVertexLocation;
		Ritem->Bound = mRenderObjects[i]->GetComponent<DistBound::BoundingBox>(3)->aabb.ToBoundBox();
		Ritem->Enable = mRenderObjects[i]->Enable;

		if (mRenderObjects[i]->GetComponent<MeshRender>(2)->mat->MatCBIndex <= matCBIndexUtils.getInstance().GetTypeIndexStart("Sky"))
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
		for (size_t j = 0; j < mRenderObjects.size(); j++)
		{
			if (mAllRitems[i]->ObjCBIndex == (CurrentSize + j))
			{
				mAllRitems[i]->World = mRenderObjects[j]->GetComponent<Transform>(1)->GetWorldXMMatrix();
				mAllRitems[i]->Enable = mRenderObjects[j]->Enable;
			}
		}
	}
}