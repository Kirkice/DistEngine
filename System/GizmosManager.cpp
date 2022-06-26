#include "GizmosManager.h"

void GizmosManager::BuildScene(
	std::unordered_map<std::string, std::unique_ptr<Texture2D>>& mGizmosTextures,
	MaterialIndexUtils& matCBIndexUtils
)
{
	//	存储类型matCB开始值
	matCBIndexUtils.getInstance().SaveTypeIndex("Gizmo", matCBIndexUtils.getInstance().GetIndex());

	//	Plane
	auto plane = std::make_unique<MeshRender>();
	plane->name = "Plane";

	//构建材质
	plane->material.Name = "Plane_mat";
	plane->material.MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	plane->material.DiffuseColor = Color(0.8f, 0.8f, 0.8f, 1.0f);

	//	创建平面网格
	plane->mesh.CreateGrid(60, 60, 240, 240);

	//	设置坐标
	plane->position = Vector3(0, 0, 0);
	plane->eulerangle = Vector3(0, 0, 0);
	plane->scale = Vector3(10, 10, 10);

	//	创建碰撞盒
	plane->bound.aabb = BoundingAABB(plane->mesh.data);
	mMeshRender.push_back(std::move(plane));


	//	灯光图标
	auto DirectionLightGizmo = std::make_unique<MeshRender>();
	DirectionLightGizmo->name = "DirectionLightGizmo";

	//	构建材质
	DirectionLightGizmo->material.Name = "DirGizmo";
	DirectionLightGizmo->material.MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();
	DirectionLightGizmo->material.DiffuseColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
	DirectionLightGizmo->material.DiffuseMapIndex = mGizmosTextures["DirectionalLight"]->TexIndex;

	//	创建平面网格
	DirectionLightGizmo->mesh.CreateGrid(6, 6, 4, 4);

	//	设置坐标
	DirectionLightGizmo->position = Vector3(-20, 30, 20);
	DirectionLightGizmo->eulerangle = Vector3(0, 0, 0);
	DirectionLightGizmo->scale = Vector3(0.5f, 0.5f, 0.5f);

	//	创建碰撞盒
	DirectionLightGizmo->bound.aabb = BoundingAABB(DirectionLightGizmo->mesh.data);
	mMeshRender.push_back(std::move(DirectionLightGizmo));
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

		if (i < 1)
		{
			Ritem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
			mRitemLayer[(int)RenderLayer::Line].push_back(Ritem.get());
		}
		else
		{
			Ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			mRitemLayer[(int)RenderLayer::Gizmo].push_back(Ritem.get());
		}

		mAllRitems.push_back(std::move(Ritem));
	}
}

/// <summary>
/// 更新CBuffer
/// </summary>
void GizmosManager::UpdateObjectBuffer(std::vector<std::unique_ptr<RenderItem>>& mAllRitems, DirectionLight& mMainLight)
{
	for (size_t i = 0; i < mAllRitems.size(); i++)
	{
		for (size_t j = 0; j < mMeshRender.size(); j++)
		{
			
			if (j > 0)
			{
				if (mAllRitems[i]->ObjCBIndex == j)
				{
					mMeshRender[j]->SetPosition(mMainLight.position);
					mAllRitems[i]->World = mMeshRender[j]->GetWorldXMMatrix();
				}
			}
		}
	}
}