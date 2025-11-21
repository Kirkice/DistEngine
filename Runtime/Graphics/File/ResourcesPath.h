#pragma once
#include "DX12Utils.h"
#include <string>
#include <vector>

extern const std::string ApplicationPath;

/// <summary>
/// RESOURCES
/// </summary>

extern const std::vector<std::string> mMaterialPath;
extern const std::vector<std::string> mScenePath;

/// <summary>
/// TEXTURES
/// </summary>

//-----------------ICON------------------
extern const std::vector<std::string> mProjectIconTextures;//ICON
extern const std::vector<std::string> mProjectIconName;//ICON

//-----------------Gizmo------------------
extern const std::vector<std::string> mProjectGizmoTextures;//Gizmo
extern const std::vector<std::string> mProjectGizmoName;//Gizmo

//-----------------Project------------------
extern const std::vector<std::string> mProjectResourceTextures;//Resource
extern const std::vector<std::string> mProjectResourceName;//Resource

//-----------------CubeMap------------------
extern const std::vector<std::wstring> mProjectCubeMapTextures;//CubeMap
extern const std::vector<std::string> mProjectCubeMapName;//CubeMap

/// <summary>
/// MODEL
/// </summary>
extern const std::vector<std::string> mProjectModels;
extern const std::vector<std::string> mProjectName;

/// <summary>
/// MATERIAL
/// </summary>
extern const std::vector<std::string> mProjectMaterials;

/// <summary>
/// SCENE
/// </summary>
extern const std::vector<std::string> mProjectScenes;

/// <summary>
/// VS
/// </summary>
extern const std::vector<std::string> mBuiltinShaderVS;

/// <summary>
/// PS
/// </summary>
extern const std::vector<std::string> mBuiltinShaderPS;

/// <summary>
/// Path
/// </summary>
extern const std::vector<std::wstring> mBuiltinShaderPath;

//Mesh Path
extern const std::string mAk47ObjPath;
extern const std::string mBunnyObjPath;

extern const std::string mPositionUCSGizmoXPath;
extern const std::string mPositionUCSGizmoYPath;
extern const std::string mPositionUCSGizmoZPath;

extern const std::string mRotationUCSGizmoXPath;
extern const std::string mRotationUCSGizmoYPath;
extern const std::string mRotationUCSGizmoZPath;

extern const std::string mScaleUCSGizmoXPath;
extern const std::string mScaleUCSGizmoYPath;
extern const std::string mScaleUCSGizmoZPath;
