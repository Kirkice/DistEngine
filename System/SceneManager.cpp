#include "SceneManager.h"
const int ProjectTexStart = 14;

SceneManager::SceneManager(SceneType tp)
{
	Type = tp;
	BuildScene();
}

SceneManager::~SceneManager()
{

}


//	构建场景
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
	//	SkyBoxMeshRender
	auto sky = std::make_unique<SkyBoxMeshRender>();
	sky->name = "DGarden";
	//构建材质
	sky->material.Name = "DGarden_mat";
	sky->material.MatCBIndex = 0;
	sky->material.Tint = Color(1.0f, 1.0f, 1.0f, 1.0f);
	sky->material.Exposure = 1;
	sky->material.Rotation = 0;
	sky->material.ACES = 1;

	//	创建平面网格
	sky->mesh.CreateSphere(0.5f, 20, 20);

	//	设置坐标
	sky->position = Vector3(0, 0, 0);
	sky->eulerangle = Vector3(0, 0, 0);
	sky->scale = Vector3(5000, 5000, 5000);
	mSkyBoxMeshRender.push_back(std::move(sky));



	//	平面的MeshRender

	auto plane = std::make_unique<PBRMeshRender>();
	plane->name = "plane_render";

	//构建材质
	plane->material.Name = "plane_mat";
	plane->material.MatCBIndex = 1;
	plane->material.DiffuseMapIndex = ProjectTexStart + 0;
	plane->material.NormalMapIndex = ProjectTexStart + 1;
	plane->material.MsoMapIndex = ProjectTexStart + 16;
	plane->material.EmissionMapIndex = ProjectTexStart + 15;
	plane->material.LUTMapIndex = ProjectTexStart + 14;
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
	sphere->material.DiffuseMapIndex = ProjectTexStart + 2;
	sphere->material.NormalMapIndex = ProjectTexStart + 3;
	sphere->material.MsoMapIndex = ProjectTexStart + 16;
	sphere->material.EmissionMapIndex = ProjectTexStart + 15;
	sphere->material.LUTMapIndex = ProjectTexStart + 14;
	sphere->material.DiffuseColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
	sphere->material.Smoothness = 0.15f;
	sphere->material.Metallic = 0.0f;
	sphere->material.Occlusion = 1.0f;
	sphere->material.EmissionColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
	sphere->material.EmissionStrength = 0.0f;

	//	创建球网格
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


//	更新场景材质
void SceneManager::UpdateSceneMaterialBuffer(UploadBuffer<PBRMaterialData>* PBRMaterialBuffer, UploadBuffer<SkyBoxMaterialData>* SkyMaterialBuffer)
{
	switch (Type)
	{
	case SceneType::Default:UpdateDefaultSceneMaterialBuffer(PBRMaterialBuffer, SkyMaterialBuffer);
		break;
	case SceneType::ConelBox:UpdateConelBoxSceneMaterialBuffer(PBRMaterialBuffer, SkyMaterialBuffer);
		break;
	case SceneType::Toon:UpdateToonSceneMaterialBuffer(PBRMaterialBuffer, 
		SkyMaterialBuffer);
		break;
	case SceneType::Water:UpdateWaterSceneMaterialBuffer(PBRMaterialBuffer, SkyMaterialBuffer);
		break;
	default:UpdateDefaultSceneMaterialBuffer(PBRMaterialBuffer, SkyMaterialBuffer);
		break;
	}
}

