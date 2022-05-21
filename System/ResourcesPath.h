#pragma once
#include <string>
#include "d3dUtil.h"

	/// <summary>
	/// TEXTURES
	/// </summary>		


	//	-----------------ICON------------------
	const std::vector<std::string> mProjectIconTextures =			//	ICON������Դ
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

	const std::vector<std::string> mProjectIconName =				//	ICON��������
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
	const std::vector<std::string> mProjectGizmoTextures =					//	����ͼ����Դ
	{
		"Asset/Textures/Gizmo/AreaLightGizmo.png",
		"Asset/Textures/Gizmo/DirectionalLightGizmo.png",
		"Asset/Textures/Gizmo/ParticleSystemGizmo.png",
		"Asset/Textures/Gizmo/PointLightGizmo.png",
		"Asset/Textures/Gizmo/SpotLightGizmo.png",
	};

	const std::vector<std::string> mProjectGizmoName =				//	����ͼ������
	{
		"AreaLight",
		"DirectionalLight",
		"ParticleSystem",
		"PointLight",
		"SpotLight",
	};






	//	-----------------Project------------------
	const std::vector<std::string> mProjectResourceTextures =		//	��Դ����
	{
		"Asset/Textures/DefaultTextures/AK47Albedo.png",
		"Asset/Textures/DefaultTextures/AK47MSO.png",

		"Asset/Textures/DefaultTextures/AK47Normal.png",

		"Asset/Textures/DefaultTextures/sampleLUT.png",

		"Asset/Textures/DefaultTextures/black.png",
		"Asset/Textures/DefaultTextures/white.png"
	};




	const std::vector<std::string> mProjectResourceName =			//	��Դ��������
	{
		"AK47Albedo",
		"AK47MSO",

		"AK47Normal",

		"sampleLUT",

		"black",
		"white",
	};






	//	-----------------CubeMap------------------
	const std::vector<std::wstring> mProjectCubeMapTextures =					//	CubeMap��Դ
	{
		L"Asset/Textures/CubeMap/DGarden_diffuseIBL.dds",
		L"Asset/Textures/CubeMap/DGarden_specularIBL.dds",
	};

	const std::vector<std::string> mProjectCubeMapName =						//	CubeMap����
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
