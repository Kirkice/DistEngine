#pragma once
#include <string>
#include "../System/EngineSystem/SystemUtils.h"

namespace Dist
{
	/// <summary>
	/// TEXTURES
	/// </summary>		

	const std::vector<std::string> mProjectIconTextures =			//	ICON纹理
	{
		"Asset/Textures/ICON/Folder.png",
		"Asset/Textures/ICON/Mat.png",
		"Asset/Textures/ICON/Mesh.png",
		"Asset/Textures/ICON/Scene.png",
		"Asset/Textures/ICON/Back.png"
	};

	const std::vector<std::string> mProjectIconName =				//	ICON纹理名称
	{
		"Folder",
		"Mat",
		"Mesh",
		"Scene",
		"Back"
	};

	const std::vector<std::string> mProjectResourceTextures =		//	资源纹理
	{
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
	};

	const std::vector<std::string> mProjectResourceName =			//	资源纹理名称
	{
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

}