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


//	��������
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
	//��������
	sky->material.Name = "DGarden_mat";
	sky->material.MatCBIndex = 0;
	sky->material.Tint = Color(1.0f, 1.0f, 1.0f, 1.0f);
	sky->material.Exposure = 1;
	sky->material.Rotation = 0;
	sky->material.ACES = 1;

	//	����ƽ������
	sky->mesh.CreateSphere(0.5f, 20, 20);

	//	��������
	sky->position = Vector3(0, 0, 0);
	sky->eulerangle = Vector3(0, 0, 0);
	sky->scale = Vector3(5000, 5000, 5000);
	mSkyBoxMeshRender.push_back(std::move(sky));



	//	ƽ���MeshRender

	auto plane = std::make_unique<PBRMeshRender>();
	plane->name = "plane_render";

	//��������
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

	//	����ƽ������
	plane->mesh.CreateGrid(20.0f, 30.0f, 60, 40);

	//	��������
	plane->position = Vector3(0, 0, 0);
	plane->eulerangle = Vector3(0, 0, 0);
	plane->scale = Vector3(10, 1, 10);

	//	������ײ��
	plane->bound.aabb = BoundingAABB(plane->mesh.data);

	//	����AABB����������
	plane->bound.aabb.m_min = plane->GetWorldMatrix() * plane->bound.aabb.m_min;
	plane->bound.aabb.m_max = plane->GetWorldMatrix() * plane->bound.aabb.m_max;
	mMeshRender.push_back(std::move(plane));

	//	���MeshRender

	auto sphere = std::make_unique<PBRMeshRender>();
	sphere->name = "sphere_render";

	//��������
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

	//	����������
	sphere->mesh.CreateSphere(0.5f, 20, 20);

	//	��������
	sphere->position = Vector3(0, 0, 0);
	sphere->eulerangle = Vector3(0, 0, 0);
	sphere->scale = Vector3(5, 5, 5);

	//	������ײ��
	sphere->bound.aabb = BoundingAABB(sphere->mesh.data);

	//	����AABB����������
	sphere->bound.aabb.m_min = sphere->GetWorldMatrix() * sphere->bound.aabb.m_min;
	sphere->bound.aabb.m_max = sphere->GetWorldMatrix() * sphere->bound.aabb.m_max;
	mMeshRender.push_back(std::move(sphere));






	//	��Ϊ����Դ
	mMainLight.isMainLight = true;
	//	�ƹ���ɫ
	mMainLight.color = Color(1, 0.9568627f, 0.8392157f, 1);
	//	���õƹ�λ��
	mMainLight.position = Vector3(0, 3, 0);
	//	���õƹ�ŷ����
	mMainLight.eulerangle = Vector3(50, -30, 0);
	//	����Դ����
	mMainLight.name = "Direction Light";

	//	���������
	mSkyBoxSetting.Tint = Color(1.0f, 1.0f, 1.0f, 1.0f);
	mSkyBoxSetting.Exposure = 1.0f;
	mSkyBoxSetting.Rotation = 0.0f;
	mSkyBoxSetting.ACES = 1.0f;

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


//	���³�������
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




//	����RenderItem
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