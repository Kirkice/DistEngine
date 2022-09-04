#include "GizmosManager.h"
#include "../File/ResourcesPath.h"

void GizmosManager::BuildWirePlane(MaterialIndexUtils& matCBIndexUtils)
{
	//	存储类型matCB开始值
	matCBIndexUtils.getInstance().SaveTypeIndex("Gizmo", matCBIndexUtils.getInstance().GetIndex());

	auto planeRenderObject = std::make_unique<GameObject>("plane");

	//构建材质
	Material* mat_plane = new Material();
	mat_plane->Name = "Plane_mat";
	mat_plane->MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	mat_plane->DiffuseColor = Color(0.8f, 0.8f, 0.8f, 1.0f);

	//	加载模型
	MeshFliter* mesh_plane = new MeshFliter("MeshFliter");
	mesh_plane->CreateGrid(60, 60, 240, 240);
	MeshRender* meshRender_plane = new MeshRender(mesh_plane, mat_plane, "MeshRender");

	//	设置坐标
	Transform* transform_plane = new Transform("Transform");
	transform_plane->position = Vector3(0, 0, 0);
	transform_plane->eulerangle = Vector3(0, 0, 0);
	transform_plane->scale = Vector3(10, 10, 10);

	//	创建碰撞盒子
	DistBound::BoundingBox* bound_plane = new DistBound::BoundingBox("BoundingBox");
	bound_plane->aabb = BoundingAABB(mesh_plane->data);

	planeRenderObject->AddComponent(transform_plane);
	planeRenderObject->AddComponent(meshRender_plane);
	planeRenderObject->AddComponent(bound_plane);
	planeRenderObject->Enable = true;

	mRenderObjects.push_back(std::move(planeRenderObject));
}

void GizmosManager::BuildLightGizmo(
	std::unordered_map<std::string, std::unique_ptr<Texture2D>>& mGizmosTextures,
	MaterialIndexUtils& matCBIndexUtils)
{
	auto directionLightGizmoObject = std::make_unique<GameObject>("DirectionLightGizmo");

	//构建材质
	Material* mat_dirGizmo = new Material();
	mat_dirGizmo->Name = "DirGizmo";
	mat_dirGizmo->MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	mat_dirGizmo->DiffuseColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
	mat_dirGizmo->DiffuseMapIndex = mGizmosTextures["DirectionalLight"]->TexIndex;

	//	加载模型
	MeshFliter* mesh_dirGizmo = new MeshFliter("MeshFliter");
	mesh_dirGizmo->CreateGrid(6, 6, 4, 4);
	MeshRender* meshRender_plane = new MeshRender(mesh_dirGizmo, mat_dirGizmo, "MeshRender");

	//	设置坐标
	Transform* transform_dirGizmo = new Transform("Transform");
	transform_dirGizmo->position = Vector3(-20, 30, 20);
	transform_dirGizmo->eulerangle = Vector3(0, 0, 0);
	transform_dirGizmo->scale = Vector3(0.5f, 0.5f, 0.5f);

	//	创建碰撞盒子
	DistBound::BoundingBox* bound_dirGizmo = new DistBound::BoundingBox("BoundingBox");
	bound_dirGizmo->aabb = BoundingAABB(mesh_dirGizmo->data);

	directionLightGizmoObject->AddComponent(transform_dirGizmo);
	directionLightGizmoObject->AddComponent(meshRender_plane);
	directionLightGizmoObject->AddComponent(bound_dirGizmo);
	directionLightGizmoObject->Enable = true;

	mRenderObjects.push_back(std::move(directionLightGizmoObject));
}

