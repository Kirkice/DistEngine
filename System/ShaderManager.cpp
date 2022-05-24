#pragma once
#include "ShaderManager.h"

ShaderManager::ShaderManager()
{

}

ShaderManager::~ShaderManager()
{

}

void ShaderManager::Build()
{
	BuildShaders();
	BuildInputLayout();
}

/// <summary>
/// BuildShaders
/// </summary>
void ShaderManager::BuildShaders()
{
	const D3D_SHADER_MACRO alphaTestDefines[] =
	{
		"ALPHA_TEST", "1",
		NULL, NULL
	};

	const D3D_SHADER_MACRO skinnedDefines[] =
	{
		"SKINNED", "1",
		NULL, NULL
	};

	mShaders["standardVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Default.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["skinnedVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Default.hlsl", skinnedDefines, "VS", "vs_5_1");
	mShaders["opaquePS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Default.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["outlineVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\OutLine.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["outlinePS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\OutLine.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["litVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\LitPass.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["litPS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\LitPass.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["gizmoVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\GizmoPass.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["gizmoPS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\GizmoPass.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["unitVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Unlit.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["unitPS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Unlit.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["boundingVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Bounding.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["boundingPS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Bounding.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["shadowVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Shadows.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["skinnedShadowVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Shadows.hlsl", skinnedDefines, "VS", "vs_5_1");
	mShaders["shadowOpaquePS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Shadows.hlsl", nullptr, "PS", "ps_5_1");
	mShaders["shadowAlphaTestedPS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Shadows.hlsl", alphaTestDefines, "PS", "ps_5_1");

	mShaders["debugVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\ShadowDebug.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["debugPS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\ShadowDebug.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["drawNormalsVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\DrawNormals.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["skinnedDrawNormalsVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\DrawNormals.hlsl", skinnedDefines, "VS", "vs_5_1");
	mShaders["drawNormalsPS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\DrawNormals.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["ssaoVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Ssao.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["ssaoPS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Ssao.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["ssaoBlurVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\SsaoBlur.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["ssaoBlurPS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\SsaoBlur.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["skyVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Sky.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["skyPS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Sky.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["copyColorVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\CopyColor.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["copyColorPS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\CopyColor.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["finalBlitVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\FinalBlit.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["finalBlitPS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\FinalBlit.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["rgbSplitVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\RGBSplit.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["rgbSplitPS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\RGBSplit.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["radialBlurVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\RadialBlur.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["radialBlurPS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\RadialBlur.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["vignetteVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Vignette.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["vignettePS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Vignette.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["decolorVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Decolor.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["decolorPS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Decolor.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["decolorVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Decolor.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["decolorPS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Decolor.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["brightnessVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Brightness.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["brightnessPS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Brightness.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["hsvVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\HSV.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["hsvPS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\HSV.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["mosaicVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Mosaic.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["mosaicPS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Mosaic.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["sharpenVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Sharpen.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["sharpenPS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Sharpen.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["spherizeVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Spherize.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["spherizePS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Spherize.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["whiteBalanceVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\WhiteBalance.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["whiteBalancePS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\WhiteBalance.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["oilPaintVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\OilPaint.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["oilPaintPS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\OilPaint.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["reliefVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Relief.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["reliefPS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\Relief.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["edgeDetectionVS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\EdgeDetection.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["edgeDetectionPS"] = d3dUtil::CompileShader(L"Asset\\Shaders\\EdgeDetection.hlsl", nullptr, "PS", "ps_5_1");

	//mShaders["bloomVS"] = d3dUtil::CompileShader(L"Shaders\\Bloom.hlsl", nullptr, "VS", "vs_5_1");
	//mShaders["bloomPrefilterPS"] = d3dUtil::CompileShader(L"Shaders\\Bloom.hlsl", nullptr, "PS_Prefilter", "ps_5_1");
	//mShaders["bloomDownsample1PS"] = d3dUtil::CompileShader(L"Shaders\\Bloom.hlsl", nullptr, "PS_Downsample1", "ps_5_1");
	//mShaders["bloomDownsample2PS"] = d3dUtil::CompileShader(L"Shaders\\Bloom.hlsl", nullptr, "PS_Downsample2", "ps_5_1");
	//mShaders["bloomUpsamplePS"] = d3dUtil::CompileShader(L"Shaders\\Bloom.hlsl", nullptr, "PS_Upsample", "ps_5_1");
	//mShaders["bloomUpsample_FinalPS"] = d3dUtil::CompileShader(L"Shaders\\Bloom.hlsl", nullptr, "PS_Upsample_Final", "ps_5_1");
}

/// <summary>
/// BuildInputLayout
/// </summary>
void ShaderManager::BuildInputLayout()
{
	mInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	mSkinnedInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BONEINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, 56, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}

///// <summary>
///// GetShader
///// </summary>
///// <param name="str"></param>
///// <returns></returns>
//ComPtr<ID3DBlob> ShaderManager::GetShader(std::string str)
//{
//	return mShaders[str];
//}
//
///// <summary>
///// GetCommonLayout
///// </summary>
///// <returns></returns>
//std::vector<D3D12_INPUT_ELEMENT_DESC> ShaderManager::GetCommonLayout()
//{
//	return mInputLayout;
//}
//
///// <summary>
///// GetSkinnedLayout
///// </summary>
///// <returns></returns>
//std::vector<D3D12_INPUT_ELEMENT_DESC> ShaderManager::GetSkinnedLayout()
//{
//	return mSkinnedInputLayout;
//}