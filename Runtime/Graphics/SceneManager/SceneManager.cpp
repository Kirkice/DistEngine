#include "SceneManager.h"
#include "../File/ResourcesPath.h"

const int ProjectTexStart = 14;

SceneManager::SceneManager(SceneType tp)
{
	Type = tp;
}


//	��������
void SceneManager::BuildScene(
	std::unordered_map<std::string, std::unique_ptr<Texture2D>>& mResourcesTextures,
	MaterialIndexUtils& matCBIndexUtils
)
{
	BuildDefaultScene(mResourcesTextures, matCBIndexUtils);
}

void SceneManager::BuildDefaultScene(
	std::unordered_map<std::string, std::unique_ptr<Texture2D>>& mResourcesTextures,
	MaterialIndexUtils& matCBIndexUtils
)
{
	//	��������ȫ������
	BuildGlobalConfig();

	//	��������Դ
	BuildMainLight();

	//	������պ�?
	BuildSkyBox(matCBIndexUtils);

	//	��Ⱦ��
	BuildRenderObejct(mResourcesTextures, matCBIndexUtils);

	//	���ζ�����
	BuildConelBoxScene(mResourcesTextures, matCBIndexUtils);
}

/// <summary>
/// ȫ������
/// </summary>
void SceneManager::BuildGlobalConfig()
{
	//	���������?
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

	//	�ƹ����� 
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

	//	�������?
	mCameraSetting.mCamFov = 45;
	mCameraSetting.mCamClipN = 0.3;
	mCameraSetting.mCamClipF = 1000;
	mCameraSetting.renderSkyBox = true;
}

void SceneManager::BuildMainLight()
{
	//	��ȡJSON
	nl::json jsonValue;
	ifstream jfile(mScenePath[0]);
	jfile >> jsonValue;

	SceneData data;
	nl::from_json(jsonValue, data);

	Transform* mTransform = new Transform("Transform");
	//	���õƹ�λ��
	mTransform->position = Vector3(data.MainLightPosition[0], data.MainLightPosition[1], data.MainLightPosition[2]);
	//	���õƹ�ŷ����
	mTransform->eulerangle = Vector3(data.MainLightEulerangle[0], data.MainLightEulerangle[1], data.MainLightEulerangle[2]);
	//	����forward
	mTransform->forward = Vector3(data.MainLightForward[0], data.MainLightForward[1], data.MainLightForward[2]);
	MainLight->AddComponent(mTransform);

	DirectionLight* mDirectionLight = new DirectionLight("DirectionLight");
	//	��Ϊ����Դ
	mDirectionLight->isMainLight = data.isMainLight;
	//	�ƹ���ɫ
	mDirectionLight->color = Color(data.MainLightColor[0], data.MainLightColor[1], data.MainLightColor[2], data.MainLightColor[3]);
	//	�ƹ�ǿ��
	mDirectionLight->intensity = 1.0;

	MainLight->AddComponent(mDirectionLight);

	//	����Դ����
	MainLight->name = data.MainLightName;
}

/// <summary>
/// �����?
/// </summary>
/// <param name="matCBIndexUtils"></param>
void SceneManager::BuildSkyBox(MaterialIndexUtils& matCBIndexUtils)
{
	//	�洢����matCB��ʼֵ
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

	//	����ģ��
	MeshFliter* mesh_sky = new MeshFliter("MeshFliter");
	mesh_sky->CreateSphere(0.5f, 20.0f, 20.0f);
	MeshRender* meshRender_sky = new MeshRender(mesh_sky, mat_sky, "MeshRender");

	//	��������
	Transform* transform_sky = new Transform("Transform");
	transform_sky->position = Vector3(0, 0, 0);
	transform_sky->eulerangle = Vector3(0, 0, 0);
	transform_sky->scale = Vector3(5000, 5000, 5000);

	//	������ײ����
	DistBound::BoundingBox* bound_sky = new DistBound::BoundingBox("BoundingBox");
	bound_sky->aabb = DistBound::BoundingAABB(mesh_sky->data);

	skyRenderObject->AddComponent(transform_sky);
	skyRenderObject->AddComponent(meshRender_sky);
	skyRenderObject->AddComponent(bound_sky);

	skyRenderObject->Enable = true;
	skyRenderObject->name = "sky";
	mRenderObjects.push_back(std::move(skyRenderObject));
}