void GizmosManager::BuildUCSPosition(MaterialIndexUtils& matCBIndexUtils)
{
	//---------------------------------------------------------------
	//						坐标
	//---------------------------------------------------------------
	matCBIndexUtils.getInstance().SaveTypeIndex("PositionUCS", matCBIndexUtils.getInstance().GetIndex());

	auto PositionUCSGizmoX = std::make_unique<GameObject>("PositionUCSGizmoX");

	//构建材质
	Material* mat_ucsGizmoX = new Material();
	mat_ucsGizmoX->Name = "PositionUCS";
	mat_ucsGizmoX->MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	mat_ucsGizmoX->DiffuseColor = Color(1.0f, 0.0f, 0.0f, 1.0f);

	//	加载模型
	MeshFliter* mesh_ucsGizmoX = new MeshFliter("MeshFliter");
	ObjLoader::LoadObj(mesh_ucsGizmoX->data, (char*)mPositionUCSGizmoXPath.c_str());
	MeshRender* meshRender_ucsGizmoX = new MeshRender(mesh_ucsGizmoX, mat_ucsGizmoX, "MeshRender");

	//	设置坐标
	Transform* transform_ucsGizmoX = new Transform("Transform");
	transform_ucsGizmoX->position = Vector3(0, 0, 0);
	transform_ucsGizmoX->eulerangle = Vector3(0, 0, 0);
	transform_ucsGizmoX->scale = Vector3(1, 1, 1);

	//	创建碰撞盒子
	DistBound::BoundingBox* bound_ucsGizmoX = new DistBound::BoundingBox("BoundingBox");
	bound_ucsGizmoX->aabb = BoundingAABB(mesh_ucsGizmoX->data);
	bound_ucsGizmoX->aabb.m_min = transform_ucsGizmoX->GetWorldMatrix() * bound_ucsGizmoX->aabb.m_min;
	bound_ucsGizmoX->aabb.m_max = transform_ucsGizmoX->GetWorldMatrix() * bound_ucsGizmoX->aabb.m_min;

	PositionUCSGizmoX->AddComponent(transform_ucsGizmoX);
	PositionUCSGizmoX->AddComponent(meshRender_ucsGizmoX);
	PositionUCSGizmoX->AddComponent(bound_ucsGizmoX);
	PositionUCSGizmoX->Enable = true;

	mRenderObjects.push_back(std::move(PositionUCSGizmoX));




	auto PositionUCSGizmoY = std::make_unique<GameObject>("PositionUCSGizmoY");

	//构建材质
	Material* mat_ucsGizmoY = new Material();
	mat_ucsGizmoY->Name = "PositionUCS";
	mat_ucsGizmoY->MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	mat_ucsGizmoY->DiffuseColor = Color(0.0f, 1.0f, 0.0f, 1.0f);

	//	加载模型
	MeshFliter* mesh_ucsGizmoY = new MeshFliter("MeshFliter");
	ObjLoader::LoadObj(mesh_ucsGizmoY->data, (char*)mPositionUCSGizmoYPath.c_str());
	MeshRender* meshRender_ucsGizmoY = new MeshRender(mesh_ucsGizmoY, mat_ucsGizmoY, "MeshRender");

	//	设置坐标
	Transform* transform_ucsGizmoY = new Transform("Transform");
	transform_ucsGizmoY->position = Vector3(0, 0, 0);
	transform_ucsGizmoY->eulerangle = Vector3(0, 0, 0);
	transform_ucsGizmoY->scale = Vector3(1, 1, 1);

	//	创建碰撞盒子
	DistBound::BoundingBox* bound_ucsGizmoY = new DistBound::BoundingBox("BoundingBox");
	bound_ucsGizmoY->aabb = BoundingAABB(mesh_ucsGizmoY->data);
	bound_ucsGizmoY->aabb.m_min = transform_ucsGizmoY->GetWorldMatrix() * bound_ucsGizmoY->aabb.m_min;
	bound_ucsGizmoY->aabb.m_max = transform_ucsGizmoY->GetWorldMatrix() * bound_ucsGizmoY->aabb.m_min;

	PositionUCSGizmoY->AddComponent(transform_ucsGizmoY);
	PositionUCSGizmoY->AddComponent(meshRender_ucsGizmoY);
	PositionUCSGizmoY->AddComponent(bound_ucsGizmoY);
	PositionUCSGizmoY->Enable = true;

	mRenderObjects.push_back(std::move(PositionUCSGizmoY));




	auto PositionUCSGizmoZ = std::make_unique<GameObject>("PositionUCSGizmoZ");

	//构建材质
	Material* mat_ucsGizmoZ = new Material();
	mat_ucsGizmoZ->Name = "PositionUCS";
	mat_ucsGizmoZ->MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	mat_ucsGizmoZ->DiffuseColor = Color(0.0f, 0.0f, 1.0f, 1.0f);

	//	加载模型
	MeshFliter* mesh_ucsGizmoZ = new MeshFliter("MeshFliter");
	ObjLoader::LoadObj(mesh_ucsGizmoZ->data, (char*)mPositionUCSGizmoZPath.c_str());
	MeshRender* meshRender_ucsGizmoZ = new MeshRender(mesh_ucsGizmoZ, mat_ucsGizmoZ, "MeshRender");

	//	设置坐标
	Transform* transform_ucsGizmoZ = new Transform("Transform");
	transform_ucsGizmoZ->position = Vector3(0, 0, 0);
	transform_ucsGizmoZ->eulerangle = Vector3(0, 0, 0);
	transform_ucsGizmoZ->scale = Vector3(1, 1, 1);

	//	创建碰撞盒子
	DistBound::BoundingBox* bound_ucsGizmoZ = new DistBound::BoundingBox("BoundingBox");
	bound_ucsGizmoZ->aabb = BoundingAABB(mesh_ucsGizmoZ->data);
	bound_ucsGizmoZ->aabb.m_min = transform_ucsGizmoZ->GetWorldMatrix() * bound_ucsGizmoZ->aabb.m_min;
	bound_ucsGizmoZ->aabb.m_max = transform_ucsGizmoZ->GetWorldMatrix() * bound_ucsGizmoZ->aabb.m_min;

	PositionUCSGizmoZ->AddComponent(transform_ucsGizmoZ);
	PositionUCSGizmoZ->AddComponent(meshRender_ucsGizmoZ);
	PositionUCSGizmoZ->AddComponent(bound_ucsGizmoZ);
	PositionUCSGizmoZ->Enable = true;

	mRenderObjects.push_back(std::move(PositionUCSGizmoZ));

}

