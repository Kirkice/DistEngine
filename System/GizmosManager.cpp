#include "GizmosManager.h"
#include "ResourcesPath.h"

void GizmosManager::BuildScene(
	std::unordered_map<std::string, std::unique_ptr<Texture2D>>& mGizmosTextures,
	MaterialIndexUtils& matCBIndexUtils
)
{
	//	�洢����matCB��ʼֵ
	matCBIndexUtils.getInstance().SaveTypeIndex("Gizmo", matCBIndexUtils.getInstance().GetIndex());

	//	Plane
	auto plane = std::make_unique<MeshRender>();
	plane->name = "Plane";

	//��������
	plane->material.Name = "Plane_mat";
	plane->material.MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	plane->material.DiffuseColor = Color(0.8f, 0.8f, 0.8f, 1.0f);

	//	����ƽ������
	plane->mesh.CreateGrid(60, 60, 240, 240);

	//	��������
	plane->position = Vector3(0, 0, 0);
	plane->eulerangle = Vector3(0, 0, 0);
	plane->scale = Vector3(10, 10, 10);

	//	������ײ��
	plane->bound.aabb = BoundingAABB(plane->mesh.data);
	plane->Enable = true;
	mMeshRender.push_back(std::move(plane));


	//	�ƹ�ͼ��
	auto DirectionLightGizmo = std::make_unique<MeshRender>();
	DirectionLightGizmo->name = "DirectionLightGizmo";

	//	��������
	DirectionLightGizmo->material.Name = "DirGizmo";
	DirectionLightGizmo->material.MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	DirectionLightGizmo->material.DiffuseColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
	DirectionLightGizmo->material.DiffuseMapIndex = mGizmosTextures["DirectionalLight"]->TexIndex;

	//	����ƽ������
	DirectionLightGizmo->mesh.CreateGrid(6, 6, 4, 4);

	//	��������
	DirectionLightGizmo->position = Vector3(-20, 30, 20);
	DirectionLightGizmo->eulerangle = Vector3(0, 0, 0);
	DirectionLightGizmo->scale = Vector3(0.5f, 0.5f, 0.5f);

	//	������ײ��
	DirectionLightGizmo->bound.aabb = BoundingAABB(DirectionLightGizmo->mesh.data);
	DirectionLightGizmo->Enable = true;
	mMeshRender.push_back(std::move(DirectionLightGizmo));


	//---------------------------------------------------------------
	//						����
	//---------------------------------------------------------------
	matCBIndexUtils.getInstance().SaveTypeIndex("PositionUCS", matCBIndexUtils.getInstance().GetIndex());
	auto PositionUCSGizmoX = std::make_unique<MeshRender>();
	PositionUCSGizmoX->name = "PositionUCS";
	//	��������
	PositionUCSGizmoX->material.Name = "PositionUCS";
	PositionUCSGizmoX->material.MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	PositionUCSGizmoX->material.DiffuseColor = Color(1.0f, 0.0f, 0.0f, 1.0f);
	ObjLoader::LoadObj(PositionUCSGizmoX->mesh.data, (char*)mPositionUCSGizmoXPath.c_str());
	//	��������
	PositionUCSGizmoX->position = Vector3(0, 0, 0);
	PositionUCSGizmoX->eulerangle = Vector3(0, 0, 0);
	PositionUCSGizmoX->scale = Vector3(1, 1, 1);
	PositionUCSGizmoX->bound.aabb = BoundingAABB(PositionUCSGizmoX->mesh.data);
	PositionUCSGizmoX->Enable = PositionUCSEnable;
	PositionUCSGizmoX->bound.aabb.m_min = PositionUCSGizmoX->GetWorldMatrix() * PositionUCSGizmoX->bound.aabb.m_min;
	PositionUCSGizmoX->bound.aabb.m_max = PositionUCSGizmoX->GetWorldMatrix() * PositionUCSGizmoX->bound.aabb.m_max;
	mMeshRender.push_back(std::move(PositionUCSGizmoX));

	auto PositionUCSGizmoY = std::make_unique<MeshRender>();
	PositionUCSGizmoY->name = "PositionUCS";
	//	��������
	PositionUCSGizmoY->material.Name = "PositionUCS";
	PositionUCSGizmoY->material.MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	PositionUCSGizmoY->material.DiffuseColor = Color(0.0f, 1.0f, 0.0f, 1.0f);
	ObjLoader::LoadObj(PositionUCSGizmoY->mesh.data, (char*)mPositionUCSGizmoYPath.c_str());
	//	��������
	PositionUCSGizmoY->position = Vector3(0, 0, 0);
	PositionUCSGizmoY->eulerangle = Vector3(0, 0, 0);
	PositionUCSGizmoY->scale = Vector3(1, 1, 1);
	PositionUCSGizmoY->bound.aabb = BoundingAABB(PositionUCSGizmoY->mesh.data);
	PositionUCSGizmoY->Enable = PositionUCSEnable;
	PositionUCSGizmoY->bound.aabb.m_min = PositionUCSGizmoY->GetWorldMatrix() * PositionUCSGizmoY->bound.aabb.m_min;
	PositionUCSGizmoY->bound.aabb.m_max = PositionUCSGizmoY->GetWorldMatrix() * PositionUCSGizmoY->bound.aabb.m_max;
	mMeshRender.push_back(std::move(PositionUCSGizmoY));

	auto PositionUCSGizmoZ = std::make_unique<MeshRender>();
	PositionUCSGizmoZ->name = "PositionUCS";
	//	��������
	PositionUCSGizmoZ->material.Name = "PositionUCS";
	PositionUCSGizmoZ->material.MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	PositionUCSGizmoZ->material.DiffuseColor = Color(0.0f, 0.0f, 1.0f, 1.0f);
	ObjLoader::LoadObj(PositionUCSGizmoZ->mesh.data, (char*)mPositionUCSGizmoZPath.c_str());
	//	��������
	PositionUCSGizmoZ->position = Vector3(0, 0, 0);
	PositionUCSGizmoZ->eulerangle = Vector3(0, 0, 0);
	PositionUCSGizmoZ->scale = Vector3(1, 1, 1);
	PositionUCSGizmoZ->bound.aabb = BoundingAABB(PositionUCSGizmoZ->mesh.data);
	PositionUCSGizmoZ->Enable = PositionUCSEnable;
	PositionUCSGizmoZ->bound.aabb.m_min = PositionUCSGizmoZ->GetWorldMatrix() * PositionUCSGizmoZ->bound.aabb.m_min;
	PositionUCSGizmoZ->bound.aabb.m_max = PositionUCSGizmoZ->GetWorldMatrix() * PositionUCSGizmoZ->bound.aabb.m_max;
	mMeshRender.push_back(std::move(PositionUCSGizmoZ));

	//---------------------------------------------------------------
	//						��ת
	//---------------------------------------------------------------
	matCBIndexUtils.getInstance().SaveTypeIndex("RotationUCS", matCBIndexUtils.getInstance().GetIndex());
	auto RotationUCSGizmoX = std::make_unique<MeshRender>();
	RotationUCSGizmoX->name = "RotationXUCS";
	//	��������
	RotationUCSGizmoX->material.Name = "RotationXUCS";
	RotationUCSGizmoX->material.MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	RotationUCSGizmoX->material.DiffuseColor = Color(1.0f, 0.0f, 0.0f, 1.0f);
	ObjLoader::LoadObj(RotationUCSGizmoX->mesh.data, (char*)mRotationUCSGizmoXPath.c_str());
	//	��������
	RotationUCSGizmoX->position = Vector3(0, 0, 0);
	RotationUCSGizmoX->eulerangle = Vector3(0, 0, 0);
	RotationUCSGizmoX->scale = Vector3(3, 3, 3);
	RotationUCSGizmoX->bound.aabb = BoundingAABB(RotationUCSGizmoX->mesh.data);
	RotationUCSGizmoX->Enable = RotationUCSEnable;
	RotationUCSGizmoX->bound.aabb.m_min = RotationUCSGizmoX->GetWorldMatrix() * RotationUCSGizmoX->bound.aabb.m_min;
	RotationUCSGizmoX->bound.aabb.m_max = RotationUCSGizmoX->GetWorldMatrix() * RotationUCSGizmoX->bound.aabb.m_max;
	mMeshRender.push_back(std::move(RotationUCSGizmoX));

	auto RotationUCSGizmoY = std::make_unique<MeshRender>();
	RotationUCSGizmoY->name = "RotationYUCS";
	//	��������
	RotationUCSGizmoY->material.Name = "RotationYUCS";
	RotationUCSGizmoY->material.MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	RotationUCSGizmoY->material.DiffuseColor = Color(0.0f, 1.0f, 0.0f, 1.0f);
	ObjLoader::LoadObj(RotationUCSGizmoY->mesh.data, (char*)mRotationUCSGizmoYPath.c_str());
	//	��������
	RotationUCSGizmoY->position = Vector3(0, 0, 0);
	RotationUCSGizmoY->eulerangle = Vector3(0, 0, 0);
	RotationUCSGizmoY->scale = Vector3(3, 3, 3);
	RotationUCSGizmoY->bound.aabb = BoundingAABB(RotationUCSGizmoY->mesh.data);
	RotationUCSGizmoY->Enable = RotationUCSEnable;
	RotationUCSGizmoY->bound.aabb.m_min = RotationUCSGizmoY->GetWorldMatrix() * RotationUCSGizmoY->bound.aabb.m_min;
	RotationUCSGizmoY->bound.aabb.m_max = RotationUCSGizmoY->GetWorldMatrix() * RotationUCSGizmoY->bound.aabb.m_max;
	mMeshRender.push_back(std::move(RotationUCSGizmoY));

	auto RotationUCSGizmoZ = std::make_unique<MeshRender>();
	RotationUCSGizmoZ->name = "RotationZUCS";
	//	��������
	RotationUCSGizmoZ->material.Name = "RotationZUCS";
	RotationUCSGizmoZ->material.MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	RotationUCSGizmoZ->material.DiffuseColor = Color(0.0f, 0.0f, 1.0f, 1.0f);
	ObjLoader::LoadObj(RotationUCSGizmoZ->mesh.data, (char*)mRotationUCSGizmoZPath.c_str());
	//	��������
	RotationUCSGizmoZ->position = Vector3(0, 0, 0);
	RotationUCSGizmoZ->eulerangle = Vector3(0, 0, 0);
	RotationUCSGizmoZ->scale = Vector3(3, 3, 3);
	RotationUCSGizmoZ->bound.aabb = BoundingAABB(RotationUCSGizmoZ->mesh.data);
	RotationUCSGizmoZ->Enable = RotationUCSEnable;
	RotationUCSGizmoZ->bound.aabb.m_min = RotationUCSGizmoZ->GetWorldMatrix() * RotationUCSGizmoZ->bound.aabb.m_min;
	RotationUCSGizmoZ->bound.aabb.m_max = RotationUCSGizmoZ->GetWorldMatrix() * RotationUCSGizmoZ->bound.aabb.m_max;
	mMeshRender.push_back(std::move(RotationUCSGizmoZ));

	//---------------------------------------------------------------
	//						����
	//---------------------------------------------------------------
	matCBIndexUtils.getInstance().SaveTypeIndex("ScaleUCS", matCBIndexUtils.getInstance().GetIndex());
	auto ScaleUCSGizmoX = std::make_unique<MeshRender>();
	ScaleUCSGizmoX->name = "ScaleXUCS";
	//	��������
	ScaleUCSGizmoX->material.Name = "ScaleXUCS";
	ScaleUCSGizmoX->material.MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	ScaleUCSGizmoX->material.DiffuseColor = Color(1.0f, 0.0f, 0.0f, 1.0f);
	ObjLoader::LoadObj(ScaleUCSGizmoX->mesh.data, (char*)mScaleUCSGizmoXPath.c_str());
	//	��������
	ScaleUCSGizmoX->position = Vector3(0, 0, 0);
	ScaleUCSGizmoX->eulerangle = Vector3(0, 0, 0);
	ScaleUCSGizmoX->scale = Vector3(1, 1, 1);
	ScaleUCSGizmoX->bound.aabb = BoundingAABB(ScaleUCSGizmoX->mesh.data);
	ScaleUCSGizmoX->Enable = ScaleUCSEnable;
	ScaleUCSGizmoX->bound.aabb.m_min = ScaleUCSGizmoX->GetWorldMatrix() * ScaleUCSGizmoX->bound.aabb.m_min;
	ScaleUCSGizmoX->bound.aabb.m_max = ScaleUCSGizmoX->GetWorldMatrix() * ScaleUCSGizmoX->bound.aabb.m_max;
	mMeshRender.push_back(std::move(ScaleUCSGizmoX));

	auto ScaleUCSGizmoY = std::make_unique<MeshRender>();
	ScaleUCSGizmoY->name = "ScaleYUCS";
	//	��������
	ScaleUCSGizmoY->material.Name = "ScaleYUCS";
	ScaleUCSGizmoY->material.MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	ScaleUCSGizmoY->material.DiffuseColor = Color(0.0f, 1.0f, 0.0f, 1.0f);
	ObjLoader::LoadObj(ScaleUCSGizmoY->mesh.data, (char*)mScaleUCSGizmoYPath.c_str());
	//	��������
	ScaleUCSGizmoY->position = Vector3(0, 0, 0);
	ScaleUCSGizmoY->eulerangle = Vector3(0, 0, 0);
	ScaleUCSGizmoY->scale = Vector3(1, 1, 1);
	ScaleUCSGizmoY->bound.aabb = BoundingAABB(ScaleUCSGizmoY->mesh.data);
	ScaleUCSGizmoY->Enable = ScaleUCSEnable;
	ScaleUCSGizmoY->bound.aabb.m_min = ScaleUCSGizmoY->GetWorldMatrix() * ScaleUCSGizmoY->bound.aabb.m_min;
	ScaleUCSGizmoY->bound.aabb.m_max = ScaleUCSGizmoY->GetWorldMatrix() * ScaleUCSGizmoY->bound.aabb.m_max;
	mMeshRender.push_back(std::move(ScaleUCSGizmoY));

	auto ScaleUCSGizmoZ = std::make_unique<MeshRender>();
	ScaleUCSGizmoZ->name = "ScaleZUCS";
	//	��������
	ScaleUCSGizmoZ->material.Name = "ScaleZUCS";
	ScaleUCSGizmoZ->material.MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	ScaleUCSGizmoZ->material.DiffuseColor = Color(0.0f, 0.0f, 1.0f, 1.0f);
	ObjLoader::LoadObj(ScaleUCSGizmoZ->mesh.data, (char*)mScaleUCSGizmoZPath.c_str());
	//	��������
	ScaleUCSGizmoZ->position = Vector3(0, 0, 0);
	ScaleUCSGizmoZ->eulerangle = Vector3(0, 0, 0);
	ScaleUCSGizmoZ->scale = Vector3(1, 1, 1);
	ScaleUCSGizmoZ->bound.aabb = BoundingAABB(ScaleUCSGizmoZ->mesh.data);
	ScaleUCSGizmoZ->Enable = ScaleUCSEnable;
	ScaleUCSGizmoZ->bound.aabb.m_min = ScaleUCSGizmoZ->GetWorldMatrix() * ScaleUCSGizmoZ->bound.aabb.m_min;
	ScaleUCSGizmoZ->bound.aabb.m_max = ScaleUCSGizmoZ->GetWorldMatrix() * ScaleUCSGizmoZ->bound.aabb.m_max;
	mMeshRender.push_back(std::move(ScaleUCSGizmoZ));
}


