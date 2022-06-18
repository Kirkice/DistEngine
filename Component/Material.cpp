#include "Material.h"
#include "d3dUtil.h"
#include "../Core/Mathf/Color.h"


/// <summary>
/// PBR Material
/// </summary>
Material::Material()
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

	Tint = Color(1.0f, 1.0f, 1.0f, 1.0f);
	Exposure = 1.0f;
	Rotation = 0.0f;
	ACES = 1.0f;
}
 
Material::~Material()
{

}

void MaterialIndexUtils::Init()
{
	Index = 0;
}

int MaterialIndexUtils::GetIndex()
{
	return Index;
}

void MaterialIndexUtils::OffsetIndex()
{
	Index++;
}

void MaterialIndexUtils::SaveTypeIndex(std::string type, int Index)
{
	mMatTypeIndexMap[type] = Index;
}

int MaterialIndexUtils::GetTypeIndexStart(std::string type)
{
	return mMatTypeIndexMap[type];
}