void GizmosManager::BuildUCSRotation(MaterialIndexUtils& matCBIndexUtils)
{
	//---------------------------------------------------------------
	//						旋转
	//---------------------------------------------------------------
	matCBIndexUtils.getInstance().SaveTypeIndex("RotationUCS", matCBIndexUtils.getInstance().GetIndex());

	auto RotationUCSGizmoX = std::make_unique<GameObject>("RotationUCSGizmoX");

	//构建材质
	Material* mat_ucsGizmoX = new Material();
	mat_ucsGizmoX->Name = "RotationUCS";
	mat_ucsGizmoX->MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	mat_ucsGizmoX->DiffuseColor = Color(1.0f, 0.0f, 0.0f, 1.0f);

	//	加载模型
	MeshFliter* mesh_ucsGizmoX = new MeshFliter("MeshFliter");
	ObjLoader::LoadObj(mesh_ucsGizmoX->data, (char*)mRotationUCSGizmoXPath.c_str());
	MeshRender* meshRender_ucsGizmoX = new MeshRender(mesh_ucsGizmoX, mat_ucsGizmoX, "MeshRender");

	//	设置坐标
	Transform* transform_ucsGizmoX = new Transform("Transform");
	transform_ucsGizmoX->position = Vector3(0, 0, 0);
	transform_ucsGizmoX->eulerangle = Vector3(0, 0, 0);
	transform_ucsGizmoX->scale = Vector3(3, 3, 3);

	//	创建碰撞盒子
	DistBound::BoundingBox* bound_ucsGizmoX = new DistBound::BoundingBox("BoundingBox");
	bound_ucsGizmoX->aabb = BoundingAABB(mesh_ucsGizmoX->data);
	bound_ucsGizmoX->aabb.m_min = transform_ucsGizmoX->GetWorldMatrix() * bound_ucsGizmoX->aabb.m_min;
	bound_ucsGizmoX->aabb.m_max = transform_ucsGizmoX->GetWorldMatrix() * bound_ucsGizmoX->aabb.m_min;

	RotationUCSGizmoX->AddComponent(transform_ucsGizmoX);
	RotationUCSGizmoX->AddComponent(meshRender_ucsGizmoX);
	RotationUCSGizmoX->AddComponent(bound_ucsGizmoX);
	RotationUCSGizmoX->Enable = true;

	mRenderObjects.push_back(std::move(RotationUCSGizmoX));




	auto RotationUCSGizmoY = std::make_unique<GameObject>("RotationUCSGizmoY");

	//构建材质
	Material* mat_ucsGizmoY = new Material();
	mat_ucsGizmoY->Name = "RotationUCS";
	mat_ucsGizmoY->MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	mat_ucsGizmoY->DiffuseColor = Color(0.0f, 1.0f, 0.0f, 1.0f);

	//	加载模型
	MeshFliter* mesh_ucsGizmoY = new MeshFliter("MeshFliter");
	ObjLoader::LoadObj(mesh_ucsGizmoY->data, (char*)mRotationUCSGizmoYPath.c_str());
	MeshRender* meshRender_ucsGizmoY = new MeshRender(mesh_ucsGizmoY, mat_ucsGizmoY, "MeshRender");

	//	设置坐标
	Transform* transform_ucsGizmoY = new Transform("Transform");
	transform_ucsGizmoY->position = Vector3(0, 0, 0);
	transform_ucsGizmoY->eulerangle = Vector3(0, 0, 0);
	transform_ucsGizmoY->scale = Vector3(3, 3, 3);

	//	创建碰撞盒子
	DistBound::BoundingBox* bound_ucsGizmoY = new DistBound::BoundingBox("BoundingBox");
	bound_ucsGizmoY->aabb = BoundingAABB(mesh_ucsGizmoY->data);
	bound_ucsGizmoY->aabb.m_min = transform_ucsGizmoY->GetWorldMatrix() * bound_ucsGizmoY->aabb.m_min;
	bound_ucsGizmoY->aabb.m_max = transform_ucsGizmoY->GetWorldMatrix() * bound_ucsGizmoY->aabb.m_min;

	RotationUCSGizmoY->AddComponent(transform_ucsGizmoY);
	RotationUCSGizmoY->AddComponent(meshRender_ucsGizmoY);
	RotationUCSGizmoY->AddComponent(bound_ucsGizmoY);
	RotationUCSGizmoY->Enable = true;

	mRenderObjects.push_back(std::move(RotationUCSGizmoY));




	auto RotationUCSGizmoZ = std::make_unique<GameObject>("RotationUCSGizmoZ");

	//构建材质
	Material* mat_ucsGizmoZ = new Material();
	mat_ucsGizmoZ->Name = "RotationUCS";
	mat_ucsGizmoZ->MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	mat_ucsGizmoZ->DiffuseColor = Color(0.0f, 0.0f, 1.0f, 1.0f);

	//	加载模型
	MeshFliter* mesh_ucsGizmoZ = new MeshFliter("MeshFliter");
	ObjLoader::LoadObj(mesh_ucsGizmoZ->data, (char*)mRotationUCSGizmoZPath.c_str());
	MeshRender* meshRender_ucsGizmoZ = new MeshRender(mesh_ucsGizmoZ, mat_ucsGizmoZ, "MeshRender");

	//	设置坐标
	Transform* transform_ucsGizmoZ = new Transform("Transform");
	transform_ucsGizmoZ->position = Vector3(0, 0, 0);
	transform_ucsGizmoZ->eulerangle = Vector3(0, 0, 0);
	transform_ucsGizmoZ->scale = Vector3(3, 3, 3);

	//	创建碰撞盒子
	DistBound::BoundingBox* bound_ucsGizmoZ = new DistBound::BoundingBox("BoundingBox");
	bound_ucsGizmoZ->aabb = BoundingAABB(mesh_ucsGizmoZ->data);
	bound_ucsGizmoZ->aabb.m_min = transform_ucsGizmoZ->GetWorldMatrix() * bound_ucsGizmoZ->aabb.m_min;
	bound_ucsGizmoZ->aabb.m_max = transform_ucsGizmoZ->GetWorldMatrix() * bound_ucsGizmoZ->aabb.m_min;

	RotationUCSGizmoZ->AddComponent(transform_ucsGizmoZ);
	RotationUCSGizmoZ->AddComponent(meshRender_ucsGizmoZ);
	RotationUCSGizmoZ->AddComponent(bound_ucsGizmoZ);
	RotationUCSGizmoZ->Enable = true;

	mRenderObjects.push_back(std::move(RotationUCSGizmoZ));
}

