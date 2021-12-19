/* $Header: /PBR/LitFunction.hlsl                                    6/26/21 20:55p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : LitFunction.hlsl                                             *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#ifndef LITFUNCTION_INCLUDE
#define LITFUNCTION_INCLUDE
#include "Core.hlsl"
#include "LightingUtil.hlsl"

#define kDieletricSpec                                  half4(0.04, 0.04, 0.04, 1.0 - 0.04) // standard dielectric reflectivity coef at incident angle (= 4%)

//Struct Data

/// <summary>
/// Surface Data
/// </summary>
struct SurfaceData
{
    float4                                              Albedo;
    float                                               Metallic;
    float                                               Smoothness;
    float3                                              Normal;
    float                                               Occlusion;
    float4                                              Emission;
};

/// <summary>
/// Input Data
/// </summary>
struct InputData 
{
    float3                                              PosW;
    float3                                              NormalW;
    float3                                              ViewW;
    float4                                              ShadowCoord;
    float3                                              bakedGI;
};

/// <summary>
/// BRDF Data
/// </summary>
struct BRDFData
{
    half3                                               diffuse;
    half3                                               specular;
    half                                                reflectivity;
    half                                                perceptualRoughness;
    half                                                roughness;
    half                                                roughness2;
    half                                                grazingTerm;

    // We save some light invariant BRDF terms so we don't have to recompute
    // them in the light loop. Take a look at DirectBRDF function for detailed explaination.
    half                                                normalizationTerm;     // roughness * 4.0 + 2.0
    half                                                roughness2MinusOne;    // roughness^2 - 1.0
};

//FUNCTIONS
inline void InitializeStandardLitSurfaceData(float2 uv, float3 N, float3 T, out SurfaceData outSurfaceData)
{
    PBRMaterialData matData                             = gMaterialData[gMaterialIndex];
    //Albedo
    float4 diffuseAlbedo                                = matData.BaseColor;
    uint diffuseMapIndex                                = matData.DiffuseMapIndex;
    diffuseAlbedo                                       *= gTextureMaps[diffuseMapIndex].Sample(gsamAnisotropicWrap, uv);

    //Normal
	uint normalMapIndex                                 = matData.NormalMapIndex;
    float4 normalMapSample                              = gTextureMaps[normalMapIndex].Sample(gsamAnisotropicWrap, uv);
	float3 bumpedNormalW                                = NormalSampleToWorldSpace(normalMapSample.rgb, normalize(N), T);

    //Occlusion
	uint msoMapIndex                                    = matData.MsoMapIndex;
	float4 mso                                          = gTextureMaps[msoMapIndex].Sample(gsamAnisotropicWrap, uv);
    float Metallic                                      = mso.r * matData.Metallic;
    float Smoothness                                    = mso.g * matData.Smoothness;
    float Occlusion                                     = mso.b * matData.Occlusion;

    //Emission
	uint emissionMapIndex                               = matData.EmissionMapIndex;
	float4 emission                                     = gTextureMaps[emissionMapIndex].Sample(gsamAnisotropicWrap, uv);
    emission                                            = matData.EmissionColor * float4(matData.EmissionStrength.xxxx) * emission;

    outSurfaceData.Albedo                               = diffuseAlbedo;
    outSurfaceData.Metallic                             = Metallic;
    outSurfaceData.Smoothness                           = Smoothness;
    outSurfaceData.Normal                               = N;
    outSurfaceData.Occlusion                            = Occlusion;

    outSurfaceData.Emission                             = emission;
}

//-------------------------------- UNITY UNIVERSAL LIT ----------------------------------------------
half OneMinusReflectivityMetallic(half metallic)
{
    // We'll need oneMinusReflectivity, so
    //   1-reflectivity = 1-lerp(dielectricSpec, 1, metallic) = lerp(1-dielectricSpec, 0, metallic)
    // store (1-dielectricSpec) in kDieletricSpec.a, then
    //   1-reflectivity = lerp(alpha, 0, metallic) = alpha + metallic*(0 - alpha) =
    //                  = alpha - metallic * alpha
    half oneMinusDielectricSpec                         = kDieletricSpec.a;
    return                                              oneMinusDielectricSpec - metallic * oneMinusDielectricSpec;
}

//粗糙度
real PerceptualRoughnessToRoughness(real perceptualRoughness)
{
    return perceptualRoughness * perceptualRoughness;
}

real PerceptualSmoothnessToPerceptualRoughness(real perceptualSmoothness)
{
    return                                              (1.0 - perceptualSmoothness);
}

//MIPMAP
real PerceptualRoughnessToMipmapLevel(real perceptualRoughness, uint mipMapCount)
{
    perceptualRoughness                                 = perceptualRoughness * (1.7 - 0.7 * perceptualRoughness);

    return                                              perceptualRoughness * mipMapCount;
}

real PerceptualRoughnessToMipmapLevel(real perceptualRoughness)
{
    return                                              PerceptualRoughnessToMipmapLevel(perceptualRoughness, 6);
}

//初始化BRDF
inline void InitializeBRDFData(half3 albedo, half metallic, half smoothness, out BRDFData outBRDFData)
{
    half oneMinusReflectivity                           = OneMinusReflectivityMetallic(metallic);
    half reflectivity                                   = 1.0 - oneMinusReflectivity;

    outBRDFData.diffuse                                 = albedo * oneMinusReflectivity;
    outBRDFData.specular                                = lerp(kDieletricSpec.rgb, albedo, metallic);
    outBRDFData.reflectivity                            = reflectivity;

    outBRDFData.grazingTerm                             = saturate(smoothness + reflectivity);
    outBRDFData.perceptualRoughness                     = PerceptualSmoothnessToPerceptualRoughness(smoothness);
    outBRDFData.roughness                               = max(PerceptualRoughnessToRoughness(outBRDFData.perceptualRoughness), HALF_MIN);
    outBRDFData.roughness2                              = outBRDFData.roughness * outBRDFData.roughness;
 
    outBRDFData.normalizationTerm                       = outBRDFData.roughness * 4.0h + 2.0h;
    outBRDFData.roughness2MinusOne                      = outBRDFData.roughness2 - 1.0h;
}

//间接光
real3 DecodeHDREnvironment(real4 encodedIrradiance, real4 decodeInstructions)
{
    // Take into account texture alpha if decodeInstructions.w is true(the alpha value affects the RGB channels)
    real alpha                                          = max(decodeInstructions.w * (encodedIrradiance.a - 1.0) + 1.0, 0.0);

    // If Linear mode is not supported we can skip exponent part
    return                                              (decodeInstructions.x * PositivePow(alpha, decodeInstructions.y)) * encodedIrradiance.rgb; 
}

half3 GlossyEnvironmentReflection(half3 reflectVector, half perceptualRoughness, half occlusion)
{
    half mip                                            = PerceptualRoughnessToMipmapLevel(perceptualRoughness);
    
    half4 encodedIrradiance                             = gCubeMap.SampleLevel(gsamLinearWrap,reflectVector, mip);
    return encodedIrradiance * occlusion;
}

half3 EnvironmentBRDF(BRDFData brdfData, half3 indirectDiffuse, half3 indirectSpecular, half fresnelTerm)
{
    half3 c                                             = indirectDiffuse * brdfData.diffuse;
    float surfaceReduction                              = 1.0 / (brdfData.roughness2 + 1.0);
    c                                                   += surfaceReduction * indirectSpecular * lerp(brdfData.specular, brdfData.grazingTerm, fresnelTerm);
    return                                              c;
}

half3 GlobalIllumination(BRDFData brdfData, half3 bakedGI, half occlusion, half3 normalWS, half3 viewDirectionWS)
{
    half3 reflectVector                                 = reflect(-viewDirectionWS, normalWS);
    half fresnelTerm                                    = Pow4(1.0 - saturate(dot(normalWS, viewDirectionWS)));

    half3 indirectDiffuse                               = bakedGI * occlusion;
    half3 indirectSpecular                              = GlossyEnvironmentReflection(reflectVector, brdfData.perceptualRoughness, occlusion);

    return                                              EnvironmentBRDF(brdfData, indirectDiffuse, indirectSpecular, fresnelTerm);
}

//PBR直接光

half3 LightingPhysicallyBased(BRDFData brdfData, BRDFData brdfDataClearCoat, half3 lightColor, half3 lightDirectionWS, half lightAttenuation, half3 normalWS, half3 viewDirectionWS, half clearCoatMask, bool specularHighlightsOff)
{
    half NdotL                                          = saturate(dot(normalWS, lightDirectionWS));
    half3 radiance                                      = lightColor * (lightAttenuation * NdotL);

    half3 brdf                                          = brdfData.diffuse;
    return                                              brdf * radiance;
}

half3 LightingPhysicallyBased(BRDFData brdfData, BRDFData brdfDataClearCoat, Light light, half3 normalWS, half3 viewDirectionWS, half clearCoatMask, bool specularHighlightsOff)
{
    return LightingPhysicallyBased(brdfData, brdfDataClearCoat, light.color, light.direction, light.distanceAttenuation * light.shadowAttenuation, normalWS, viewDirectionWS, clearCoatMask, specularHighlightsOff);
}

half3 LightingPhysicallyBased(BRDFData brdfData, Light light, half3 normalWS, half3 viewDirectionWS)
{
    const BRDFData noClearCoat                          = (BRDFData)0;
    return LightingPhysicallyBased(brdfData, noClearCoat, light, normalWS, viewDirectionWS, 0.0, false);
}

float4 UniversalFragmentPBR(InputData inputData, half3 albedo, half metallic, half smoothness, half occlusion, half3 emission)
{
    BRDFData                                            brdfData;
    InitializeBRDFData(albedo, metallic, smoothness, brdfData);

    Light mainLight                                     = GetMainLight();
    float3 color                                        = GlobalIllumination(brdfData, inputData.bakedGI, occlusion, inputData.NormalW, inputData.ViewW);
    color                                               += LightingPhysicallyBased(brdfData, mainLight, inputData.NormalW, inputData.ViewW);
    return float4(color,1);
}

//----------------------------  DIST ENGINE LIT  ---------------------------------------


float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a                                             = roughness * roughness;
    float a2                                            = a * a;
    float NdotH                                         = max(dot(N, H), 0.0);
    float NdotH2                                        = NdotH * NdotH;

    float nom                                           = a2;
    float denom                                         = (NdotH2 * (a2 - 1.0) + 1.0);
    denom                                               = PI * denom * denom;

    return                                              nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r                                             = (roughness + 1.0);
    float k                                             = (r * r) / 8.0;

    float nom                                           = NdotV;
    float denom                                         = NdotV * (1.0 - k) + k;

    return                                              nom / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV                                         = abs(dot(N, V));
    float NdotL                                         = abs(dot(N, L));
    float ggx2                                          = GeometrySchlickGGX(NdotV, roughness);
    float ggx1                                          = GeometrySchlickGGX(NdotL, roughness);

    return                                              ggx1 * ggx2;
}

float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return                                              F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 SpecularBRDF(float D, float G, float3 F, float3 V, float3 L, float3 N)
{        
    float                                               NdotL = abs(dot(N, L));
    float                                               NdotV = abs(dot(N, V));
            
    //specualr
    //Microfacet specular = D * G * F / (4 * NoL * NoV)
    float3                                              nominator = D * G * F;
    float                                               denominator = 4.0 * NdotV * NdotL + 0.001;
    float3                                              specularBrdf = nominator / denominator;

    return                                              specularBrdf;
}

float3 DiffuseBRDF(float3 albedo)
{
    return                                              albedo / PI;
}

float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    return                                              F0 + (max(float3(1 ,1, 1) * (1 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float ImportanceSampleGGX_PDF(float NDF, float NdotH, float VdotH)
{
      //ImportanceSampleGGX pdf
        //pdf = D * NoH / (4 * VoH)
    return                                              NDF * NdotH / (4 * VdotH);
   
}

//SampleHemisphere alpha = 1
float CosinSamplingPDF(float NdotL)
{
    return                                              NdotL / PI;
}

void DistLightingPhysicallyBased(InputData inputData, half3 albedo, half metallic, half smoothness, half occlusion, half3 emission, out half3 outColor)
{
    BRDFData                                            brdfData;

    InitializeBRDFData(albedo, metallic, smoothness, brdfData);

    Light mainLight                                     = GetMainLight();

    float3 L                                            = normalize(mainLight.direction);
    float3 H                                            = normalize(inputData.ViewW + L);
    float3 N                                            = normalize(inputData.NormalW);
    float3 V                                            = normalize(inputData.ViewW);
    float NdotL                                         = saturate(dot(N, L));
    float NdotH                                         = saturate(dot(N, H));
    float VdotH                                         = saturate(dot(V, H));
    float NdotV                                         = saturate(dot(N, V));

    float diffuseRatio                                  = 0.5 * (1.0 - metallic);
    float specularRoatio                                = 1 - diffuseRatio;

    float3 F0                                           = half3(0.08, 0.08, 0.08);
    F0                                                  = lerp(F0 * albedo, albedo, metallic);   
    float NDF                                           = DistributionGGX(N, H, (1 - smoothness));
    float G                                             = GeometrySmith(N, V, L, (1 - smoothness));
    float3 F                                            = FresnelSchlick(max(dot(H, V), 0.0), F0);

    float3 specularBrdf                                 = SpecularBRDF(NDF, G, F, V, L, N);
    float speccualrPdf                                  = ImportanceSampleGGX_PDF(NDF, NdotH, VdotH);

    float3 kS                                           = F;
    float3 kD                                           = (1.0 - kS);
    kD                                                  *= (1.0 - metallic);

    float3 diffuseBrdf                                  = DiffuseBRDF(albedo);
    float diffusePdf                                    = CosinSamplingPDF(NdotL);

    float3 totalBrdf                                    = (diffuseBrdf * kD + specularBrdf) * NdotL;
    float totalPdf                                      = diffuseRatio * diffusePdf + specularRoatio * speccualrPdf;

    float3 energy                                       = saturate(totalBrdf);

    outColor                                            = energy * mainLight.color;
}

float3 DistGlobalIllumination(InputData inputData, half3 albedo, half metallic, half smoothness, half3 bakeGI)
{
    float NdotV                                         = saturate(dot(inputData.NormalW, inputData.ViewW));

    float3 F0                                           = half3(0.08, 0.08, 0.08);
    F0                                                  = lerp(F0 * albedo, albedo, metallic);
    float roughness                                     = (1 - smoothness) * (1 - smoothness);
    float3 ambient                                      = 0.03 * bakeGI;
    float3 iblDiffuse                                   = max(half3(0, 0, 0), ambient.rgb);
    float3 Flast                                        = fresnelSchlickRoughness(max(NdotV, 0.0), F0, roughness);
    float kdLast                                        = (1 - Flast) * (1 - metallic);
    float3 iblDiffuseResult                             = iblDiffuse * kdLast * albedo;

    float mip_roughness                                 = (1 - smoothness) * (1.7 - 0.7 * (1 - smoothness));
    float3 reflectVec                                   = reflect(-inputData.ViewW, inputData.NormalW);
    half mip                                            = mip_roughness * 6;

    float4 rgbm                                         = gCubeMap.SampleLevel(gsamLinearWrap,reflectVec, mip);

    PBRMaterialData matData                             = gMaterialData[gMaterialIndex];
    uint lutIndex                                       = matData.LUTMapIndex;
    float2 envBDRF                                      = gTextureMaps[lutIndex].Sample(gsamAnisotropicWrap, float2(lerp(0, 0.99, NdotV.x), lerp(0, 0.99, roughness))).rg;
    float3 iblSpecularResult                            = rgbm.rgb * (Flast * envBDRF.r + envBDRF.g);
    float3 IndirectResult                               = iblDiffuseResult + iblSpecularResult;
    
    return                                              IndirectResult;
}

#endif