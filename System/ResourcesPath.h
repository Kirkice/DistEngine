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
		"Asset/Textures/ICON/Mat.png",
		"Asset/Textures/ICON/Mesh.png",
		"Asset/Textures/ICON/Scene.png",
		"Asset/Textures/ICON/Back.png"
	};

	const std::vector<std::string> mProjectIconName =				//	ICON纹理名称
	{
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
		"Asset/Textures/DefaultTextures/BricsDiffuse.png",
		"Asset/Textures/DefaultTextures/BricsNormal.png",

		"Asset/Textures/DefaultTextures/OakDiffuse.png",
		"Asset/Textures/DefaultTextures/OakNormal.png",

		"Asset/Textures/DefaultTextures/PlaneDiffuse.png",
		"Asset/Textures/DefaultTextures/PlaneNormal.png",

		"Asset/Textures/DefaultTextures/WoodenDiffuse.png",
		"Asset/Textures/DefaultTextures/WoodenNormal.png",

		"Asset/Textures/DefaultTextures/WoodenDiffuse2.png",
		"Asset/Textures/DefaultTextures/WoodenNormal2.png",

		"Asset/Textures/DefaultTextures/WoodenDiffuse3.png",
		"Asset/Textures/DefaultTextures/WoodenNormal3.png",

		"Asset/Textures/DefaultTextures/WoodenDiffuse4.png",
		"Asset/Textures/DefaultTextures/WoodenNormal4.png",

		"Asset/Textures/DefaultTextures/sampleLUT.png"

		//"Asset/Textures/DefaultTextures/black.png",
		//"Asset/Textures/DefaultTextures/mso.png",
		//"Asset/Textures/DefaultTextures/norm.png",
		//"Asset/Textures/DefaultTextures/white.png",
	};

	const std::vector<std::string> mProjectResourceName =			//	资源纹理名称
	{
		"BricsDiffuse",
		"BricsNormal",

		"OakDiffuse",
		"OakNormal",

		"PlaneDiffuse",
		"PlaneNormal",

		"WoodenDiffuse",
		"WoodenNormal",

		"WoodenDiffuse2",
		"WoodenNormal2",

		"WoodenDiffuse3",
		"WoodenNormal3",

		"WoodenDiffuse4",
		"WoodenNormal4",

		"sampleLUT"

		//"black",
		//"mso",
		//"norm",
		//"white",
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