void GizmosManager::BuildUCSScale(MaterialIndexUtils& matCBIndexUtils)
{

	//---------------------------------------------------------------
	//						缩放
	//---------------------------------------------------------------
	matCBIndexUtils.getInstance().SaveTypeIndex("ScaleUCS", matCBIndexUtils.getInstance().GetIndex());

	auto ScaleUCSGizmoX = std::make_unique<GameObject>("ScaleUCSGizmoX");

	//构建材质
	Material* mat_ucsGizmoX = new Material();
	mat_ucsGizmoX->Name = "ScaleUCS";
	mat_ucsGizmoX->MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	mat_ucsGizmoX->DiffuseColor = Color(1.0f, 0.0f, 0.0f, 1.0f);

	//	加载模型
	MeshFliter* mesh_ucsGizmoX = new MeshFliter("MeshFliter");
	ObjLoader::LoadObj(mesh_ucsGizmoX->data, (char*)mScaleUCSGizmoXPath.c_str());
	MeshRender* meshRender_ucsGizmoX = new MeshRender(mesh_ucsGizmoX, mat_ucsGizmoX, "MeshRender");

	//	设置坐标
	Transform* transform_ucsGizmoX = new Transform("Transform");
	transform_ucsGizmoX->position = Vector3(0, 0, 0);
	transform_ucsGizmoX->eulerangle = Vector3(0, 0, 0);
	transform_ucsGizmoX->scale = Vector3(3, 3, 3);

	//	创建碰撞盒子
	DistBound::BoundingBox* bound_ucsGizmoX = new DistBound::BoundingBox("BoundingBox");
	bound_ucsGizmoX->aabb = BoundingAABB(mesh_ucsGizmoX->data);
	bound_ucsGizmoX->aabb.m_min = transform_ucsGizmoX->GetWorldMatrix() * bound_ucsGizmoX->aabb.m_min;
	bound_ucsGizmoX->aabb.m_max = transform_ucsGizmoX->GetWorldMatrix() * bound_ucsGizmoX->aabb.m_min;

	ScaleUCSGizmoX->AddComponent(transform_ucsGizmoX);
	ScaleUCSGizmoX->AddComponent(meshRender_ucsGizmoX);
	ScaleUCSGizmoX->AddComponent(bound_ucsGizmoX);
	ScaleUCSGizmoX->Enable = true;

	mRenderObjects.push_back(std::move(ScaleUCSGizmoX));




	auto ScaleUCSGizmoY = std::make_unique<GameObject>("ScaleUCSGizmoY");

	//构建材质
	Material* mat_ucsGizmoY = new Material();
	mat_ucsGizmoY->Name = "ScaleUCS";
	mat_ucsGizmoY->MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	mat_ucsGizmoY->DiffuseColor = Color(0.0f, 1.0f, 0.0f, 1.0f);

	//	加载模型
	MeshFliter* mesh_ucsGizmoY = new MeshFliter("MeshFliter");
	ObjLoader::LoadObj(mesh_ucsGizmoY->data, (char*)mScaleUCSGizmoYPath.c_str());
	MeshRender* meshRender_ucsGizmoY = new MeshRender(mesh_ucsGizmoY, mat_ucsGizmoY, "MeshRender");

	//	设置坐标
	Transform* transform_ucsGizmoY = new Transform("Transform");
	transform_ucsGizmoY->position = Vector3(0, 0, 0);
	transform_ucsGizmoY->eulerangle = Vector3(0, 0, 0);
	transform_ucsGizmoY->scale = Vector3(3, 3, 3);

	//	创建碰撞盒子
	DistBound::BoundingBox* bound_ucsGizmoY = new DistBound::BoundingBox("BoundingBox");
	bound_ucsGizmoY->aabb = BoundingAABB(mesh_ucsGizmoY->data);
	bound_ucsGizmoY->aabb.m_min = transform_ucsGizmoY->GetWorldMatrix() * bound_ucsGizmoY->aabb.m_min;
	bound_ucsGizmoY->aabb.m_max = transform_ucsGizmoY->GetWorldMatrix() * bound_ucsGizmoY->aabb.m_min;

	ScaleUCSGizmoY->AddComponent(transform_ucsGizmoY);
	ScaleUCSGizmoY->AddComponent(meshRender_ucsGizmoY);
	ScaleUCSGizmoY->AddComponent(bound_ucsGizmoY);
	ScaleUCSGizmoY->Enable = true;

	mRenderObjects.push_back(std::move(ScaleUCSGizmoY));




	auto ScaleUCSGizmoZ = std::make_unique<GameObject>("ScaleUCSGizmoZ");

	//构建材质
	Material* mat_ucsGizmoZ = new Material();
	mat_ucsGizmoZ->Name = "ScaleUCS";
	mat_ucsGizmoZ->MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	mat_ucsGizmoZ->DiffuseColor = Color(0.0f, 0.0f, 1.0f, 1.0f);

	//	加载模型
	MeshFliter* mesh_ucsGizmoZ = new MeshFliter("MeshFliter");
	ObjLoader::LoadObj(mesh_ucsGizmoZ->data, (char*)mScaleUCSGizmoZPath.c_str());
	MeshRender* meshRender_ucsGizmoZ = new MeshRender(mesh_ucsGizmoZ, mat_ucsGizmoZ, "MeshRender");

	//	设置坐标
	Transform* transform_ucsGizmoZ = new Transform("Transform");
	transform_ucsGizmoZ->position = Vector3(0, 0, 0);
	transform_ucsGizmoZ->eulerangle = Vector3(0, 0, 0);
	transform_ucsGizmoZ->scale = Vector3(1, 1, 1);

	//	创建碰撞盒子
	DistBound::BoundingBox* bound_ucsGizmoZ = new DistBound::BoundingBox("BoundingBox");
	bound_ucsGizmoZ->aabb = BoundingAABB(mesh_ucsGizmoZ->data);
	bound_ucsGizmoZ->aabb.m_min = transform_ucsGizmoZ->GetWorldMatrix() * bound_ucsGizmoZ->aabb.m_min;
	bound_ucsGizmoZ->aabb.m_max = transform_ucsGizmoZ->GetWorldMatrix() * bound_ucsGizmoZ->aabb.m_min;

	ScaleUCSGizmoZ->AddComponent(transform_ucsGizmoZ);
	ScaleUCSGizmoZ->AddComponent(meshRender_ucsGizmoZ);
	ScaleUCSGizmoZ->AddComponent(bound_ucsGizmoZ);
	ScaleUCSGizmoZ->Enable = true;

	mRenderObjects.push_back(std::move(ScaleUCSGizmoZ));
}

