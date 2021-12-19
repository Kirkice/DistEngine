//***************************************************************************************
// Default.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************

// Defaults for number of lights.
#ifndef NUM_DIR_LIGHTS
    #define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
    #define NUM_POINT_LIGHTS 0 
#endif

#ifndef NUM_SPOT_LIGHTS
    #define NUM_SPOT_LIGHTS 0
#endif

// Include common HLSL code.
#include "Core.hlsl"

struct VertexIn
{
	float3 PosL                                         : POSITION;
    float3 NormalL                                      : NORMAL;
	float2 TexC                                         : TEXCOORD;
	float3 TangentL                                     : TANGENT;
};

struct VertexOut
{
	float4 PosH                                         : SV_POSITION;
    float4 ShadowPosH                                   : POSITION0;
    float4 SsaoPosH                                     : POSITION1;
    float3 PosW                                         : POSITION2;
    float3 NormalW                                      : NORMAL;
	float3 TangentW                                     : TANGENT;
	float2 TexC                                         : TEXCOORD;
    float3 V                                            : POSITION3; 
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout                                      = (VertexOut)0.0f;

	PBRMaterialData matData                                = gMaterialData[gMaterialIndex];

    float4 posW                                         = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosW                                           = posW.xyz;

    vout.NormalW                                        = mul(vin.NormalL, (float3x3)gWorld);

	vout.TangentW                                       = mul(vin.TangentL, (float3x3)gWorld);

    vout.PosH                                           = mul(posW, gViewProj);

    vout.SsaoPosH                                       = mul(posW, gViewProjTex);

	float4 texC                                         = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
	vout.TexC                                           = mul(texC, matData.MatTransform).xy;

    vout.ShadowPosH                                     = mul(posW, gShadowTransform);

    vout.V                                              = normalize((float3)gEyePosW.xyz - posW.xyz);
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	PBRMaterialData matData                                = gMaterialData[gMaterialIndex];
	// float4 diffuseAlbedo                                = matData.DiffuseAlbedo;
	// float3 fresnelR0                                    = matData.FresnelR0;
	// float  roughness                                    = matData.Roughness;
    // float metallic                                      = matData.Metallic;

	// uint diffuseMapIndex                                = matData.DiffuseMapIndex;
	// uint normalMapIndex                                 = matData.NormalMapIndex;

    // float4 TexColor                                     = gTextureMaps[diffuseMapIndex].Sample(gsamAnisotropicWrap, pin.TexC);

    // pin.NormalW                                         = normalize(pin.NormalW);
	
    // float4 normalMapSample                              = gTextureMaps[normalMapIndex].Sample(gsamAnisotropicWrap, pin.TexC);
	// float3 bumpedNormalW                                = NormalSampleToWorldSpace(normalMapSample.rgb, pin.NormalW, pin.TangentW);

    // // Finish texture projection and sample SSAO map.
    // pin.SsaoPosH                                        /= pin.SsaoPosH.w;
    // float ambientAccess                                 = gSsaoMap.Sample(gsamLinearClamp, pin.SsaoPosH.xy, 0.0f).r;

    // // Light terms.
    // float4 ambient                                      = ambientAccess * diffuseAlbedo * TexColor;

    // Only the first light casts a shadow.
    float3 shadowFactor                                 = float3(1.0f, 1.0f, 1.0f);
    shadowFactor[0]                                     = CalcShadowFactor(pin.ShadowPosH);

    // const float shininess                               = (1.0f - roughness) * normalMapSample.a;
    // Material mat                                        = { diffuseAlbedo, fresnelR0, shininess };
    // float4 directLight                                  = GetDirectLighting(gLights, fresnelR0, metallic, roughness, diffuseAlbedo, pin.PosW, bumpedNormalW, pin.V, shadowFactor);
    // float4 litColor                                     = directLight;

	// // // Add in specular reflections.
    // float3 r                                            = reflect(-pin.V, bumpedNormalW);
    // float4 reflectionColor                              = gCubeMap.Sample(gsamLinearWrap, r);

    // float3 inDirectLight                                = GetInDirectLighting(pin.V, bumpedNormalW, fresnelR0, metallic, roughness, diffuseAlbedo, reflectionColor.rgb);
    // // float3 fresnelFactor = SchlickFresnel(fresnelR0, bumpedNormalW, r);
    // // litColor.rgb += shininess * fresnelFactor * reflectionColor.rgb;
	// litColor.rgb                                        += inDirectLight;
    // // Common convention to take alpha from diffuse albedo.
    // // litColor.rgb                                        *= shadowFactor[0].xxx;
    // litColor.a                                          = diffuseAlbedo.a;

    //-------------  FLOAT4 VECTOR --------------
    float4  mainColor = float4(1,1,1,1);

    //-------------  FLOAT4 VECTOR  -------------
    float3 N = normalize(pin.NormalW);
    float3 L = normalize(-gDirectionLights.Direction);
    float3 V = normalize(pin.V);

    float halfLambert = saturate(dot(N, L) * 0.5 + 0.5);

    // //Shadow
    // float Set_ShadowMask_1st = saturate(((halfLambert - (matData.shadowRange_1st - matData.shadowFeather_1st)) * - 1.0 ) / (matData.shadowRange_1st - (matData.shadowRange_1st-matData.shadowFeather_1st)) + 1);
    // mainColor = lerp(mainColor, mainColor * matData.shadowColor_1st, Set_ShadowMask_1st);

    // float Set_ShadowMask_2nd = saturate(((halfLambert - (matData.shadowRange_2nd - matData.shadowFeather_2nd)) * - 1.0 ) / (matData.shadowRange_2nd - (matData.shadowRange_2nd-matData.shadowFeather_2nd)) + 1);
    // mainColor = lerp(mainColor, mainColor * matData.shadowColor_2nd, Set_ShadowMask_2nd);

    // //RimLight
    // float NoV                                       = saturate(dot(N, V));
    // float rimFactor                                 = pow(1.0 - max(0,NoV),1);
    // float Set_RimLightMask                          = 1 - saturate(((rimFactor - (matData.rimRange - matData.rimFeather)) * - 1.0 ) / (matData.rimRange - (matData.rimRange-matData.rimFeather)) + 1);
    // float4 rimColor                                 = lerp(matData.rimColor, matData.rimColor * 0.9, saturate(1 - Set_ShadowMask_1st));
    // mainColor                                      += (rimColor * Set_RimLightMask);

    // //HighLight
    // float NoH                                       = saturate(pow(dot(N,normalize(L + V)),10));
    // float Set_HighLightMask                         = 1 - saturate(((NoH - (matData.specularRange - matData.specularFeather)) * - 1.0 ) / (matData.specularRange - (matData.specularRange-matData.specularFeather)) + 1);
    // float4 HighLight                                = matData.specularColor * Set_HighLightMask;
    // mainColor                                       += HighLight;

    // mainColor                                       = mainColor * matData.emissColor.r + (1 - matData.emissColor.r) * halfLambert;

    // mainColor                                       = lerp(mainColor * matData.LightShadowColor,mainColor, shadowFactor[0].x);
    return half4(gPointLights.Color,1);
}


