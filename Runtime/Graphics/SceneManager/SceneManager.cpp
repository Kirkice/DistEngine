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
	nl::json jsonValue;
	ifstream jfile(mScenePath[0]);
	jfile >> jsonValue;

	SceneData data;
	nl::from_json(jsonValue, data);

	Transform* mTransform = new Transform("Transform");
	//	设置灯光位置
	mTransform->position = Vector3(data.MainLightPosition[0], data.MainLightPosition[1], data.MainLightPosition[2]);
	//	设置灯光欧拉角
	mTransform->eulerangle = Vector3(data.MainLightEulerangle[0], data.MainLightEulerangle[1], data.MainLightEulerangle[2]);
	//	设置forward
	mTransform->forward = Vector3(data.MainLightForward[0], data.MainLightForward[1], data.MainLightForward[2]);
	MainLight->AddComponent(mTransform);

	DirectionLight* mDirectionLight = new DirectionLight("DirectionLight");
	//	设为主光源
	mDirectionLight->isMainLight = data.isMainLight;
	//	灯光颜色
	mDirectionLight->color = Color(data.MainLightColor[0], data.MainLightColor[1], data.MainLightColor[2], data.MainLightColor[3]);
	//	灯光强度
	mDirectionLight->intensity = 1.0;

	MainLight->AddComponent(mDirectionLight);

	//	主光源名字
	MainLight->name = data.MainLightName;
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
	skyRenderObject->name = "sky";
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
	//	读取场景JSON
	nl::json sceneData;
	ifstream sceneFile(mScenePath[0]);
	sceneFile >> sceneData;
	SceneData scene_data;
	nl::from_json(sceneData, scene_data);

	//	读取材质JSON
	nl::json matData;
	ifstream matFile(scene_data.MaterialPath);
	matFile >> matData;
	MaterialLitData mat_data;
	nl::from_json(matData, mat_data);

	matCBIndexUtils.getInstance().SaveTypeIndex("Opaque", matCBIndexUtils.getInstance().GetIndex());

	auto RenderObjectAk47 = std::make_unique<GameObject>("ak47");

	// Build Material
	Material* mat_ak47 = new Material();
	mat_ak47->Name = mat_data.Name;
	mat_ak47->MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	mat_ak47->DiffuseColor = Color(mat_data.DiffuseColor[0], mat_data.DiffuseColor[1], mat_data.DiffuseColor[2], mat_data.DiffuseColor[3]);
	mat_ak47->Smoothness = mat_data.Smoothness;
	mat_ak47->Metallic = mat_data.Metallic;
	mat_ak47->Occlusion = mat_data.Occlusion;
	mat_ak47->EmissionColor = Color(mat_data.EmissionColor[0], mat_data.EmissionColor[1], mat_data.EmissionColor[2], mat_data.EmissionColor[3]);
	mat_ak47->EmissionStrength = mat_data.EmissionStrength;
	mat_ak47->UseNormalMap = mat_data.UseNormalMap;
	mat_ak47->NormalScale = mat_data.NormalScale;
	mat_ak47->DiffuseMapIndex = mat_data.DiffuseMapIndex;
	mat_ak47->NormalMapIndex = mat_data.NormalMapIndex;
	mat_ak47->MsoMapIndex = mat_data.MsoMapIndex;
	mat_ak47->EmissionMapIndex = mat_data.EmissionMapIndex;
	mat_ak47->LUTMapIndex = mat_data.LUTMapIndex;

	//	加载模型
	MeshFliter* mesh_ak47 = new MeshFliter("MeshFliter");
	ObjLoader::LoadObj(mesh_ak47->data, (char*)scene_data.MeshPath.c_str());
	MeshRender* meshRender_ak47 = new MeshRender(mesh_ak47, mat_ak47, "MeshRender");

	//	设置坐标
	Transform* transform_ak47 = new Transform("Transform");
	transform_ak47->position = Vector3(scene_data.RenderObjectPosition[0], scene_data.RenderObjectPosition[1], scene_data.RenderObjectPosition[2]);
	transform_ak47->eulerangle = Vector3(scene_data.RenderObjectEulerAngle[0], scene_data.RenderObjectEulerAngle[1], scene_data.RenderObjectEulerAngle[2]);
	transform_ak47->scale = Vector3(scene_data.RenderObjectScale[0], scene_data.RenderObjectScale[1], scene_data.RenderObjectScale[2]);

	//	创建碰撞盒子
	DistBound::BoundingBox* bound_ak47 = new DistBound::BoundingBox("BoundingBox");
	bound_ak47->aabb = BoundingAABB(mesh_ak47->data);

	RenderObjectAk47->AddComponent(transform_ak47);
	RenderObjectAk47->AddComponent(meshRender_ak47);
	RenderObjectAk47->AddComponent(bound_ak47);
	RenderObjectAk47->Enable = true;

	RenderObjectAk47->Enable = true;
	RenderObjectAk47->name = scene_data.RenderObjectName;
	mRenderObjects.push_back(std::move(RenderObjectAk47));









	auto RenderObjectBunny = std::make_unique<GameObject>("Bunny");
	// Build Material
	Material* mat_Bunny = new Material();
	mat_Bunny->Name = "Bunny";
	mat_Bunny->MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	mat_Bunny->DiffuseColor = Color(1.0f, 0.0f, 0.0f, 1.0f);
	mat_Bunny->Smoothness = 0.54f;
	mat_Bunny->Metallic = 0.78f;
	mat_Bunny->Occlusion = 1.0;
	mat_Bunny->EmissionColor = Color(0.0f,0.0f,0.0f,0.0f);
	mat_Bunny->EmissionStrength = 0.0f;
	mat_Bunny->UseNormalMap = 0;
	mat_Bunny->NormalScale = 1;
	mat_Bunny->DiffuseMapIndex = mResourcesTextures["white"]->TexIndex;
	mat_Bunny->NormalMapIndex = mResourcesTextures["white"]->TexIndex;
	mat_Bunny->MsoMapIndex = mResourcesTextures["white"]->TexIndex;
	mat_Bunny->EmissionMapIndex = mResourcesTextures["white"]->TexIndex;
	mat_Bunny->LUTMapIndex = mResourcesTextures["sampleLUT"]->TexIndex;

	//	加载模型
	MeshFliter* mesh_Bunny = new MeshFliter("MeshFliter");
	ObjLoader::LoadObj(mesh_Bunny->data, (char*)mBunnyObjPath.c_str());
	MeshRender* meshRender_Bunny = new MeshRender(mesh_Bunny, mat_Bunny, "MeshRender");

	//	设置坐标
	Transform* transform_Bunny = new Transform("Transform");
	transform_Bunny->position = Vector3(0, -0.14,1);
	transform_Bunny->eulerangle = Vector3(0,-60,0);
	transform_Bunny->scale = Vector3(8,8,8);

	//	创建碰撞盒子
	DistBound::BoundingBox* bound_Bunny = new DistBound::BoundingBox("BoundingBox");
	bound_Bunny->aabb = BoundingAABB(mesh_Bunny->data);

	RenderObjectBunny->AddComponent(transform_Bunny);
	RenderObjectBunny->AddComponent(meshRender_Bunny);
	RenderObjectBunny->AddComponent(bound_Bunny);
	RenderObjectBunny->Enable = true;

	RenderObjectBunny->name = "Bunny";
	mRenderObjects.push_back(std::move(RenderObjectBunny));




	auto RenderObjectSphere01 = std::make_unique<GameObject>("Sphere01");
	// Build Material
	Material* mat_Sphere = new Material();
	mat_Sphere->Name = "Sphere01";
	mat_Sphere->MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	mat_Sphere->DiffuseColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
	mat_Sphere->Smoothness = 0.1f;
	mat_Sphere->Metallic = 0.1f;
	mat_Sphere->Occlusion = 1.0;
	mat_Sphere->EmissionColor = Color(1.0f, 0.0f, 0.0f, 0.0f);
	mat_Sphere->EmissionStrength = 0.0f;
	mat_Sphere->UseNormalMap = 0;
	mat_Sphere->NormalScale = 1;
	mat_Sphere->DiffuseMapIndex = mResourcesTextures["white"]->TexIndex;
	mat_Sphere->NormalMapIndex = mResourcesTextures["white"]->TexIndex;
	mat_Sphere->MsoMapIndex = mResourcesTextures["white"]->TexIndex;
	mat_Sphere->EmissionMapIndex = mResourcesTextures["white"]->TexIndex;
	mat_Sphere->LUTMapIndex = mResourcesTextures["sampleLUT"]->TexIndex;

	//	加载模型
	MeshFliter* mesh_Sphere01 = new MeshFliter("MeshFliter");
	mesh_Sphere01->CreateSphere(1.0f, 20.0f, 20.0f);

	MeshRender* meshRender_Sphere01 = new MeshRender(mesh_Sphere01, mat_Sphere, "MeshRender");

	//	设置坐标
	Transform* transform_Sphere01 = new Transform("Transform");
	transform_Sphere01->position = Vector3(-2.0f, 1.0f, -1.0f);
	transform_Sphere01->eulerangle = Vector3(0, 0, 0);
	transform_Sphere01->scale = Vector3(2, 2, 2);

	//	创建碰撞盒子
	DistBound::BoundingBox* bound_Sphere01 = new DistBound::BoundingBox("BoundingBox");
	bound_Sphere01->aabb = BoundingAABB(mesh_Bunny->data);

	RenderObjectSphere01->AddComponent(transform_Sphere01);
	RenderObjectSphere01->AddComponent(meshRender_Sphere01);
	RenderObjectSphere01->AddComponent(bound_Sphere01);
	RenderObjectSphere01->Enable = true;

	RenderObjectSphere01->name = "Sphere01";
	mRenderObjects.push_back(std::move(RenderObjectSphere01));













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
			matData.UseNormalMap = mat->UseNormalMap;
			matData.NormalScale = mat->NormalScale;
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