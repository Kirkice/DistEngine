#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

#include "DX12Utils.h"
#include "Mathf.h"
#include <string>
#include <unordered_map>

// PBR Material
class Material
{
public:
    // Name
    std::string Name;
    // Index Value
    int MatCBIndex = -1;
    // Dirty Frames
    int NumFramesDirty;
    
    // PBR Parameters
    Mathf::Color DiffuseColor;
    float Smoothness;
    float Metallic;
    float Occlusion;
    float EmissionStrength;
    Mathf::Color EmissionColor;
    
    // Normal Map
    float UseNormalMap;
    float NormalScale;
    
    DirectX::XMFLOAT4X4 MatTransform;

    // Texture Indices
    UINT DiffuseMapIndex;
    UINT NormalMapIndex;
    UINT MsoMapIndex;
    UINT EmissionMapIndex;
    UINT LUTMapIndex;

    // Post Process Parameters
    Mathf::Color Tint;
    float Exposure;
    float Rotation;
    float ACES;

    // RGBSplit / Decolor
    float RGBSplitStrength = 0.02f;
    float DecolorStrength = 0.5f;

    // Bloom
    float Threshold = 0.3f;
    float SoftKnee = 0.1f;
    float Radius = 3.5f;
    float Intensity = 0.5f;

    // Brightness And Contrast
    float Brightness = 1.2f;
    float Contrast = 1.0f;

    // HSV
    float Hue = 0;
    float Saturation = 0;
    float Value = 0;

    // Mosaic
    float PixelSize = 80.01f;

    // Radial Blur
    float BlurFactory = 0.01f;

    // EdgeDetection
    float EdgeWdith = 1.5f;
    float EdgeNeonFade = 1.0f;
    DirectX::XMFLOAT4 OutLineColor = { 0, 1.3f, 2.6f, 1 };
    float BackgroundFade = 0;
    DirectX::XMFLOAT4 BackgroundColor = { 0, 0.35f, 1, 1 };

    // WhiteBalance
    float Temperature = 0;
    float WhiteBalanceTint = 0;

    // Vignette
    DirectX::XMFLOAT4 VignetteColor = { 0, 0, 0, 1 };
    float Width = 1.4f;
    float Height = 0.7f;
    float Smooth = 10;
    float Alpha = 1;

    // Oil Paint
    float ResoultionValue = 2.0f;
    float OilPaintRadius = 3.0f;

    // Sharpen
    float SharpenStrength = 3.0f;
    float SharpenThreshold = 0.5f;

    // Spherize
    float Spherify = 0.5f;

    // Motion Blur
    float MotionBlurAmount = 0;

    // DOF
    float Aperture = 0;
    float FocalLength = 0;
    float Distance = 0;

    // FidelityFX Super Resolution
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