void SceneManager::UpdateDefaultSceneMaterialBuffer(UploadBuffer<PBRMaterialData>* PBRMaterialBuffer, UploadBuffer<SkyBoxMaterialData>* SkyMaterialBuffer)
{
	for (size_t i = 0; i < mMeshRender.size(); i++)
	{
		PBRMaterial* mat = &(mMeshRender[i]->material);

		XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);

		PBRMaterialData matData;
		matData.DiffuseColor = mat->DiffuseColor;
		matData.Smoothness = mat->Smoothness;
		matData.Metallic = mat->Metallic;
		matData.Occlusion = mat->Occlusion;
		matData.EmissionColor = mat->EmissionColor;
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


	for (size_t i = 0; i < mSkyBoxMeshRender.size(); i++)
	{
		SkyBoxMaterial* skyMat = &mSkyBoxMeshRender[i]->material;
		SkyBoxMaterialData matData;
		matData.Tint = skyMat->Tint;
		matData.Exposure = skyMat->Exposure;
		matData.Rotation = skyMat->Rotation;
		matData.ACES = skyMat->ACES;
		SkyMaterialBuffer->CopyData(skyMat->MatCBIndex, matData);

		// Next FrameResource need to be updated too.
		skyMat->NumFramesDirty--;
	}
}

void SceneManager::UpdateConelBoxSceneMaterialBuffer(UploadBuffer<PBRMaterialData>* PBRMaterialBuffer, UploadBuffer<SkyBoxMaterialData>* SkyMaterialBuffer)
{

}

void SceneManager::UpdateToonSceneMaterialBuffer(UploadBuffer<PBRMaterialData>* PBRMaterialBuffer, UploadBuffer<SkyBoxMaterialData>* SkyMaterialBuffer)
{

}

void SceneManager::UpdateWaterSceneMaterialBuffer(UploadBuffer<PBRMaterialData>* PBRMaterialBuffer, UploadBuffer<SkyBoxMaterialData>* SkyMaterialBuffer)
{

}




//	创建RenderItem
void SceneManager::BuildRenderItem(std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count], std::vector<std::unique_ptr<RenderItem>>& mAllRitems, Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList)
{
	for (size_t i = 0; i < mMeshRender.size(); i++)
	{
		auto Ritem = std::make_unique<RenderItem>();
		Ritem->World = mMeshRender[i]->GetWorldXMMatrix();
		Ritem->TexTransform = Mathf::Identity4x4();
		Ritem->ObjCBIndex = i;
		Ritem->Mat = &mMeshRender[i]->material;
		Ritem->Geo = GraphicsUtils::BuidlMeshGeometryFromMeshData(mMeshRender[i]->name, mMeshRender[i]->mesh.data, md3dDevice, mCommandList);
		Ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		Ritem->IndexCount = Ritem->Geo->DrawArgs["mesh"].IndexCount;
		Ritem->StartIndexLocation = Ritem->Geo->DrawArgs["mesh"].StartIndexLocation;
		Ritem->BaseVertexLocation = Ritem->Geo->DrawArgs["mesh"].BaseVertexLocation;

		mRitemLayer[(int)RenderLayer::Opaque].push_back(Ritem.get());
		mAllRitems.push_back(std::move(Ritem));
	}

	for (size_t i = 0; i < mSkyBoxMeshRender.size(); i++)
	{
		auto Ritem = std::make_unique<RenderItem>();
		Ritem->World = mSkyBoxMeshRender[i]->GetWorldXMMatrix();
		Ritem->TexTransform = Mathf::Identity4x4();
		Ritem->ObjCBIndex = i;
		Ritem->Mat = &mSkyBoxMeshRender[i]->material;
		Ritem->Geo = GraphicsUtils::BuidlMeshGeometryFromMeshData(mSkyBoxMeshRender[i]->name, mSkyBoxMeshRender[i]->mesh.data, md3dDevice, mCommandList);
		Ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		Ritem->IndexCount = Ritem->Geo->DrawArgs["mesh"].IndexCount;
		Ritem->StartIndexLocation = Ritem->Geo->DrawArgs["mesh"].StartIndexLocation;
		Ritem->BaseVertexLocation = Ritem->Geo->DrawArgs["mesh"].BaseVertexLocation;

		mRitemLayer[(int)RenderLayer::Sky].push_back(Ritem.get());
		mAllRitems.push_back(std::move(Ritem));
	}
}