/// <summary>
/// ��ȾItem
/// </summary>
/// <param name="mResourcesTextures"></param>
/// <param name="matCBIndexUtils"></param>
void SceneManager::BuildRenderObejct(
	std::unordered_map<std::string, std::unique_ptr<Texture2D>>& mResourcesTextures,
	MaterialIndexUtils& matCBIndexUtils
)
{
	//	��ȡ����JSON
	nl::json sceneData;
	ifstream sceneFile(mScenePath[0]);
	sceneFile >> sceneData;
	SceneData scene_data;
	nl::from_json(sceneData, scene_data);

	//	��ȡ����JSON
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

	//	����ģ��
	MeshFliter* mesh_ak47 = new MeshFliter("MeshFliter");
	ObjLoader::LoadObj(mesh_ak47->data, (char*)scene_data.MeshPath.c_str());
	MeshRender* meshRender_ak47 = new MeshRender(mesh_ak47, mat_ak47, "MeshRender");

	//	��������
	Transform* transform_ak47 = new Transform("Transform");
	transform_ak47->position = Vector3(scene_data.RenderObjectPosition[0], scene_data.RenderObjectPosition[1], scene_data.RenderObjectPosition[2]);
	transform_ak47->eulerangle = Vector3(scene_data.RenderObjectEulerAngle[0], scene_data.RenderObjectEulerAngle[1], scene_data.RenderObjectEulerAngle[2]);
	transform_ak47->scale = Vector3(scene_data.RenderObjectScale[0], scene_data.RenderObjectScale[1], scene_data.RenderObjectScale[2]);

	//	������ײ����
	DistBound::BoundingBox* bound_ak47 = new DistBound::BoundingBox("BoundingBox");
	bound_ak47->aabb = DistBound::BoundingAABB(mesh_ak47->data);

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
	mat_Bunny->DiffuseColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
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

	//	����ģ��
	MeshFliter* mesh_Bunny = new MeshFliter("MeshFliter");
	ObjLoader::LoadObj(mesh_Bunny->data, (char*)mBunnyObjPath.c_str());
	MeshRender* meshRender_Bunny = new MeshRender(mesh_Bunny, mat_Bunny, "MeshRender");

	//	��������
	Transform* transform_Bunny = new Transform("Transform");
	transform_Bunny->position = Vector3(-5, -1.5, 8);
	transform_Bunny->eulerangle = Vector3(0,-45,0); 
	transform_Bunny->scale = Vector3(12,12,12);

	//	������ײ����
	DistBound::BoundingBox* bound_Bunny = new DistBound::BoundingBox("BoundingBox");
	bound_Bunny->aabb = DistBound::BoundingAABB(mesh_Bunny->data);

	RenderObjectBunny->AddComponent(transform_Bunny);
	RenderObjectBunny->AddComponent(meshRender_Bunny);
	RenderObjectBunny->AddComponent(bound_Bunny);
	RenderObjectBunny->Enable = true;

	RenderObjectBunny->name = "Bunny";
	mRenderObjects.push_back(std::move(RenderObjectBunny));




	auto RenderObjectSphere01 = std::make_unique<GameObject>("Sphere01");
	// Build Material
	Material* mat_Sphere01 = new Material();
	mat_Sphere01->Name = "Sphere01";
	mat_Sphere01->MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	mat_Sphere01->DiffuseColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
	mat_Sphere01->Smoothness = 0.1f;
	mat_Sphere01->Metallic = 0.1f;
	mat_Sphere01->Occlusion = 1.0;
	mat_Sphere01->EmissionColor = Color(1.0f, 0.0f, 0.0f, 0.0f);
	mat_Sphere01->EmissionStrength = 0.0f;
	mat_Sphere01->UseNormalMap = 0;
	mat_Sphere01->NormalScale = 1;
	mat_Sphere01->DiffuseMapIndex = mResourcesTextures["white"]->TexIndex;
	mat_Sphere01->NormalMapIndex = mResourcesTextures["white"]->TexIndex;
	mat_Sphere01->MsoMapIndex = mResourcesTextures["white"]->TexIndex;
	mat_Sphere01->EmissionMapIndex = mResourcesTextures["white"]->TexIndex;
	mat_Sphere01->LUTMapIndex = mResourcesTextures["sampleLUT"]->TexIndex;

	//	����ģ��
	MeshFliter* mesh_Sphere01 = new MeshFliter("MeshFliter");
	mesh_Sphere01->CreateSphere(1.0f, 20.0f, 20.0f);

	MeshRender* meshRender_Sphere01 = new MeshRender(mesh_Sphere01, mat_Sphere01, "MeshRender");

	//	��������
	Transform* transform_Sphere01 = new Transform("Transform");
	transform_Sphere01->position = Vector3(-3.0f, 3.0f, -8.0f);
	transform_Sphere01->eulerangle = Vector3(0, 0, 0);
	transform_Sphere01->scale = Vector3(3, 3, 3);

	//	������ײ����
	DistBound::BoundingBox* bound_Sphere01 = new DistBound::BoundingBox("BoundingBox");
	bound_Sphere01->aabb = DistBound::BoundingAABB(mesh_Bunny->data);

	RenderObjectSphere01->AddComponent(transform_Sphere01);
	RenderObjectSphere01->AddComponent(meshRender_Sphere01);
	RenderObjectSphere01->AddComponent(bound_Sphere01);
	RenderObjectSphere01->Enable = true;

	RenderObjectSphere01->name = "Sphere01";
	mRenderObjects.push_back(std::move(RenderObjectSphere01));

	auto RenderObjectSphere02 = std::make_unique<GameObject>("Sphere02");
	// Build Material
	Material* mat_Sphere02 = new Material();
	mat_Sphere02->Name = "Sphere02";
	mat_Sphere02->MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	mat_Sphere02->DiffuseColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
	mat_Sphere02->Smoothness = 0.1f;
	mat_Sphere02->Metallic = 0.1f;
	mat_Sphere02->Occlusion = 1.0;
	mat_Sphere02->EmissionColor = Color(1.0f, 0.0f, 0.0f, 0.0f);
	mat_Sphere02->EmissionStrength = 0.0f;
	mat_Sphere02->UseNormalMap = 0;
	mat_Sphere02->NormalScale = 1;
	mat_Sphere02->DiffuseMapIndex = mResourcesTextures["white"]->TexIndex;
	mat_Sphere02->NormalMapIndex = mResourcesTextures["white"]->TexIndex;
	mat_Sphere02->MsoMapIndex = mResourcesTextures["white"]->TexIndex;
	mat_Sphere02->EmissionMapIndex = mResourcesTextures["white"]->TexIndex;
	mat_Sphere02->LUTMapIndex = mResourcesTextures["sampleLUT"]->TexIndex;

	//	����ģ��
	MeshFliter* mesh_Sphere02 = new MeshFliter("MeshFliter");
	mesh_Sphere02->CreateSphere(1.0f, 20.0f, 20.0f);

	MeshRender* meshRender_Sphere02 = new MeshRender(mesh_Sphere02, mat_Sphere02, "MeshRender");

	//	��������
	Transform* transform_Sphere02 = new Transform("Transform");
	transform_Sphere02->position = Vector3(-8.0f, 2.0f, -3.0f);
	transform_Sphere02->eulerangle = Vector3(0, 0, 0);
	transform_Sphere02->scale = Vector3(2, 2, 2);

	//	������ײ����
	DistBound::BoundingBox* bound_Sphere02 = new DistBound::BoundingBox("BoundingBox");
	bound_Sphere02->aabb = DistBound::BoundingAABB(mesh_Bunny->data);

	RenderObjectSphere02->AddComponent(transform_Sphere02); 
	RenderObjectSphere02->AddComponent(meshRender_Sphere02);
	RenderObjectSphere02->AddComponent(bound_Sphere02);
	RenderObjectSphere02->Enable = true;

	RenderObjectSphere02->name = "Sphere02";
	mRenderObjects.push_back(std::move(RenderObjectSphere02));

}