void GizmosManager::BuildScene(
	std::unordered_map<std::string, std::unique_ptr<Texture2D>>& mGizmosTextures,
	MaterialIndexUtils& matCBIndexUtils
)
{
	BuildWirePlane(matCBIndexUtils);

	BuildLightGizmo(mGizmosTextures, matCBIndexUtils);

	BuildUCSPosition(matCBIndexUtils);

	BuildUCSRotation(matCBIndexUtils);

	BuildUCSScale(matCBIndexUtils);
}


void GizmosManager::UpdateSceneMaterialBuffer(UploadBuffer<PBRMaterialData>* PBRMaterialBuffer)
{
	for (size_t i = 0; i < mRenderObjects.size(); i++)
	{
		Material* mat = mRenderObjects[i]->GetComponent<MeshRender>(2)->mat;

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

void GizmosManager::BuildRenderItem(
	std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count],
	std::vector<std::unique_ptr<RenderItem>>& mAllRitems,
	Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice,
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList
)
{
	for (size_t i = 0; i < mRenderObjects.size(); i++)
	{
		auto Ritem = std::make_unique<RenderItem>();
		Ritem->World = mRenderObjects[i]->GetComponent<Transform>(2)->GetWorldXMMatrix();
		Ritem->TexTransform = Mathf::Identity4x4();
		Ritem->ObjCBIndex = i;
		Ritem->Mat = mRenderObjects[i]->GetComponent<MeshRender>(2)->mat;
		Ritem->Geo = GraphicsUtils::BuidlMeshGeometryFromMeshData(mRenderObjects[i]->name, mRenderObjects[i]->GetComponent<MeshRender>(2)->mesh->data, md3dDevice, mCommandList);
		Ritem->IndexCount = Ritem->Geo->DrawArgs["mesh"].IndexCount;
		Ritem->StartIndexLocation = Ritem->Geo->DrawArgs["mesh"].StartIndexLocation;
		Ritem->BaseVertexLocation = Ritem->Geo->DrawArgs["mesh"].BaseVertexLocation;
		Ritem->Bound = mRenderObjects[i]->GetComponent<DistBound::BoundingBox>(3)->aabb.ToBoundBox();
		Ritem->Enable = mRenderObjects[i]->Enable;

		if (i < 1)
		{
			Ritem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
			mRitemLayer[(int)RenderLayer::Line].push_back(Ritem.get());
		}
		else if(i >= 1 && i < 2)
		{
			Ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			mRitemLayer[(int)RenderLayer::Gizmo].push_back(Ritem.get());
		}
		else
		{
			Ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			mRitemLayer[(int)RenderLayer::Axis].push_back(Ritem.get());
		}

		mAllRitems.push_back(std::move(Ritem));
	}
}

/// <summary>
/// 更新CBuffer
/// </summary>
void GizmosManager::UpdateObjectBuffer(std::vector<std::unique_ptr<RenderItem>>& mAllRitems, Transform* mMainLightPos, std::unique_ptr<GameObject>& mTargetRender)
{
	for (size_t i = 0; i < mAllRitems.size(); i++)
	{
		for (size_t j = 0; j < mRenderObjects.size(); j++)
		{

			//	LightGizmo
			if (j > 0 && j <2)
			{
				if (mAllRitems[i]->ObjCBIndex == j)
				{
					mRenderObjects[j]->GetComponent<Transform>(1)->SetPosition(mMainLightPos->position);
					mAllRitems[i]->Enable = mRenderObjects[j]->Enable;
					mAllRitems[i]->World = mRenderObjects[j]->GetComponent<Transform>(1)->GetWorldXMMatrix();
				}
			}
			//	坐标
			else if (j >= 2 && j < 5)
			{
				if (mAllRitems[i]->ObjCBIndex == j)
				{
					mRenderObjects[j]->GetComponent<Transform>(1)->SetPosition(mTargetRender->GetComponent<Transform>(1)->position);
					mAllRitems[i]->Enable = PositionUCSEnable;
					mAllRitems[i]->World = mRenderObjects[j]->GetComponent<Transform>(1)->GetWorldXMMatrix();
				}
			}
			else if(j >= 5 && j < 8)
			{
				if (mAllRitems[i]->ObjCBIndex == j)
				{
					mRenderObjects[j]->GetComponent<Transform>(1)->SetPosition(mTargetRender->GetComponent<Transform>(1)->position);
					mAllRitems[i]->Enable = RotationUCSEnable;
					mAllRitems[i]->World = mRenderObjects[j]->GetComponent<Transform>(1)->GetWorldXMMatrix();
				}
			}
			else if (j >= 8 && j < 11)
			{
				if (mAllRitems[i]->ObjCBIndex == j)
				{
					mRenderObjects[j]->GetComponent<Transform>(1)->SetPosition(mTargetRender->GetComponent<Transform>(1)->position);
					mAllRitems[i]->Enable = ScaleUCSEnable;
					mAllRitems[i]->World = mRenderObjects[j]->GetComponent<Transform>(1)->GetWorldXMMatrix();
				}
			}
		}
	}
}

/// <summary>
/// 显示坐标
/// </summary>
void GizmosManager::ShowPosition()
{
	if (PositionUCSEnable)
	{
		PositionUCSEnable = false;
		RotationUCSEnable = false;
		ScaleUCSEnable = false;
	}
	else
	{
		PositionUCSEnable = true;
		RotationUCSEnable = false;
		ScaleUCSEnable = false;
	}
}

/// <summary>
/// 显示旋转
/// </summary>
void GizmosManager::ShowRotation()
{
	if (RotationUCSEnable)
	{
		PositionUCSEnable = false;
		RotationUCSEnable = false;
		ScaleUCSEnable = false;
	}
	else
	{
		PositionUCSEnable = false;
		RotationUCSEnable = true;
		ScaleUCSEnable = false;
	}
}

/// <summary>
/// 显示缩放
/// </summary>
void GizmosManager::ShowScale()
{
	if (ScaleUCSEnable)
	{
		PositionUCSEnable = false;
		RotationUCSEnable = false;
		ScaleUCSEnable = false;
	}
	else
	{
		PositionUCSEnable = false;
		RotationUCSEnable = false;
		ScaleUCSEnable = true;
	}
}