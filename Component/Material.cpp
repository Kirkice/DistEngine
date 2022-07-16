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

	//RGBSplit   Decolourize (Turn Gray)
	RGBSplitStrength = 0.02f;
	DecolorStrength = 0.5;

	//Bloom
	Threshold = 0.3f;
	SoftKnee = 0.1f;
	Radius = 3.5f;
	Intensity = 0.5f;

	//Brightness And Contrast
	Brightness = 1.2f;
	Contrast = 1.0f;

	//HSV
	Hue = 0;
	Saturation = 0;
	Value = 0;

	//Mosaic
	PixelSize = 80.01f;

	//Radial Blur
	BlurFactory = 0.01f;

	//EdgeDetection
	EdgeWdith = 1.5f;
	EdgeNeonFade = 1.0f;
	OutLineColor = { 0,1.3f,2.6f,1 };
	BackgroundFade = 0;
	BackgroundColor = { 0,0.35f,1,1 };

	//WhiteBalance
	Temperature = 0;
	WhiteBalanceTint = 0;

	//Sharpen
	//Vignette
	VignetteColor = { 0,0,0,1 };

	//Oil Paint
	ResoultionValue = 2.0f;

	//Vignette
	Width = 1.4f;
	Height = 0.7f;
	Smooth = 10;
	Alpha = 1;

	//Sharpen
	SharpenStrength = 3.0f;
	SharpenThreshold = 0.5f;

	//Spherize
	Spherify = 0.5f;

	OilPaintRadius = 3.0f;

	//Motion Blur
	MotionBlurAmount = 0;

	//DOF
	Aperture = 0;
	FocalLength = 0;
	Distance = 0;

	//FidelityFX Super Resolution
	ResoultionLevel = 0;
	FSRSharpen = 0;
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