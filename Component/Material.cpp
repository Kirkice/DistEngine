#include "Material.h"
#include "d3dUtil.h"
#include "../Core/Mathf/Color.h"


/// <summary>
/// PBR Material
/// </summary>
PBRMaterial::PBRMaterial()
{
	Name = "New Material";
	MatCBIndex = -1;
	NumFramesDirty = gNumFrameResources;
	DiffuseColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
	Smoothness = 0.5f;
	Metallic = 0.0f;
	Occlusion = 1;
	EmissionStrength = 0;
	EmissionColor = Color(0.0f, 0.0f, 0.0f, 1.0f);

	MatTransform = Mathf::Identity4x4();

	DiffuseMapIndex = 0;
	NormalMapIndex = 0;
	MsoMapIndex = 0;
	EmissionMapIndex = 0;
	LUTMapIndex = 0;
}

PBRMaterial::~PBRMaterial()
{

}


/// <summary>
/// SkyBox Material
/// </summary>
SkyBoxMaterial::SkyBoxMaterial()
{
	Name = "New SkyBox Material";
	MatCBIndex = -1;
	NumFramesDirty = gNumFrameResources;
	Tint = Color(1.0f, 1.0f, 1.0f, 1.0f);
	Exposure = 1.0f;
	Rotation = 0.0f;
	ACES = 1.0f;
}

SkyBoxMaterial::~SkyBoxMaterial()
{

}