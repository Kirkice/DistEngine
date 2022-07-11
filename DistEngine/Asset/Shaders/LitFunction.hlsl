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

/// <summary>
/// LightingDotProducts
/// </summary>
struct LightingDotProducts
{
	float                                               NoL;
	float                                               NoV;
	float                                               NoH;
	float                                               LoH;
	float                                               VoH;
};

/// <summary>
/// BRDFParams
/// </summary>
struct BRDFParams
{
	float3                                              Albedo;
	float3                                              Specularity;
	float                                               Roughness;
	float                                               Metalness;
};

/// <summary>
/// LightingComponents
/// </summary>
struct LightingComponents
{
	float3                                              Diffuse;
	float3                                              Specular;
};

// CalculateLigthingDotProducts
LightingDotProducts CalculateLigthingDotProducts( float3 L, float3 N, float3 V )
{
	LightingDotProducts dots;

	float3 H                                            = normalize(V + L);
	dots.NoL                                            = saturate( dot(N, L) );
	dots.NoV                                            = clamp( dot(N, V), 1e-5, 1.0 );
	dots.NoH                                            = saturate( dot(N, H) );
	dots.LoH                                            = saturate( dot(L, H) );
	dots.VoH                                            = saturate( dot(V, H ) );

	return                                              dots;
}

// SurfaceProperties
struct SurfaceProperties
{
    float3 												N;
    float3 												V;
    float3 												c_diff;
    float3 												c_spec;
    float 												roughness;
    float 												alpha; // roughness squared
    float 												alphaSqr; // alpha squared
    float 												NdotV;
};

//FUNCTIONS
inline void InitializeStandardLitSurfaceData(float2 uv, float3 N, float3 T, out SurfaceData outSurfaceData)
{
    PBRMaterialData matData                             = gMaterialData[gMaterialIndex];
    //Albedo
    float4 diffuseAlbedo                                = matData.DiffuseColor;
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
    outSurfaceData.Normal                               = bumpedNormalW;
    outSurfaceData.Occlusion                            = Occlusion;

    outSurfaceData.Emission                             = emission;
}

