#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

#include "Material.h"
#include "DX12Utils.h"
#include "Mathf.h"
#include <string>
#include <unordered_map>

using namespace Mathf;
//	PBR材质
class Material
{
public:
	//	名字
	std::string Name;
	//	Index 值
	int MatCBIndex = -1;
	//	材质变化值
	int NumFramesDirty;
	//	固有色
	Color DiffuseColor;
	//	粗糙度
	float Smoothness;
	//	金属度
	float Metallic;
	//	AO
	float Occlusion;
	//	自发光强度
	float EmissionStrength;
	//	自发光颜色
	Color EmissionColor;

	// 纹理映射矩阵
	DirectX::XMFLOAT4X4 MatTransform;

	//	Diffuse贴图Index
	UINT DiffuseMapIndex;
	//	法线贴图Index
	UINT NormalMapIndex;
	//	MSO贴图Index
	UINT MsoMapIndex;
	//	自发光贴图Index
	UINT EmissionMapIndex;
	//	LUT贴图Index
	UINT LUTMapIndex;  



	//	固有色
	Color Tint;
	//	曝光度
	float Exposure;
	//	旋转
	float Rotation;
	//	ACES开启
	float ACES;

	//RGBSplit   Decolourize (Turn Gray)
	float RGBSplitStrength = 0.02f;
	float DecolorStrength = 0.5;

	//Bloom
	float Threshold = 0.3f;
	float SoftKnee = 0.1f;
	float Radius = 3.5f;
	float Intensity = 0.5f;

	//Brightness And Contrast
	float Brightness = 1.2f;
	float Contrast = 1.0f;

	//HSV
	float Hue = 0;
	float Saturation = 0;
	float Value = 0;

	//Mosaic
	float PixelSize = 80.01f;

	//Radial Blur
	float BlurFactory = 0.01f;

	//EdgeDetection
	float EdgeWdith = 1.5f;
	float EdgeNeonFade = 1.0f;
	DirectX::XMFLOAT4 OutLineColor = { 0,1.3f,2.6f,1 };
	float BackgroundFade = 0;
	DirectX::XMFLOAT4 BackgroundColor = { 0,0.35f,1,1 };

	//WhiteBalance
	float Temperature = 0;
	float WhiteBalanceTint = 0;

	//Sharpen
	//Vignette
	DirectX::XMFLOAT4 VignetteColor = { 0,0,0,1 };

	//Oil Paint
	float ResoultionValue = 2.0f;

	//Vignette
	float Width = 1.4f;
	float Height = 0.7f;
	float Smooth = 10;
	float Alpha = 1;

	//Sharpen
	float SharpenStrength = 3.0f;
	float SharpenThreshold = 0.5f;

	//Spherize
	float Spherify = 0.5f;

	float OilPaintRadius = 3.0f;

	//Motion Blur
	float MotionBlurAmount = 0;

	//DOF
	float Aperture = 0;
	float FocalLength = 0;
	float Distance = 0;

	//FidelityFX Super Resolution
	float ResoultionLevel = 0;
	float FSRSharpen = 0;


public:
	Material();
	~Material();
};


class MaterialIndexUtils
{
public:

	MaterialIndexUtils() = default;
	~MaterialIndexUtils() = default;

	static MaterialIndexUtils& getInstance() {
		static MaterialIndexUtils instance;
		return instance;
	}

public:

	void Init();

	int GetIndex();
	void OffsetIndex();

	void SaveTypeIndex(std::string type, int Index);

	int GetTypeIndexStart(std::string type);

private:
	int Index = -1;

	std::unordered_map<std::string, int> mMatTypeIndexMap;
};
#endif