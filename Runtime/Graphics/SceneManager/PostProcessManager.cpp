#include "PostProcessManager.h"

void PostProcessManager::Build(MaterialIndexUtils& matCBIndexUtils)
{
	/// <summary>
	/// 体积雾
	/// </summary>
	/// <param name="matCBIndexUtils"></param>
	auto VolumeFog = std::make_unique<GameObject>("VolumeFog");

	//构建材质
	Material* mat_volumeFog = new Material();
	mat_volumeFog->Name = "VolumeFog_mat";
	mat_volumeFog->MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();

	//	创建平面网格
	MeshFliter* mesh_volumeFog = new MeshFliter("MeshFliter");
	mesh_volumeFog->CreateQuad(0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	MeshRender* meshRender_volumeFog = new MeshRender(mesh_volumeFog, mat_volumeFog, "MeshRender");

	//	设置坐标
	Transform* transform_volumeFog = new Transform("Transform");
	transform_volumeFog->position = Vector3(0, 0, 0);
	transform_volumeFog->eulerangle = Vector3(0, 0, 0);
	transform_volumeFog->scale = Vector3(1, 1, 1);

	//	创建碰撞盒子
	DistBound::BoundingBox* bound_volumeFog = new DistBound::BoundingBox("BoundingBox");
	bound_volumeFog->aabb = BoundingAABB(mesh_volumeFog->data);

	VolumeFog->AddComponent(transform_volumeFog);
	VolumeFog->AddComponent(meshRender_volumeFog);
	VolumeFog->AddComponent(bound_volumeFog);

	VolumeFog->Enable = true;
	mRenderObjects.push_back(std::move(VolumeFog));

	/// <summary>
	/// FxAA
	/// </summary>
	/// <param name="matCBIndexUtils"></param>
	auto FastApproximateAntialiasing = std::make_unique<GameObject>("FastApproximateAntialiasing");

	//构建材质
	Material* mat_fxaa = new Material();
	mat_fxaa->Name = "FastApproximateAntialiasing_mat";
	mat_fxaa->MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();

	//	创建平面网格
	MeshFliter* mesh_fxaa = new MeshFliter("MeshFliter");
	mesh_fxaa->CreateQuad(0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	MeshRender* meshRender_fxaa = new MeshRender(mesh_fxaa, mat_fxaa, "MeshRender");

	//	设置坐标
	Transform* transform_fxaa = new Transform("Transform");
	transform_fxaa->position = Vector3(0, 0, 0);
	transform_fxaa->eulerangle = Vector3(0, 0, 0);
	transform_fxaa->scale = Vector3(1, 1, 1);

	//	创建碰撞盒
	DistBound::BoundingBox* bound_fxaa = new DistBound::BoundingBox("BoundingBox");
	bound_fxaa->aabb = BoundingAABB(mesh_fxaa->data);

	FastApproximateAntialiasing->AddComponent(transform_fxaa);
	FastApproximateAntialiasing->AddComponent(meshRender_fxaa);
	FastApproximateAntialiasing->AddComponent(bound_fxaa);

	FastApproximateAntialiasing->Enable = true;
	mRenderObjects.push_back(std::move(FastApproximateAntialiasing));

}

void PostProcessManager::UpdateMaterialBuffer(
	UploadBuffer<PostprocessingData>* PostProcessBuffer,
	MaterialIndexUtils& matCBIndexUtils
)
{
	for (size_t i = 0; i < mRenderObjects.size(); i++)
	{
		Material* mat = mRenderObjects[i]->GetComponent<MeshRender>(1)->mat;

		PostprocessingData matData;
		matData.RGBSplitStrength = mat->RGBSplitStrength;
		matData.DecolorStrength = mat->DecolorStrength;
		matData.Threshold = mat->Threshold;
		matData.SoftKnee = mat->SoftKnee;
		matData.Radius = mat->Radius;
		matData.Intensity = mat->Intensity;
		matData.Brightness = mat->Brightness;
		matData.Contrast = mat->Contrast;
		matData.Hue = mat->Hue;
		matData.Saturation = mat->Saturation;
		matData.Value = mat->Value;
		matData.PixelSize = mat->PixelSize;
		matData.BlurFactory = mat->BlurFactory;

		matData.EdgeWdith = mat->EdgeWdith;
		matData.EdgeNeonFade = mat->EdgeNeonFade;
		matData.OutLineColor = mat->OutLineColor;
		matData.BackgroundFade = mat->BackgroundFade;
		matData.BackgroundColor = mat->BackgroundColor;
		matData.Temperature = mat->Temperature;
		matData.Tint = mat->WhiteBalanceTint;
		matData.VignetteColor = mat->VignetteColor;
		matData.Width = mat->Width;
		matData.Height = mat->Height;
		matData.Smooth = mat->Smooth;
		matData.Alpha = mat->Alpha;
		matData.SharpenStrength = mat->SharpenStrength;
		matData.SharpenThreshold = mat->SharpenThreshold;
		matData.Spherify = mat->Spherify;
		matData.OilPaintRadius = mat->OilPaintRadius;
		matData.ResoultionValue = mat->ResoultionValue;
		matData.MotionBlurAmount = mat->MotionBlurAmount;
		matData.Aperture = mat->Aperture;
		matData.FocalLength - mat->FocalLength;
		matData.Distance = mat->Distance;
		matData.ResoultionLevel = mat->ResoultionLevel;
		matData.FSRSharpen = matData.FSRSharpen;
		PostProcessBuffer->CopyData(mat->MatCBIndex, matData);
		mat->NumFramesDirty--;
	}
}

void PostProcessManager::BuildRenderItem(
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
		Ritem->World = mRenderObjects[i]->GetComponent<Transform>(0)->GetWorldXMMatrix();
		Ritem->TexTransform = Mathf::Identity4x4();
		Ritem->ObjCBIndex = CurrentSize + i;
		Ritem->Mat = mRenderObjects[i]->GetComponent<MeshRender>(1)->mat;
		Ritem->Geo = GraphicsUtils::BuidlMeshGeometryFromMeshData(mRenderObjects[i]->name, mRenderObjects[i]->GetComponent<MeshRender>(1)->mesh->data, md3dDevice, mCommandList);
		Ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		Ritem->IndexCount = Ritem->Geo->DrawArgs["mesh"].IndexCount;
		Ritem->StartIndexLocation = Ritem->Geo->DrawArgs["mesh"].StartIndexLocation;
		Ritem->BaseVertexLocation = Ritem->Geo->DrawArgs["mesh"].BaseVertexLocation;
		Ritem->Bound = mRenderObjects[i]->GetComponent<DistBound::BoundingBox>(2)->aabb.ToBoundBox();
		Ritem->Enable = mRenderObjects[i]->Enable;

		mRitemLayer[(int)RenderLayer::PostProcess].push_back(Ritem.get());
		mAllRitems.push_back(std::move(Ritem));
	}
}