/* $Header: /Core/Common.hlsl                                        6/26/21 20:55p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : Common.hlsl                                                  *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/ 

#ifndef COMMON_INCLUDE
#define COMMON_INCLUDE

//Lights
#ifndef NUM_DIR_LIGHTS
#define NUM_DIR_LIGHTS 3
#endif 
 
#ifndef NUM_POINT_LIGHTS
#define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
#define NUM_SPOT_LIGHTS 0
#endif

//平行光
struct DirectionLight
{
    float3                                                                          Direction;
    float                                                                           Strength;
    float3                                                                          Color;
    float                                                                           CastShadow;
    float3                                                                          Position;
    float                                                                           Active;
};
 
//点光源
struct PointLight
{
    float3                                                                          Color;
    float                                                                           rangeFactory;
    float3                                                                          Position;
    float                                                                           Strength;
    float                                                                           Active;
};

//聚光灯
struct SpotLight
{
    float3                                                                          Color;
    float                                                                           rangeFactory;
    float3                                                                          Position;
    float                                                                           spotLightsStrength;
    float3                                                                          Direction;
    float                                                                           spotAngle;
    float                                                                           spotLightsActive;
};
 
//PBR Material
struct PBRMaterialData
{
    float4                                                                              BaseColor;
    float                                                                               Smoothness;
    float                                                                               Metallic;
    float                                                                               Occlusion;
    float4                                                                              EmissionColor;
    float                                                                               EmissionStrength;
    float                                                                               ReceiveShadow;
    float4x4                                                                            MatTransform;

    uint                                                                                DiffuseMapIndex;
    uint                                                                                NormalMapIndex;
    uint                                                                                MsoMapIndex;
    uint                                                                                EmissionMapIndex;
    uint                                                                                LUTMapIndex;
};

struct SkyBoxMaterialData
{
    float4                                                                              SkyBoxTint;
    float                                                                               SkyBoxExposure;
    float                                                                               SkyBoxRotation;
    float                                                                               ACES;
};

struct PostprocessingData
{
    //RGBSplit   Decolourize (Turn Gray)
    float                                                                               Strength;

    //Bloom
    float                                                                               Threshold;
    float                                                                               SoftKnee;
    float                                                                               Radius;
    float                                                                               Intensity;

    //Brightness And Contrast
    float                                                                               Brightness;
    float                                                                               Contrast;

    //HSV
    float                                                                               Hue;
    float                                                                               Saturation;
    float                                                                               Value;

    //Mosaic
    float                                                                               PixelSize;

    //Radial Blur
    float                                                                               BlurFactory;

    //EdgeDetection
    float                                                                               EdgeWdith;
    float                                                                               EdgeNeonFade;
    float4                                                                              OutLineColor;
    float                                                                               BackgroundFade;
    float4                                                                              BackgroundColor;

    //WhiteBalance
    float                                                                               Temperature;
    float                                                                               Tint;

    //Sharpen
    //Vignette
    float4                                                                              VignetteColor;

    //Oil Paint
    float                                                                               ResoultionValue;
};
 
StructuredBuffer<PBRMaterialData> gMaterialData                                         : register(t0, space1);

StructuredBuffer<SkyBoxMaterialData> gSkyMaterialData                                   : register(t0, space1);

StructuredBuffer<PostprocessingData> gPostprocessingData                                : register(t0, space1);
//TEXTURES
TextureCube gCubeIBL                                                                    : register(t0);
TextureCube gCubeMap                                                                    : register(t1);
Texture2D gRenderTarget                                                                 : register(t2);
Texture1D gCameraOpaqueTexture                                                          : register(t3);
Texture2D gShadowMap                                                                    : register(t4);
Texture2D gSsaoMap                                                                      : register(t5);
 
Texture2D gGizmoTextureMaps[12]                                                         : register(t6);
Texture2D gTextureMaps[48]                                                              : register(t6);


//SAMPLER
SamplerState gsamPointWrap                                                              : register(s0);
SamplerState gsamPointClamp                                                             : register(s1);
SamplerState gsamLinearWrap                                                             : register(s2);
SamplerState gsamLinearClamp                                                            : register(s3);
SamplerState gsamAnisotropicWrap                                                        : register(s4);
SamplerState gsamAnisotropicClamp                                                       : register(s5);
SamplerComparisonState gsamShadow                                                       : register(s6);

// CBuffer
cbuffer cbPerObject                                                                     : register(b0)
{
    float4x4                                                                            gWorld;
    float4x4                                                                            gInvWorld;
    float4x4                                                                            gTexTransform;
    uint                                                                                gMaterialIndex;
    uint                                                                                gObjPad0;
    uint                                                                                gObjPad1;
    uint                                                                                gObjPad2;
};

cbuffer cbSkinned : register(b1)
{
    float4x4                                                                            gBoneTransforms[96];
};

// Constant data that varies per material.
cbuffer cbPass : register(b2)
{
    float4x4                                                                            gView;
    float4x4                                                                            gInvView;
    float4x4                                                                            gProj;
    float4x4                                                                            gInvProj;
    float4x4                                                                            gViewProj;
    float4x4                                                                            gInvViewProj;
    float4x4                                                                            gViewProjTex;
    float4x4                                                                            gShadowTransform;
    float3                                                                              gEyePosW;
    float                                                                               cbPerObjectPad1;
    float2                                                                              gRenderTargetSize;
    float2                                                                              gInvRenderTargetSize;
    float                                                                               gNearZ;
    float                                                                               gFarZ;
    float                                                                               gTotalTime;
    float                                                                               gDeltaTime;

    DirectionLight                                                                      gDirectionLights;
    PointLight                                                                          gPointLights;
    SpotLight                                                                           gSpotLights;
};

//---------------------------------------------------------------------------------------
// Transforms a normal map sample to world space.
//---------------------------------------------------------------------------------------
float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 unitNormalW, float3 tangentW)
{
    // Uncompress each component from [0,1] to [-1,1].
    float3 normalT = 2.0f * normalMapSample - 1.0f;
    normalT = normalize(normalT * float3(0.7, 0.7, 1));
    // Build orthonormal basis.
    float3 N = unitNormalW;
    float3 T = normalize(tangentW - dot(tangentW, N) * N);
    float3 B = cross(N, T);

    float3x3 TBN = float3x3(T, B, N);

    // Transform from tangent space to world space.
    float3 bumpedNormalW = mul(normalT, TBN);

    return bumpedNormalW;
}

//---------------------------------------------------------------------------------------
// PCF for shadow mapping.
//---------------------------------------------------------------------------------------
//#define SMAP_SIZE = (2048.0f)
//#define SMAP_DX = (1.0f / SMAP_SIZE)
float CalcShadowFactor(float4 shadowPosH)
{
    // Complete projection by doing division by w.
    shadowPosH.xyz /= shadowPosH.w;

    // Depth in NDC space.
    float depth = shadowPosH.z;

    uint width, height, numMips;
    gShadowMap.GetDimensions(0, width, height, numMips);

    // Texel size.
    float dx = 1.0f / (float)width;

    float percentLit = 0.0f;
    const float2 offsets[9] =
    {
        float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
        float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
    };

    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        percentLit += gShadowMap.SampleCmpLevelZero(gsamShadow,
            shadowPosH.xy + offsets[i], depth).r;
    }

    return percentLit / 9.0f;
}

#endif