void SceneManager::BuildConelBoxScene(
	std::unordered_map<std::string, std::unique_ptr<Texture2D>>& mResourcesTextures,
	MaterialIndexUtils& matCBIndexUtils
)
{
	// Build Material
	Material* mat_White = new Material();
	mat_White->Name = "White";
	mat_White->MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	mat_White->DiffuseColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
	mat_White->Smoothness = 0.15f;
	mat_White->Metallic = 0.2f;
	mat_White->Occlusion = 1.0;
	mat_White->EmissionColor = Color(1.0f, 0.0f, 0.0f, 0.0f);
	mat_White->EmissionStrength = 0.0f;
	mat_White->UseNormalMap = 0;
	mat_White->NormalScale = 1;
	mat_White->DiffuseMapIndex = mResourcesTextures["white"]->TexIndex;
	mat_White->NormalMapIndex = mResourcesTextures["white"]->TexIndex;
	mat_White->MsoMapIndex = mResourcesTextures["white"]->TexIndex;
	mat_White->EmissionMapIndex = mResourcesTextures["white"]->TexIndex;
	mat_White->LUTMapIndex = mResourcesTextures["sampleLUT"]->TexIndex;


	Material* mat_Red = new Material();
	mat_Red->Name = "Red";
	mat_Red->MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	mat_Red->DiffuseColor = Color(1.0f, 0.0f, 0.0f, 1.0f);
	mat_Red->Smoothness = 0.15f;
	mat_Red->Metallic = 0.2f;
	mat_Red->Occlusion = 1.0;
	mat_Red->EmissionColor = Color(1.0f, 0.0f, 0.0f, 0.0f);
	mat_Red->EmissionStrength = 0.0f;
	mat_Red->UseNormalMap = 0;
	mat_Red->NormalScale = 1;
	mat_Red->DiffuseMapIndex = mResourcesTextures["white"]->TexIndex;
	mat_Red->NormalMapIndex = mResourcesTextures["white"]->TexIndex;
	mat_Red->MsoMapIndex = mResourcesTextures["white"]->TexIndex;
	mat_Red->EmissionMapIndex = mResourcesTextures["white"]->TexIndex;
	mat_Red->LUTMapIndex = mResourcesTextures["sampleLUT"]->TexIndex;

	Material* mat_Green = new Material();
	mat_Green->Name = "Green";
	mat_Green->MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	mat_Green->DiffuseColor = Color(0.0f, 1.0f, 0.0f, 1.0f);
	mat_Green->Smoothness = 0.15f;
	mat_Green->Metallic = 0.2f;
	mat_Green->Occlusion = 1.0;
	mat_Green->EmissionColor = Color(1.0f, 0.0f, 0.0f, 0.0f);
	mat_Green->EmissionStrength = 0.0f;
	mat_Green->UseNormalMap = 0;
	mat_Green->NormalScale = 1;
	mat_Green->DiffuseMapIndex = mResourcesTextures["white"]->TexIndex;
	mat_Green->NormalMapIndex = mResourcesTextures["white"]->TexIndex;
	mat_Green->MsoMapIndex = mResourcesTextures["white"]->TexIndex;
	mat_Green->EmissionMapIndex = mResourcesTextures["white"]->TexIndex;
	mat_Green->LUTMapIndex = mResourcesTextures["sampleLUT"]->TexIndex;

	//	����ģ��
	MeshFliter* mesh_Plane = new MeshFliter("MeshFliter");
	mesh_Plane->CreateGrid(6, 6, 24, 24);

	MeshRender* meshRender_PlaneWhite = new MeshRender(mesh_Plane, mat_White, "MeshRender");
	MeshRender* meshRender_PlaneRed = new MeshRender(mesh_Plane, mat_Red, "MeshRender");
	MeshRender* meshRender_PlaneGreen = new MeshRender(mesh_Plane, mat_Green, "MeshRender");

	//	��������
	Transform* transform_Plane01 = new Transform("Transform");
	transform_Plane01->position = Vector3(0.0f, 0.0f, 0.0f);
	transform_Plane01->eulerangle = Vector3(0, 0, 0);
	transform_Plane01->scale = Vector3(4, 1, 4);


	//	������ײ����
	DistBound::BoundingBox* bound_Plane = new DistBound::BoundingBox("BoundingBox");
	bound_Plane->aabb = DistBound::BoundingAABB(mesh_Plane->data);

	auto RenderObjectPlane01 = std::make_unique<GameObject>("White01");
	RenderObjectPlane01->AddComponent(transform_Plane01);
	RenderObjectPlane01->AddComponent(meshRender_PlaneWhite);
	RenderObjectPlane01->AddComponent(bound_Plane);
	RenderObjectPlane01->Enable = true;

	RenderObjectPlane01->name = "Plane01";
	mRenderObjects.push_back(std::move(RenderObjectPlane01));




	//	��������
	Transform* transform_Plane02 = new Transform("Transform");
	transform_Plane02->position = Vector3(12.0f, 12.0f, 0.0f);
	transform_Plane02->eulerangle = Vector3(90, -90, 0);
	transform_Plane02->scale = Vector3(4, 1, 4);

	auto RenderObjectPlane02 = std::make_unique<GameObject>("White02");
	RenderObjectPlane02->AddComponent(transform_Plane02);
	RenderObjectPlane02->AddComponent(meshRender_PlaneWhite);
	RenderObjectPlane02->AddComponent(bound_Plane);
	RenderObjectPlane02->Enable = true;

	RenderObjectPlane02->name = "Plane02";
	mRenderObjects.push_back(std::move(RenderObjectPlane02));


	//	��������
	Transform* transform_Plane03 = new Transform("Transform");
	transform_Plane03->position = Vector3(0.0f, 24.0f, 0.0f);
	transform_Plane03->eulerangle = Vector3(0, 0, 180);
	transform_Plane03->scale = Vector3(4, 1, 4);

	auto RenderObjectPlane03 = std::make_unique<GameObject>("White03");
	RenderObjectPlane03->AddComponent(transform_Plane03);
	RenderObjectPlane03->AddComponent(meshRender_PlaneWhite);
	RenderObjectPlane03->AddComponent(bound_Plane);
	RenderObjectPlane03->Enable = true;

	RenderObjectPlane03->name = "Plane03";
	mRenderObjects.push_back(std::move(RenderObjectPlane03));


	//	��������
	Transform* transform_Plane04 = new Transform("Transform");
	transform_Plane04->position = Vector3(0.0f, 12.0f, -12.0f);
	transform_Plane04->eulerangle = Vector3(90, 0, 0);
	transform_Plane04->scale = Vector3(4, 1, 4);

	auto RenderObjectPlane04 = std::make_unique<GameObject>("Red");
	RenderObjectPlane04->AddComponent(transform_Plane04);
	RenderObjectPlane04->AddComponent(meshRender_PlaneRed);
	RenderObjectPlane04->AddComponent(bound_Plane);
	RenderObjectPlane04->Enable = true;

	RenderObjectPlane04->name = "Plane04";
	mRenderObjects.push_back(std::move(RenderObjectPlane04));


	//	��������
	Transform* transform_Plane05 = new Transform("Transform");
	transform_Plane05->position = Vector3(0.0f, 12.0f, 12.0f);
	transform_Plane05->eulerangle = Vector3(-90, 0, 0);
	transform_Plane05->scale = Vector3(4, 1, 4);

	auto RenderObjectPlane05 = std::make_unique<GameObject>("Green");
	RenderObjectPlane05->AddComponent(transform_Plane05);
	RenderObjectPlane05->AddComponent(meshRender_PlaneGreen);
	RenderObjectPlane05->AddComponent(bound_Plane);
	RenderObjectPlane05->Enable = true;

	RenderObjectPlane05->name = "Plane05";
	mRenderObjects.push_back(std::move(RenderObjectPlane05));
}


//	���³�������
void SceneManager::UpdateSceneMaterialBuffer(
	UploadBuffer<PBRMaterialData>* PBRMaterialBuffer, 
	UploadBuffer<SkyBoxMaterialData>* SkyMaterialBuffer,
	MaterialIndexUtils& matCBIndexUtils
)
{
	UpdateDefaultSceneMaterialBuffer(PBRMaterialBuffer, SkyMaterialBuffer, matCBIndexUtils);
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


//	����RenderItem
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
		//	������Ⱦ��
		auto Ritem = std::make_unique<RenderItem>();
		Ritem->World = mRenderObjects[i]->GetComponent<Transform>(1)->GetWorldXMMatrix();
		Ritem->TexTransform = Mathf::Identity4x4();
		Ritem->ObjCBIndex = CurrentSize + i;
		Ritem->Mat = mRenderObjects[i]->GetComponent<MeshRender>(2)->mat;
		Ritem->Geo = GraphicsUtils::BuildMeshGeometryFromMeshData(mRenderObjects[i]->name, mRenderObjects[i]->GetComponent<MeshRender>(2)->mesh->data, md3dDevice.Get(), mCommandList.Get()).release();
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
/// ����CBuffer 
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
