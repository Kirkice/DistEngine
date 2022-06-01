#pragma once
#include <string>
#include "d3dUtil.h"

	/// <summary>
	/// TEXTURES
	/// </summary>		


	//	-----------------ICON------------------
	const std::vector<std::string> mProjectIconTextures =			//	ICON纹理资源
	{
		"Asset/Textures/ICON/Folder.png",
		"Asset/Textures/ICON/Folder.png",
		"Asset/Textures/ICON/Folder.png",
		"Asset/Textures/ICON/Folder.png",
		"Asset/Textures/ICON/Folder.png",
		"Asset/Textures/ICON/Mat.png",
		"Asset/Textures/ICON/Mesh.png",
		"Asset/Textures/ICON/Scene.png",
		"Asset/Textures/ICON/Back.png"
	};

	const std::vector<std::string> mProjectIconName =				//	ICON纹理名称
	{
		"Folder",
		"FolderPic",
		"FolderMat",
		"FolderMesh",
		"FolderScene",
		"Mat",
		"Mesh",
		"Scene",
		"Back"
	};






	//	-----------------Gizmo------------------
	const std::vector<std::string> mProjectGizmoTextures =					//	引擎图标资源
	{
		"Asset/Textures/Gizmo/AreaLightGizmo.png",
		"Asset/Textures/Gizmo/DirectionalLightGizmo.png",
		"Asset/Textures/Gizmo/ParticleSystemGizmo.png",
		"Asset/Textures/Gizmo/PointLightGizmo.png",
		"Asset/Textures/Gizmo/SpotLightGizmo.png",
	};

	const std::vector<std::string> mProjectGizmoName =				//	引擎图标名称
	{
		"AreaLight",
		"DirectionalLight",
		"ParticleSystem",
		"PointLight",
		"SpotLight",
	};






	//	-----------------Project------------------
	const std::vector<std::string> mProjectResourceTextures =		//	资源纹理
	{
		"Asset/Textures/DefaultTextures/AK47Albedo.png",
		"Asset/Textures/DefaultTextures/AK47MSO.png",

		"Asset/Textures/DefaultTextures/AK47Normal.png",

		"Asset/Textures/DefaultTextures/sampleLUT.png",

		"Asset/Textures/DefaultTextures/black.png",
		"Asset/Textures/DefaultTextures/white.png"
	};




	const std::vector<std::string> mProjectResourceName =			//	资源纹理名称
	{
		"AK47Albedo",
		"AK47MSO",

		"AK47Normal",

		"sampleLUT",

		"black",
		"white",
	};






	//	-----------------CubeMap------------------
	const std::vector<std::wstring> mProjectCubeMapTextures =					//	CubeMap资源
	{
		L"Asset/Textures/CubeMap/DGarden_diffuseIBL.dds",
		L"Asset/Textures/CubeMap/DGarden_specularIBL.dds",
	};

	const std::vector<std::string> mProjectCubeMapName =						//	CubeMap名称
	{
		"DGarden_diffuseIBL",
		"DGarden_specularIBL",
	};









	/// <summary>
	/// MODEL
	/// </summary>	
	const std::vector<std::string> mProjectModels =
	{
		"Asset/Mesh/Bunny.FBX",
		"Asset/Mesh/Dragon.FBX",
		"Asset/Mesh/Sample.FBX",
		"Asset/Mesh/sponza.obj"
	};

	const std::vector<std::string> mProjectName =
	{
		"Bunny",
		"Dragon",
		"Sample",
		"sponza"
	};







	/// <summary>
	/// MATERIAL
	/// </summary>	
	const std::vector<std::string> mProjectMaterials =
	{
		"Asset/Materials/plane_mat.dmat",
		"Asset/Materials/wooden2.dmat",
	};






	/// <summary>
	/// SCENE
	/// </summary>	
	const std::vector<std::string> mProjectScenes =
	{
		"Asset/Scene/Default.scene",
	};


	/// <summary>
	/// VS
	/// </summary>
	const std::vector<std::string> mBuiltinShaderVS =
	{
		"standardVS",
		"outlineVS",
		"litVS",
		"gbuffer0VS",
		"gbuffer1VS",
		"gbuffer2VS",
		"gbuffer3VS",
		"gizmoVS",
		"unitVS",
		"boundingVS",
		"shadowVS",
		"debugVS",
		"drawNormalsVS",
		"ssaoVS",
		"ssaoBlurVS",
		"skyVS",
		"copyColorVS",
		"finalBlitVS",
		"rgbSplitVS",
		"radialBlurVS",
		"vignetteVS",
		"decolorVS",
		"brightnessVS",
		"hsvVS",
		"mosaicVS",
		"sharpenVS",
		"spherizeVS",
		"whiteBalanceVS",
		"oilPaintVS",
		"reliefVS",
		"edgeDetectionVS",
	};

	/// <summary>
	/// PS
	/// </summary>
	const std::vector<std::string> mBuiltinShaderPS =
	{
		"opaquePS",
		"outlinePS",
		"litPS",
		"gbuffer0PS",
		"gbuffer1PS",
		"gbuffer2PS",
		"gbuffer3PS",
		"gizmoPS",
		"unitPS",
		"boundingPS",
		"shadowOpaquePS",
		"debugPS",
		"drawNormalsPS",
		"ssaoPS",
		"ssaoBlurPS",
		"skyPS",
		"copyColorPS",
		"finalBlitPS",
		"rgbSplitPS",
		"radialBlurPS",
		"vignettePS",
		"decolorPS",
		"brightnessPS",
		"hsvPS",
		"mosaicPS",
		"sharpenPS",
		"spherizePS",
		"whiteBalancePS",
		"oilPaintPS",
		"reliefPS",
		"edgeDetectionPS",
	};

	/// <summary>
	/// Path
	/// </summary>
	const std::vector<std::wstring> mBuiltinShaderPath =
	{
		L"Asset\\Shaders\\Default.hlsl",
		L"Asset\\Shaders\\OutLine.hlsl",
		L"Asset\\Shaders\\LitPass.hlsl",
		L"Asset\\Shaders\\GBuffer0.hlsl",
		L"Asset\\Shaders\\GBuffer1.hlsl",
		L"Asset\\Shaders\\GBuffer2.hlsl",
		L"Asset\\Shaders\\GBuffer3.hlsl",
		L"Asset\\Shaders\\GizmoPass.hlsl",
		L"Asset\\Shaders\\Unlit.hlsl",
		L"Asset\\Shaders\\Bounding.hlsl",
		L"Asset\\Shaders\\Shadows.hlsl",
		L"Asset\\Shaders\\ShadowDebug.hlsl",
		L"Asset\\Shaders\\DrawNormals.hlsl",
		L"Asset\\Shaders\\Ssao.hlsl",
		L"Asset\\Shaders\\SsaoBlur.hlsl",
		L"Asset\\Shaders\\Sky.hlsl",
		L"Asset\\Shaders\\CopyColor.hlsl",
		L"Asset\\Shaders\\FinalBlit.hlsl",
		L"Asset\\Shaders\\RGBSplit.hlsl",
		L"Asset\\Shaders\\RadialBlur.hlsl",
		L"Asset\\Shaders\\Vignette.hlsl",
		L"Asset\\Shaders\\Decolor.hlsl",
		L"Asset\\Shaders\\Brightness.hlsl",
		L"Asset\\Shaders\\HSV.hlsl",
		L"Asset\\Shaders\\Mosaic.hlsl",
		L"Asset\\Shaders\\Sharpen.hlsl",
		L"Asset\\Shaders\\Spherize.hlsl",
		L"Asset\\Shaders\\WhiteBalance.hlsl",
		L"Asset\\Shaders\\OilPaint.hlsl",
		L"Asset\\Shaders\\Relief.hlsl",
		L"Asset\\Shaders\\EdgeDetection.hlsl",
	};