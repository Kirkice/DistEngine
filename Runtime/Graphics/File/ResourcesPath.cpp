#include "ResourcesPath.h"

const std::string ApplicationPath = "H:/Project/DistEngine/DistEngine/";

const std::vector<std::string> mMaterialPath =
{
	ApplicationPath + "Asset/Materials/ak47.mat",
};

const std::vector<std::string> mScenePath =
{
	ApplicationPath + "Asset/Scene/Default.scene",
};

const std::vector<std::string> mProjectIconTextures =
{
	"Asset/Textures/ICON/Folder.png",
	"Asset/Textures/ICON/Folder.png",
	"Asset/Textures/ICON/Folder.png",
	"Asset/Textures/ICON/Folder.png",
	"Asset/Textures/ICON/Folder.png",
	"Asset/Textures/ICON/Mat.png",
	"Asset/Textures/ICON/Mesh.png",
	"Asset/Textures/ICON/Scene.png",
	"Asset/Textures/ICON/Back.png",
	"Asset/Textures/ICON/TransformIcon.png",
	"Asset/Textures/ICON/RotationIcon.png",
	"Asset/Textures/ICON/ScaleIcon.png",
	"Asset/Textures/ICON/LightIcon.png",
	"Asset/Textures/ICON/WireIcon.png",
};

extern const std::vector<std::string> mProjectIconName =
{
	"Folder",
	"FolderPic",
	"FolderMat",
	"FolderMesh",
	"FolderScene",
	"Mat",
	"Mesh",
	"Scene",
	"Back",
	"TransformIcon",
	"RotationIcon",
	"ScaleIcon",
	"LightIcon",
	"WireIcon",
};

const std::vector<std::string> mProjectGizmoTextures =
{
	"Asset/Textures/Gizmo/AreaLightGizmo.png",
	"Asset/Textures/Gizmo/DirectionalLightGizmo.png",
	"Asset/Textures/Gizmo/ParticleSystemGizmo.png",
	"Asset/Textures/Gizmo/PointLightGizmo.png",
	"Asset/Textures/Gizmo/SpotLightGizmo.png",
};

const std::vector<std::string> mProjectGizmoName =
{
	"AreaLight",
	"DirectionalLight",
	"ParticleSystem",
	"PointLight",
	"SpotLight",
};

const std::vector<std::string> mProjectResourceTextures =
{
	"Asset/Textures/DefaultTextures/AK47Albedo.png",
	"Asset/Textures/DefaultTextures/AK47MSO.png",
	"Asset/Textures/DefaultTextures/AK47Normal.png",
	"Asset/Textures/DefaultTextures/sampleLUT.png",
	"Asset/Textures/DefaultTextures/black.png",
	"Asset/Textures/DefaultTextures/white.png",
};

const std::vector<std::string> mProjectResourceName =
{
	"AK47Albedo",
	"AK47MSO",
	"AK47Normal",
	"sampleLUT",
	"black",
	"white",
};

const std::vector<std::wstring> mProjectCubeMapTextures =
{
	L"Asset/Textures/CubeMap/Sky_diffuseIBL.dds",
	L"Asset/Textures/CubeMap/Sky_specularIBL.dds",
};

const std::vector<std::string> mProjectCubeMapName =
{
	"Sky_diffuseIBL",
	"Sky_specularIBL",
};

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

const std::vector<std::string> mProjectMaterials =
{
	"Asset/Materials/plane_mat.dmat",
	"Asset/Materials/wooden2.dmat",
};

const std::vector<std::string> mProjectScenes =
{
	"Asset/Scene/Default.scene",
};

const std::vector<std::string> mBuiltinShaderVS =
{
	"standardVS",
	"outlineVS",
	"litVS",
	"gbufferVS",
	"gizmoVS",
	"unlitVS",
	"lineVS",
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
	"volumeFogVS",
	"FastApproximateAntialiasingVS",
};

const std::vector<std::string> mBuiltinShaderPS =
{
	"opaquePS",
	"outlinePS",
	"litPS",
	"gbufferPS",
	"gizmoPS",
	"unlitPS",
	"linePS",
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
	"volumeFogPS",
	"FastApproximateAntialiasingPS",
};

const std::vector<std::wstring> mBuiltinShaderPath =
{
	L"Asset\\Shaders\\Default.hlsl",
	L"Asset\\Shaders\\OutLine.hlsl",
	L"Asset\\Shaders\\LitPass.hlsl",
	L"Asset\\Shaders\\GBuffer.hlsl",
	L"Asset\\Shaders\\GizmoPass.hlsl",
	L"Asset\\Shaders\\Unlit.hlsl",
	L"Asset\\Shaders\\Line.hlsl",
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
	L"Asset\\Shaders\\VolumeFog.hlsl",
	L"Asset\\Shaders\\FastApproximateAntialiasing.hlsl",
};

const std::string mAk47ObjPath = ApplicationPath + "Asset/Mesh/ak47.obj";
const std::string mBunnyObjPath = ApplicationPath + "Asset/Mesh/Bunny.obj";

const std::string mPositionUCSGizmoXPath = ApplicationPath + "Asset/Mesh/position_x.obj";
const std::string mPositionUCSGizmoYPath = ApplicationPath + "Asset/Mesh/position_y.obj";
const std::string mPositionUCSGizmoZPath = ApplicationPath + "Asset/Mesh/position_z.obj";

const std::string mRotationUCSGizmoXPath = ApplicationPath + "Asset/Mesh/rotation_x.obj";
const std::string mRotationUCSGizmoYPath = ApplicationPath + "Asset/Mesh/rotation_y.obj";
const std::string mRotationUCSGizmoZPath = ApplicationPath + "Asset/Mesh/rotation_z.obj";

const std::string mScaleUCSGizmoXPath = ApplicationPath + "Asset/Mesh/scale_x.obj";
const std::string mScaleUCSGizmoYPath = ApplicationPath + "Asset/Mesh/scale_y.obj";
const std::string mScaleUCSGizmoZPath = ApplicationPath + "Asset/Mesh/scale_z.obj";
