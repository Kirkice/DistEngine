/* $Header: /PBR/LitPass.hlsl                                        6/26/21 20:55p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : LitPass.hlsl                                                 *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#ifndef LITPASS_INCLUDE
#define LITPASS_INCLUDE

#include "LitFunction.hlsl"

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
    float3 NormalW                                      : NORMAL;
	float3 TangentW                                     : TANGENT;
	float2 TexC                                         : TEXCOORD;
    float4 ShadowPosH                                   : POSITION0;
    float4 SsaoPosH                                     : POSITION1;
    float3 PosW                                         : POSITION2;
    float3 V                                            : POSITION3; 
    float3 PosL                                         : POSITION4;
};

//----------------- UNITY LIT --------------------
void InitializeInputData(VertexOut input, half3 normalTS, out InputData inputData)
{
    inputData                                           = (InputData)0;
    half3 viewDirWS                                     = SafeNormalize(input.V);
    inputData.NormalW                                   = normalTS;
    inputData.ViewW                                     = viewDirWS;
    inputData.ShadowCoord                               = CalcShadowFactor(input.ShadowPosH);
    half3 reflectVector                                 = reflect(-input.V, input.NormalW);
    inputData.bakedGI                                   = gCubeIBL.Sample(gsamLinearWrap, reflectVector);
}

VertexOut VS(VertexIn vin)
{
	VertexOut vout                                      = (VertexOut)0.0f;

	PBRMaterialData matData                             = gMaterialData[gMaterialIndex];

    vout.PosW                                           = TransformObjectToWorld(vin.PosL);
    vout.NormalW                                        = TransformObjectToWorldNormal(vin.NormalL);
	vout.TangentW                                       = TransformObjectToWorld(vin.TangentL);
    vout.PosH                                           = TransformObjectToHClip(vin.PosL);
    vout.PosL                                           = vin.PosL;

    vout.SsaoPosH                                       = mul(float4(vout.PosW,1), gViewProjTex);

	vout.TexC                                           = vin.TexC;//mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
	// vout.TexC                                           = mul(texC, matData.MatTransform).xy;

    vout.ShadowPosH                                     = mul(float4(vout.PosW,1), gShadowTransform);

    vout.V                                              = normalize(GetCameraPositionWS() - vout.PosW);

    return vout;
}

float4 DistPBRColor(VertexOut pin)
{
    SurfaceData surfaceData;
    InitializeStandardLitSurfaceData(pin.TexC, pin.NormalW, pin.TangentW, surfaceData);
    InputData inputData;
    InitializeInputData(pin, surfaceData.Normal, inputData);

    float4 outColor                                     = float4(0,0,0,1);
    DistLightingPhysicallyBased(inputData, surfaceData.Albedo.rgb, surfaceData.Metallic, surfaceData.Smoothness, surfaceData.Occlusion, surfaceData.Emission.rgb, outColor.rgb);
    outColor.rgb                                       +=  DistGlobalIllumination(inputData, surfaceData.Albedo.rgb, surfaceData.Metallic, surfaceData.Smoothness, inputData.bakedGI.rgb);

    outColor.rgb                                        = DistShadow(inputData,outColor.rgb);

    // outColor.rgb                                       += surfaceData.Emission.rgb;
    outColor.rgb                                        = pow(outColor.rgb,(1/2.2));
    return outColor;

}

float4 UnityPBRColor(VertexOut pin)
{
    SurfaceData surfaceData;
    InitializeStandardLitSurfaceData(pin.TexC, pin.NormalW, pin.TangentW, surfaceData);
    InputData inputData;
    InitializeInputData(pin, surfaceData.Normal, inputData);

    float4 outColor = UniversalFragmentPBR(inputData, surfaceData.Albedo.rgb, surfaceData.Metallic, surfaceData.Smoothness, surfaceData.Occlusion, surfaceData.Emission.rgb);

        outColor.rgb                                        = DistShadow(inputData,outColor.rgb);
    return outColor;
}

float4 REDPBRColor(VertexOut pin)
{
    SurfaceData surfaceData;
    InitializeStandardLitSurfaceData(pin.TexC, pin.NormalW, pin.TangentW, surfaceData);
    InputData inputData;
    InitializeInputData(pin, surfaceData.Normal, inputData);

    float3 outColor                                     = RED_SBS_CalculateLighting(surfaceData.Albedo.rgb, (1 - surfaceData.Smoothness), surfaceData.Metallic, inputData.NormalW, inputData.ViewW, pin.PosW); 
    outColor                                            += RED_SBS_GlobalIllumination(surfaceData.Albedo.rgb, surfaceData.Metallic, surfaceData.Smoothness, surfaceData.Occlusion, inputData.NormalW, inputData.ViewW);
    outColor.rgb                                        = DistShadow(inputData,outColor.rgb);
    return float4(outColor,1);
}

float4 Dist_IBL(VertexOut pin)
{
    SurfaceData surfaceData;
    InitializeStandardLitSurfaceData(pin.TexC, pin.NormalW, pin.TangentW, surfaceData);
    InputData inputData;
    InitializeInputData(pin, surfaceData.Normal, inputData);

    float3 outColor                                     = Dist_ImageBasedLighting(surfaceData.Albedo.rgb,surfaceData.Smoothness, surfaceData.Metallic, surfaceData.Occlusion, surfaceData.Emission.rgb, inputData.NormalW,inputData.ViewW);
    outColor.rgb                                        = DistShadow(inputData,outColor.rgb);
    return                                              float4(outColor,surfaceData.Albedo.a);
}

float4 PS(VertexOut pin) : SV_Target
{
    return REDPBRColor(pin);
}

#endif