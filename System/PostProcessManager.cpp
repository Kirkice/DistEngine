#include "PostProcessManager.h"

void PostProcessManager::Build(MaterialIndexUtils& matCBIndexUtils)
{
	auto VolumeFog = std::make_unique<MeshRender>();
	VolumeFog->name = "VolumeFog";
	//构建材质
	VolumeFog->material.Name = "VolumeFog_mat";
	VolumeFog->material.MatCBIndex = matCBIndexUtils.getInstance().GetIndex();
	matCBIndexUtils.getInstance().OffsetIndex();

	//	创建平面网格
	VolumeFog->mesh.CreateQuad(0.0f, 0.0f, 1.0f, 1.0f, 0.0f);

	//	创建碰撞盒
	VolumeFog->Enable = true;
	mMeshRender.push_back(std::move(VolumeFog));
}

void PostProcessManager::UpdateMaterialBuffer(
	UploadBuffer<PostprocessingData>* PostProcessBuffer,
	MaterialIndexUtils& matCBIndexUtils
)
{
	for (size_t i = 0; i < mMeshRender.size(); i++)
	{
		Material* mat = &(mMeshRender[i]->material);
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
	for (size_t i = 0; i < mMeshRender.size(); i++)
	{
		//	创建渲染项
		auto Ritem = std::make_unique<RenderItem>();
		Ritem->World = mMeshRender[i]->GetWorldXMMatrix();
		Ritem->TexTransform = Mathf::Identity4x4();
		Ritem->ObjCBIndex = CurrentSize + i;
		Ritem->Mat = &mMeshRender[i]->material;
		Ritem->Geo = GraphicsUtils::BuidlMeshGeometryFromMeshData(mMeshRender[i]->name, mMeshRender[i]->mesh.data, md3dDevice, mCommandList);
		Ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		Ritem->IndexCount = Ritem->Geo->DrawArgs["mesh"].IndexCount;
		Ritem->StartIndexLocation = Ritem->Geo->DrawArgs["mesh"].StartIndexLocation;
		Ritem->BaseVertexLocation = Ritem->Geo->DrawArgs["mesh"].BaseVertexLocation;
		Ritem->Bound = mMeshRender[i]->bound.aabb.ToBoundBox();
		Ritem->Enable = mMeshRender[i]->Enable;

		mRitemLayer[(int)RenderLayer::PostProcess].push_back(Ritem.get());
		mAllRitems.push_back(std::move(Ritem));
	}
}