void GizmosManager::UpdateSceneMaterialBuffer(UploadBuffer<PBRMaterialData>* PBRMaterialBuffer)
{
	for (size_t i = 0; i < mMeshRender.size(); i++)
	{
		Material* mat = &(mMeshRender[i]->material);

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
	for (size_t i = 0; i < mMeshRender.size(); i++)
	{
		auto Ritem = std::make_unique<RenderItem>();
		Ritem->World = mMeshRender[i]->GetWorldXMMatrix();
		Ritem->TexTransform = Mathf::Identity4x4();
		Ritem->ObjCBIndex = i;
		Ritem->Mat = &mMeshRender[i]->material;
		Ritem->Geo = GraphicsUtils::BuidlMeshGeometryFromMeshData(mMeshRender[i]->name, mMeshRender[i]->mesh.data, md3dDevice, mCommandList);
		Ritem->IndexCount = Ritem->Geo->DrawArgs["mesh"].IndexCount;
		Ritem->StartIndexLocation = Ritem->Geo->DrawArgs["mesh"].StartIndexLocation;
		Ritem->BaseVertexLocation = Ritem->Geo->DrawArgs["mesh"].BaseVertexLocation;
		Ritem->Bound = mMeshRender[i]->bound.aabb.ToBoundBox();
		Ritem->Enable = mMeshRender[i]->Enable;

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
/// ����CBuffer
/// </summary>
void GizmosManager::UpdateObjectBuffer(std::vector<std::unique_ptr<RenderItem>>& mAllRitems, DirectionLight& mMainLight, std::unique_ptr<MeshRender>& mTargetRender)
{
	for (size_t i = 0; i < mAllRitems.size(); i++)
	{
		for (size_t j = 0; j < mMeshRender.size(); j++)
		{

			//	LightGizmo
			if (j > 0 && j <2)
			{
				if (mAllRitems[i]->ObjCBIndex == j)
				{
					mMeshRender[j]->SetPosition(mMainLight.position);
					mAllRitems[i]->Enable = mMeshRender[j]->Enable;
					mAllRitems[i]->World = mMeshRender[j]->GetWorldXMMatrix();
				}
			}
			//	����
			else if (j >= 2 && j < 5)
			{
				if (mAllRitems[i]->ObjCBIndex == j)
				{
					mMeshRender[j]->SetPosition(mTargetRender->position);
					mAllRitems[i]->Enable = PositionUCSEnable;
					mAllRitems[i]->World = mMeshRender[j]->GetWorldXMMatrix();
				}
			}
			else if(j >= 5 && j < 8)
			{
				if (mAllRitems[i]->ObjCBIndex == j)
				{
					mMeshRender[j]->SetPosition(mTargetRender->position);
					mAllRitems[i]->Enable = RotationUCSEnable;
					mAllRitems[i]->World = mMeshRender[j]->GetWorldXMMatrix();
				}
			}
			else if (j >= 8 && j < 11)
			{
				if (mAllRitems[i]->ObjCBIndex == j)
				{
					mMeshRender[j]->SetPosition(mTargetRender->position);
					mAllRitems[i]->Enable = ScaleUCSEnable;
					mAllRitems[i]->World = mMeshRender[j]->GetWorldXMMatrix();
				}
			}
		}
	}
}

/// <summary>
/// ��ʾ����
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
/// ��ʾ��ת
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
/// ��ʾ����
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