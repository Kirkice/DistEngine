#pragma once
#include "d3dApp.h"
#include "../Core/Mathf/Mathf.h"

using namespace Mathf;


//PBR_Matreials
struct Material
{
	std::string Name;
	int MatCBIndex = -1;
	int NumFramesDirty = gNumFrameResources;

	Vector4 SkyBoxTint = { 1.0f, 1.0f, 1.0f, 1.0f };
	float SkyBoxExposure = 1;
	float SkyBoxRotation = 0;
	float ACES = 1;

	Vector4 BaseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	float Smoothness = 0.5f;
	float Metallic = 0.5;
	float Occlusion = 0.0f;
	Vector4 EmissionColor = { 0,0,0,1 };
	float EmissionStrength = 0.0f;
	float ReceiveShadow = 0;

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
	float Tint = 0;

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

	XMFLOAT4X4 MatTransform = Mathf::Identity4x4();

	int DiffuseSrvHeapIndex = -1;
	int NormalSrvHeapIndex = -1;
	int MsoSrvHeapIndex = -1;
	int EmissionSrvHeapIndex = -1;
	int LUTSrvHeapIndex = -1;
};