#include "SceneManager.h"
const int GizmoCount = 6;

SceneManager::SceneManager(SceneType tp)
{
	Type = tp;
	BuildScene();
}

SceneManager::~SceneManager()
{

}

void SceneManager::BuildScene()
{
	switch (Type)
	{
	case SceneType::Default:BuildDefaultScene();
		break;
	case SceneType::ConelBox:BuildConelBoxScene();
		break;
	case SceneType::Toon:BuildToonScene();
		break;
	case SceneType::Water:BuildWaterScene();
		break;
	default:BuildDefaultScene();
		break;
	}
}

void SceneManager::BuildDefaultScene()
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


	//	平面的MeshRender

	auto plane = std::make_unique<PBRMeshRender>();
	plane->name = "plane_render";

	//构建材质
	plane->material.Name = "plane_mat";
	plane->material.MatCBIndex = 1;
	plane->material.DiffuseMapIndex = GizmoCount + 0;
	plane->material.NormalMapIndex = GizmoCount + 1;
	plane->material.MsoMapIndex = GizmoCount + 2;
	plane->material.EmissionMapIndex = GizmoCount + 3;
	plane->material.LUTMapIndex = GizmoCount + 4;
	plane->material.DiffuseColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
	plane->material.Smoothness = 0.0f;
	plane->material.Metallic = 0.0f;
	plane->material.Occlusion = 1;
	plane->material.EmissionColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
	plane->material.EmissionStrength = 0.0f;

	//	创建平面网格
	plane->mesh.CreateGrid(20.0f, 30.0f, 60, 40);

	//	设置坐标
	plane->position = Vector3(0, 0, 0);
	plane->eulerangle = Vector3(0, 0, 0);
	plane->scale = Vector3(10, 1, 10);

	//	创建碰撞盒
	plane->bound.aabb = BoundingAABB(plane->mesh.data);

	//	设置AABB的世界坐标
	plane->bound.aabb.m_min = plane->GetWorldMatrix() * plane->bound.aabb.m_min;
	plane->bound.aabb.m_max = plane->GetWorldMatrix() * plane->bound.aabb.m_max;
	mMeshRender.push_back(std::move(plane));

	//	球的MeshRender

	auto sphere = std::make_unique<PBRMeshRender>();
	sphere->name = "sphere_render";

	//构建材质
	sphere->material.Name = "Wooden2";
	sphere->material.MatCBIndex = 2;
	sphere->material.DiffuseMapIndex = GizmoCount + 13;
	sphere->material.NormalMapIndex = GizmoCount + 14;
	sphere->material.MsoMapIndex = GizmoCount + 2;
	sphere->material.EmissionMapIndex = GizmoCount + 3;
	sphere->material.LUTMapIndex = GizmoCount + 4;
	sphere->material.DiffuseColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
	sphere->material.Smoothness = 0.15f;
	sphere->material.Metallic = 0.0f;
	sphere->material.Occlusion = 1.0f;
	sphere->material.EmissionColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
	sphere->material.EmissionStrength = 0.0f;

	//	创建平面网格
	sphere->mesh.CreateSphere(0.5f, 20, 20);

	//	设置坐标
	sphere->position = Vector3(0, 0, 0);
	sphere->eulerangle = Vector3(0, 0, 0);
	sphere->scale = Vector3(5, 5, 5);

	//	创建碰撞盒
	sphere->bound.aabb = BoundingAABB(sphere->mesh.data);

	//	设置AABB的世界坐标
	sphere->bound.aabb.m_min = sphere->GetWorldMatrix() * sphere->bound.aabb.m_min;
	sphere->bound.aabb.m_max = sphere->GetWorldMatrix() * sphere->bound.aabb.m_max;
	mMeshRender.push_back(std::move(sphere));

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

}

void SceneManager::BuildConelBoxScene()
{

}

void SceneManager::BuildToonScene()
{

}

void SceneManager::BuildWaterScene()
{

}