//aces_approx
float3 aces_approx(float3 v)
{
    v 														*= 0.6f;
    float a 												= 2.51f;
    float b 												= 0.03f;
    float c 												= 2.43f;
    float d 												= 0.59f;
    float e 												= 0.14f;
    return 													clamp((v*(a*v+b))/(v*(c*v+d)+e), 0.0f, 1.0f);
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


//----------------------------  RED ENGINE PBR  ---------------------------------------

float Pow5( float x )
{
	float x2                                            = x * x;
	float x5                                            = x2 * x2 * x;
	return                                              x5;
}

// Schlick approximation to Fresnel
float3 SchlickFresnel(float3 f0, float f90, float u)
{
	return                                              f0 + ((float3)f90 - f0) * Pow5( 1-u );
}
float SchlickFresnel(float f0, float f90, float u)
{
	return                                              f0 + (f90 - f0) * Pow5( 1-u );
}

// Disney diffuse BRDF as in: http://blog.selfshadow.com/publications/s2012-shading-course/burley/s2012_pbs_disney_brdf_notes_v3.pdf
// with the conservation correction factor from: 
float3 RED_DiffuseBRDF(float NdotV, float NdotL, float LdotH, float3 albedo, float roughness)
{
	// Disney diffuse with energy correction factor from Frostbite
	float energyBias                                    = 0.5 * roughness;
	float energyFactor                                  = -0.337748344 * roughness + 1.0; // lerp( 1.0, 1.0 / 1.51, roughness );

	float fd90                                          = energyBias + 2.0 * LdotH * LdotH * roughness;
	float lightScatter                                  = SchlickFresnel(1.0f, fd90, NdotL);
	float viewScatter                                   = SchlickFresnel( 1.0f, fd90, NdotV);
	return                                              albedo * ( lightScatter * viewScatter * energyFactor * INV_PI );
}

float3 FuncF(float3 f0, float f90, float u)
{
	return                                              SchlickFresnel(f0, f90, u);
}

float FuncD(float NdotH, float roughness)
{
	float alpha                                         = roughness * roughness;
	float alpha2                                        = alpha * alpha;

	float NH                                            = saturate(NdotH);
	float NH2                                           = NH * NH;

	float b                                             = NH2 * (alpha2 - 1) + 1;
	return                                              alpha2 / (PI * b * b);
}

float FuncG(float roughness, float NoV, float NoL )
{
	float alphaG                                        = roughness * roughness;
	float alphaG2                                       = alphaG * alphaG;

	// Original formulation of G_SmithGGX Correlated
	float NdotV2                                        = NoV * NoV;
	float NdotL2                                        = NoL * NoL;
	float lambda_v                                      = sqrt( alphaG2 * (1 - NdotL2) / NdotL2 + 1 );
	float lambda_l                                      = sqrt( alphaG2 * (1 - NdotV2) / NdotV2 + 1 );
	return                                              (2 / (lambda_v + lambda_l));
}


float ComputeSpecularHorizonOcclusion( float NoL )
{
	// Horizon fading trick from http://marmosetco.tumblr.com/post/81245981087
	const float                                         horizonFade = 1.3;
	// horizon occlusion with falloff, should be computed for direct specular too
	float horizon                                       = min( 1.0 + horizonFade * NoL, 1.0 );
	return                                              horizon * horizon;
}

float3 RED_SpecularBRDF(float NdotH, float NdotV, float NdotL, float VdotH, float3 specularity, float roughness)
{
	float d, v;

	// straight-copy Disney model:
	// - alpha = roughness^2 parametrization 
	// - GGX for D
	// - height-correlated Smith for G
	// - Schlick approx for F

	d                                                   = FuncD(NdotH, roughness);
	v                                                   = FuncG(roughness, NdotV, NdotL);

	// Not multiplying it with NoL in here to avoid floating point precision issues at small grazing angles. 
	// It abbreviates with NoL multiplication from the rendering equation.
	v                                                   /= (4.0 * NdotV /* NdotL */);

	const float horizon                                 = ComputeSpecularHorizonOcclusion( NdotL );

	float3 spec                                         = FuncF(specularity, 1.0, VdotH) * min( d * v * horizon, 1e2f ); //clamp to prevent high specvular blobs at night
	return spec;
}

LightingComponents BRDF(float3 L, float3 N, float3 V, BRDFParams params, float NoL, float LoH )
{
	LightingDotProducts dots                            = CalculateLigthingDotProducts( L, N, V );

	LightingComponents ret;
	ret.Diffuse		                                    = RED_DiffuseBRDF( dots.NoV, NoL, LoH, params.Albedo, params.Roughness );
	ret.Specular	                                    = RED_SpecularBRDF( dots.NoH, dots.NoV, NoL, dots.VoH, params.Specularity, params.Roughness );

	return ret;
}

float3 CalculateLighting( float3 L, float atten, float3 N, float3 V, float3 lightColor, BRDFParams params )
{
	LightingComponents brdf                             = BRDF( L, N, V, params, saturate(dot(N, L)), saturate(dot(L, normalize(L + V))) );

	// no NdotL term for specular as it cancels out with the denominator in SpecularBRDF
	return                                              (brdf.Diffuse * saturate( dot( N, L ) ) * atten + brdf.Specular) * lightColor;
}

float3 CalculateLighting( float3 L, float atten, float3 N, float3 V, float3 lightColor, float3 albedo, float roughness, float specularity )
{
	// BRDF params initialization
	BRDFParams                                          params;
	params.Albedo                                       = albedo;
	params.Roughness                                    = roughness;
	params.Specularity                                  = specularity;

	return CalculateLighting( L, atten, N, V, lightColor, params );
}

float3 RED_SBS_CalculateLighting(float3 baseColor, float roughness, float metalness, float3 N, float3 V, float3 PosW)
{
    Light mainLight                                     = GetMainLight();
	float3 color                                        = CalculateLighting( mainLight.direction, mainLight.distanceAttenuation, N, V, mainLight.color, baseColor, roughness, metalness );

	Light pointLight 									= GetPointLight(PosW);
	color											   += CalculateLighting( pointLight.direction, pointLight.distanceAttenuation, N, V, pointLight.color, baseColor, roughness, metalness );
	return												  color;
}

float3 RED_SBS_GlobalIllumination(float3 albedo, float metallic, float smoothness, float occlusion, float3 normalWS, float3 viewDirectionWS, float3 emission = float3(0,0,0))
{
	float3 reflectVector 								= reflect(-viewDirectionWS, normalWS);
    float fresnelTerm 									= Pow4(1.0 - saturate(dot(normalWS, viewDirectionWS)));
	float3 IBLColor										= pow(gCubeIBL.Sample(gsamLinearWrap, normalWS).rgb,1);
    float3 bakedGI										= aces_approx(IBLColor);
	float3 indirectDiffuse 								= bakedGI * occlusion;

	float perceptualRoughness 							= PerceptualSmoothnessToPerceptualRoughness(smoothness);
    float3 indirectSpecular 							= GlossyEnvironmentReflection(reflectVector, perceptualRoughness, occlusion);

	float oneMinusReflectivity 							= OneMinusReflectivityMetallic(metallic);
	half3 c 											= indirectDiffuse * albedo * oneMinusReflectivity;

	float roughness 									= max(PerceptualRoughnessToRoughness(PerceptualSmoothnessToPerceptualRoughness(smoothness)), HALF_MIN);
    float roughness2 									= roughness * roughness;
    float surfaceReduction 								= 1.0 / (roughness2 + 1.0);
	float3 specular 									= lerp(kDieletricSpec.rgb, albedo, metallic);
	float grazingTerm 									= saturate(smoothness + 1.0 - oneMinusReflectivity);
    c 													+= surfaceReduction * indirectSpecular * lerp(lerp(kDieletricSpec.rgb, albedo, metallic), grazingTerm, fresnelTerm);
	c													+= emission;
	return c;
}

float3x3 BuildCubeFaceNormalRotation( int faceIdx ) // cp77 shouldn't be here (not related to lighting)
{
	if ( 0 == faceIdx )		                            return float3x3( float3( 0, 0, 1 ), float3( 0, -1, 0 ), float3( -1, 0, 0 ) );
	if ( 1 == faceIdx )		                            return float3x3( float3( 0, 0, -1 ), float3( 0, -1, 0 ), float3( 1, 0, 0 ) );
	if ( 2 == faceIdx )		                            return float3x3( float3( 1, 0, 0 ), float3( 0, 0, 1 ), float3( 0, 1, 0 ) );
	if ( 3 == faceIdx )		                            return float3x3( float3( 1, 0, 0 ), float3( 0, 0, -1 ), float3( 0, -1, 0 ) );
	if ( 4 == faceIdx )		                            return float3x3( float3( 1, 0, 0 ), float3( 0, -1, 0 ), float3( 0, 0, 1 ) );
							                            return float3x3( float3( -1, 0, 0 ), float3( 0, -1, 0 ), float3( 0, 0, -1 ) );
}

float3 IntegrateDiffuse(float CubemapLevel, float3 pointingNormal, int numSidePoints = 63 )
{
	float4 colAccum                                     = float4(0,0,0,0);

	for ( int face_idx = 0; face_idx < 6; ++face_idx )
	{
		float3x3 faceRotation                           = BuildCubeFaceNormalRotation( face_idx );			

		for ( int i=0; i<numSidePoints; i+=1 )
		for ( int j=0; j<numSidePoints; j+=1 )
		{
			float3 currNormalUnnorm                     = 2 * float3( (i+0.5)/numSidePoints - 0.5, (j+0.5)/numSidePoints - 0.5, 0.5 );
			float3 currNormal                           = mul( faceRotation, normalize( currNormalUnnorm ) );
			float currSolidAngle                        = 4.0 / (numSidePoints * numSidePoints * pow( dot(currNormalUnnorm, currNormalUnnorm), 1.5 ) );
			float weight                                = currSolidAngle * saturate( dot( pointingNormal, currNormal ) );

			float3 cubemapValue                         = gCubeMap.SampleLevel(gsamLinearWrap,currNormal, CubemapLevel).rgb;
			colAccum                                    += float4( weight * cubemapValue, weight );
		}
	}
	
	return                                              colAccum.xyz / max(0.001, colAccum.w );		// colAccum sums up to approx PI here
}

float3 CalculateReflectionDominantDirUnnorm( float3 N, float3 R, float roughness )
{
	return                                              lerp( R, N, 0.75 * roughness * roughness );
}

float3 CalculateReflectionDominantDirNorm( float3 N, float3 R, float roughness )
{
	return                                              normalize( CalculateReflectionDominantDirUnnorm( N, R, roughness ) );
}

float CalcImportanceSamplingMipLevel( float3 N, float3 H, float3 L, float paramRoughness, uint sampleCount, float width, float mipRange )
{
	float NdotH                                         = saturate ( dot (N, H) );
	float LdotH                                         = saturate ( dot (L, H) );
	float D_GGX_Divide_Pi                               = FuncD( NdotH, paramRoughness );
	float pdf                                           = D_GGX_Divide_Pi * NdotH /( 4* LdotH );
	float omegaS                                        = 1.0 / ( float(sampleCount) * pdf );
	float omegaP                                        = 4.0 * PI / (6.0 * width * width );
	float mipLevel                                      = clamp (0.5 * log2 ( omegaS / omegaP ), 0.0, mipRange );
	return                                              mipLevel;
}

// Hammersley - random 2d points distribution
float2 Hammersley(uint i, uint N) 
{
	float vradicalInverse_VdC                           = 0;
	{
		uint bits                                       = i;
		bits                                            = (bits << 16u) | (bits >> 16u);
		bits                                            = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
		bits                                            = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
		bits                                            = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
		bits                                            = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
		vradicalInverse_VdC                             = float(bits) * 2.3283064365386963e-10; // / 0x100000000
	}

	return                                              float2( float(i)/float(N), vradicalInverse_VdC );
}

// Calculates GGX microfacet normal.
// Xi - random 2d value
float3 ImportanceSampleGGX( float2 Xi, float Roughness )
{
	float a                                             = Roughness * Roughness;
	float Phi                                           = 2 * PI * Xi.x;
	float CosTheta                                      = sqrt( (1 - Xi.y) / (1 + (a*a - 1) * Xi.y) );
	float SinTheta                                      = sqrt( 1 - CosTheta * CosTheta );
	float3 H;
	H.x                                                 = SinTheta * cos( Phi );
	H.y                                                 = SinTheta * sin( Phi );
	H.z                                                 = CosTheta;

	return H;
}

// based on "Building an Orthonormal Basis, Revisited" (2017) paper
// by T. Duff, J. Burgess, P. Christensen, C. Hery, A. Kensler, M. Liani, and R. Villemin
void BuildOrthonormalBasis( in float3 n, out float3 u, out float3 v )
{
    const float s                                       = n.z < 0.0 ? -1.0 : 1.0;
    const float a                                       = -1.0f / (s + n.z);
    u                                                   = float3( 1.0f + s * n.x * n.x * a, s * n.x * n.y * a, -s * n.x);
    v                                                   = float3( n.x * n.y * a, s + n.y * n.y * a, -n.y);
}

float3 ImportanceSampleGGX( float2 Xi, float Roughness , float3 N )
{
	float3 tangentX, tangentY;
	BuildOrthonormalBasis( N, tangentX, tangentY );

	float3 H                                            = ImportanceSampleGGX( Xi, Roughness );

	// Tangent to world space
	return                                              tangentX * H.x + tangentY * H.y + N * H.z;
}

float3 SplitSum_PrefilterEnvMap(float Roughness , float3 N, float3 V, uint NumSamples, float cubeResolution, float cubeMipRange )
{
	float3 PrefilteredColor = float3(0,0,0);
	float TotalWeight = 0;
	for( uint i = 0u; i < NumSamples; i++ )
	{
		float2 Xi                                       = Hammersley( i, NumSamples );
		float3 H                                        = ImportanceSampleGGX( Xi, Roughness , N );
		float3 L                                        = 2.0 * dot( V, H ) * H - V;
		
		// small hack in order to not have missing reflection in very steep angles
		// checked to visual differece at a few roughness levels and it's insignificant
		// original line: float NoL = saturate( dot( N, L ) );				
		float NoL                                       = saturate( lerp( 0.075 * (1 - Roughness), 1, dot( N, L ) ) );
		
		if( NoL > 0.0 )		
		{
			float currMipLevel                          = CalcImportanceSamplingMipLevel( N, H, L, Roughness, NumSamples, cubeResolution, cubeMipRange );

			float3 cubeMapValue                         = gCubeMap.SampleLevel(gsamLinearWrap,L, currMipLevel).rgb;
			PrefilteredColor                            += cubeMapValue * NoL;
			TotalWeight                                 += NoL;
		}
	}
	return PrefilteredColor / TotalWeight;
}

void GetSplitSumNormalView( float NoV, out float3 outN, out float3 outV )
{
	float3 V;
	V.x                                                 = sqrt( 1.0f - NoV * NoV ); // sin
	V.y                                                 = 0;
	V.z                                                 = NoV; // cos

	outN                                                = float3(0,0,1);
	outV                                                = V;
}

/// GGX split sum approximation - calculate ambient BRDF
float2 SplitSum_IntegrateBRDF( float Roughness , float NoV, uint NumSamples = 1024u )
{
	float3 N, V;
	GetSplitSumNormalView( NoV, N, V );

	float A = 0.0;
	float B = 0.0;
	for( uint i = 0u; i < NumSamples; i++ )
	{

		float2 Xi                                       = Hammersley( i, NumSamples );

		float3 H                                        = ImportanceSampleGGX( Xi, Roughness , N );
		float3 L                                        = 2.0 * dot( V, H ) * H - V;
		float NoL                                       = saturate( dot( N, L ) );
		float NoH                                       = saturate( dot( N, H ) );
		float VoH                                       = saturate( dot( V, H ) );
		if( NoL > 0.0 )
		{
			float G                                     = FuncG( Roughness, NoV, NoL );
			G                                           /= (4.0 * NoV);
			float G_Vis                                 = G * (VoH / NoH);
			float Fc                                    = Pow5( 1.0 - VoH );
			A                                           += (1.0 - Fc) * G_Vis;
			B                                           += Fc * G_Vis;
		}
	}
	return float2( A, B ) * ( 4.0 / float(NumSamples) );
}

void ImportanceSampleCosDir( in float2 u, in float3 N, out float3 L, out float NdotL , out float pdf )
{
	float3                                              tangentX, tangentY;
	BuildOrthonormalBasis( N, tangentX, tangentY);

	float u1                                            = u.x;
	float u2                                            = u.y;

	float r                                             = sqrt(u1);
	float phi                                           = u2 * PI * 2;

	float3 local                                        = float3(r * cos( phi ), r * sin( phi ), sqrt(max(0.0f, 1.0f-u1)));
	L                                                   = tangentX * local.y + tangentY * local.x + N * local.z;

	NdotL                                               = local.z;
	pdf                                                 = NdotL * INV_PI ;
}

float IntegrateAmbientLightingDFG( in float3 V, in float3 N, in float roughness, uint sampleCount = 64 )
{
	float NdotV                                         = saturate ( dot (N, V) );
	float acc                                           = 0;
	float accWeight                                     = 0;

	for ( uint i=0u; i<sampleCount ; ++i )
	{

		float2 u                                        = frac( 0.5 + Hammersley( i, sampleCount ) );


		float3 L;
		float NdotL;
		float pdf;		
		ImportanceSampleCosDir (u, N, L, NdotL, pdf );
		if (NdotL > 0)
		{
			float LdotH                                 = saturate(dot(L, normalize(V + L)));
			float3 albedo                               = float3(1, 1, 1);
			acc                                         += RED_DiffuseBRDF(NdotV, NdotL, LdotH, albedo, roughness).x;
		}

		accWeight                                       += 1.0;
	}

	return                                              acc * (1.0 / accWeight );
}

float3 IntegrateFullAmbientBRDF( float NoV, float roughness, uint numAmbientSamples, uint numReflectionSamples )
{
	NoV			                                        = max( 0.001, NoV );			//< this is here to get rid of steep angle artifacts in substance
	roughness	                                        = max( 2.0/255.0, roughness );

	float2 reflectionScaleBias                          = SplitSum_IntegrateBRDF( roughness, NoV, numReflectionSamples );	
	
	float3 N, V;
	GetSplitSumNormalView( NoV, N, V );
	float ambientScale                                  = IntegrateAmbientLightingDFG( V, N, roughness, numAmbientSamples );

	return                                              float3( reflectionScaleBias.x, reflectionScaleBias.y, ambientScale );
}

float3 RED_SBS_ImageBasedLighting(float maxLod, float3 albedo, float3 N, float3 V, float roughness)
{
	float NoV                                           = saturate( dot( N, V ) );
	
	float3 lavaAmbient                                  = float3(0,0,0);

    int sampleCubeNumSamples                            = 8;
    int sampleCubeMipOffset                             = 4;
    float sampleCubeLevel                               = maxLod - sampleCubeMipOffset;		
    lavaAmbient                                         = IntegrateDiffuse(sampleCubeLevel, N, sampleCubeNumSamples );
	
	float3 lavaReflection                               = float3(0,0,0);
	{
		float3 reflV                                    = V;
		{
			// we want to perform some bending here, but since we don't have functions which would integrate reflection based on
			// R vector, then let's just manipulate the view vector
			// yyyyyyyyyyyyyyy check if this shit is correct enough
			
			float3 R                                    = reflect( -V, N );
			R                                           = CalculateReflectionDominantDirNorm( N, R, roughness );
			reflV                                       = reflect( -R, N );
		}
	
		float cubeResolution = pow( 2, maxLod - 1 ); // approx cube resolution (we're dealing with panorama here)
		float cubeMipRange = maxLod - 3;			
		uint sampleCubeNumSamples = 256u;
		lavaReflection = SplitSum_PrefilterEnvMap(roughness, N, reflV, sampleCubeNumSamples, cubeResolution, cubeMipRange );
	}

	float3 ambientBRDF = IntegrateFullAmbientBRDF( NoV, roughness, 128u, 128u );
	
	float3 outAmbient = albedo * lavaAmbient * ambientBRDF.z;
	float3 outReflection = (ambientBRDF.x + ambientBRDF.y) * lavaReflection;

    return outReflection * outAmbient;
}

//------------------------------- Dist ImageBasedLighting  ---------------------------------------------

// Shlick's approximation of Fresnel
float3 Fresnel_Shlick(float3 F0, float3 F90, float cosine)
{
    return 												lerp(F0, F90, Pow5(1.0 - cosine));
}

float Fresnel_Shlick(float F0, float F90, float cosine)
{
    return 												lerp(F0, F90, Pow5(1.0 - cosine));
}

// Diffuse irradiance
float3 Diffuse_IBL(SurfaceProperties Surface)
{
    // Assumption:  L = N

    //return Surface.c_diff * irradianceIBLTexture.Sample(defaultSampler, Surface.N);

    // This is nicer but more expensive, and specular can often drown out the diffuse anyway
    float LdotH 											= saturate(dot(Surface.N, normalize(Surface.N + Surface.V)));
    float fd90 												= 0.5 + 2.0 * Surface.roughness * LdotH * LdotH;
    float3 DiffuseBurley 									= Surface.c_diff * Fresnel_Shlick(1, fd90, Surface.NdotV);
	float3 IBLColor											= pow(gCubeIBL.Sample(gsamLinearWrap, Surface.N).rgb,1);
    return 													DiffuseBurley * aces_approx(IBLColor);
}

// Approximate specular IBL by sampling lower mips according to roughness.  Then modulate by Fresnel. 
float3 Specular_IBL(SurfaceProperties Surface)
{
	float perceptualRoughness 								= PerceptualSmoothnessToPerceptualRoughness(1 - Surface.roughness);
	float mip 												= PerceptualRoughnessToMipmapLevel(perceptualRoughness);
    float3 specular 										= Fresnel_Shlick(Surface.c_spec, 1, Surface.NdotV);
	float3 CubeMapColor										= pow(gCubeMap.SampleLevel(gsamLinearWrap, reflect(-Surface.V, Surface.N), mip).rgb,1);
    return 													specular * CubeMapColor;
}

float3 Dist_ImageBasedLighting(float3 baseColor,float smoothness, half metallic, float occlusion, float3 emissive,float3 N,float3 V)
{
	SurfaceProperties 										Surface;
    Surface.N 												= N;
    Surface.V 												= V;
    Surface.NdotV = saturate(dot(Surface.N, Surface.V));
    Surface.c_diff 											= baseColor.rgb * (1 - kDielectricSpecular) * (1 - metallic) * occlusion;
    Surface.c_spec 											= lerp(kDielectricSpecular, baseColor.rgb, metallic) * occlusion;
    Surface.roughness 										= 1 - smoothness;
    Surface.alpha 											= (1- smoothness) * (1- smoothness);
    Surface.alphaSqr 										= Surface.alpha * Surface.alpha;

    // Begin accumulating light starting with emissive
    float3 colorAccum 										= emissive;

	    // Add IBL
    colorAccum 												+= Diffuse_IBL(Surface);
    colorAccum 												+= Specular_IBL(Surface);

	return 													colorAccum;
}

// Burley's diffuse BRDF
float3 Diffuse_Burley(SurfaceProperties Surface, float LdotH, float NdotL)
{
    float fd90 = 0.5 + 2.0 * Surface.roughness * LdotH * LdotH;
    return Surface.c_diff * Fresnel_Shlick(1, fd90, NdotL).x * Fresnel_Shlick(1, fd90, Surface.NdotV).x;
}

// GGX specular D (normal distribution)
float Specular_D_GGX(SurfaceProperties Surface, float NdotH)
{
    float lower = lerp(1, Surface.alphaSqr, NdotH * NdotH);
    return Surface.alphaSqr / max(1e-6, PI * lower * lower);
}

// Schlick-Smith specular geometric visibility function
float G_Schlick_Smith(SurfaceProperties Surface, float NdotL)
{
    return 1.0 / max(1e-6, lerp(Surface.NdotV, 1, Surface.alpha * 0.5) * lerp(NdotL, 1, Surface.alpha * 0.5));
}

// Schlick-Smith specular visibility with Hable's LdotH approximation
float G_Shlick_Smith_Hable(SurfaceProperties Surface, float LdotH)
{
    return 1.0 / lerp(LdotH * LdotH, 1, Surface.alphaSqr * 0.25);
}

// A microfacet based BRDF.
// alpha:    This is roughness squared as in the Disney PBR model by Burley et al.
// c_spec:   The F0 reflectance value - 0.04 for non-metals, or RGB for metals.  This is the specular albedo.
// NdotV, NdotL, LdotH, NdotH:  vector dot products
//  N - surface normal
//  V - normalized view vector
//  L - normalized direction to light
//  H - normalized half vector (L+V)/2 -- halfway between L and V
float3 Specular_BRDF(SurfaceProperties Surface, float LdotH, float NdotH)
{
    // Normal Distribution term
    float ND = Specular_D_GGX(Surface, NdotH);

    // Geometric Visibility term
    //float GV = G_Schlick_Smith(Surface, Light);
    float GV = G_Shlick_Smith_Hable(Surface, LdotH);

    // Fresnel term
    float3 F = Fresnel_Shlick(Surface.c_spec, 1.0, LdotH);

    return ND * GV * F;
}

float3 ShadeDirectionalLight(SurfaceProperties Surface, float Shadow)
{
    Light mainLight                                     = GetMainLight();

    // Half vector
    float3 H = normalize(mainLight.direction + Surface.V);

    // Pre-compute dot products
    float NdotL = saturate(dot(Surface.N, mainLight.direction));
    float LdotH = saturate(dot(mainLight.direction, H));
    float NdotH = saturate(dot(Surface.N, H));

    // Diffuse & specular factors
    float3 diffuse = Diffuse_Burley(Surface, LdotH, NdotL);
    float3 specular = Specular_BRDF(Surface, LdotH, NdotH);

    // Directional light
    return NdotL * Shadow * mainLight.strength * (diffuse + specular);
}

float3 Dist_DirectionalLight(float3 baseColor,float smoothness, half metallic, float occlusion, float3 emissive,float3 N,float3 V, float Shadow)
{
	SurfaceProperties 										Surface;
    Surface.N 												= N;
    Surface.V 												= V;
    Surface.NdotV = saturate(dot(Surface.N, Surface.V));
    Surface.c_diff 											= baseColor.rgb * (1 - kDielectricSpecular) * (1 - metallic) * occlusion;
    Surface.c_spec 											= lerp(kDielectricSpecular, baseColor.rgb, metallic) * occlusion;
    Surface.roughness 										= 1 - smoothness;
    Surface.alpha 											= (1- smoothness) * (1- smoothness);
    Surface.alphaSqr 										= Surface.alpha * Surface.alpha;

    // Begin accumulating light starting with emissive
    float3 colorAccum 										= emissive;
	colorAccum 												= ShadeDirectionalLight(Surface, Shadow);
	return colorAccum;
}
//----------------------------  Shadow  ---------------------------------------
float3 DistShadow(InputData inputData, half3 outColor)
{
    half Shadow                                         	= inputData.ShadowCoord;
    return                                              	lerp(outColor * half3(0.2,0.2,0.2),outColor,Shadow);
}

float GetShadow(InputData inputData)
{
	half Shadow                                         	= inputData.ShadowCoord;
	return 													Shadow;
}

//----------------------------  Fog  ---------------------------------------

float GetFogRatioByDistance(float3 worldPos)
{
    float f 												= 1 - (gLinearFogParam.y - abs(worldPos.z - GetCameraPositionWS().z))/(gLinearFogParam.y - gLinearFogParam.x);
    return f;
}


float3 SetLinearFog(float3 PosW, float3 outColor)
{
    float fogDensity 										= GetFogRatioByDistance(PosW);
    fogDensity 												= saturate(fogDensity * gLinearFogParam.z);
	fogDensity												= (gFogColor.a > 0.5) ? fogDensity : 0;
	return 													lerp(outColor, gFogColor.rgb, fogDensity);
}

float3 SetHeightFog(float3 PosW, float3 outColor)
{
    PosW.y                                                  = saturate(remap(PosW.y, gHeightFogParam.z,gHeightFogParam.w, 0, 1 ));
    PosW.y                                                  = saturate(((PosW.y - (gHeightFogParam.y -gHeightFogParam.x)) * - 1.0 ) / (gHeightFogParam.y - (gHeightFogParam.y-gHeightFogParam.x)) + 1 );
    PosW.y													= (gLinearFogParam.w > 0.5) ? PosW.y : 0;
	return                                                 	lerp(outColor, gFogColor.rgb, PosW.y);
}

#endif