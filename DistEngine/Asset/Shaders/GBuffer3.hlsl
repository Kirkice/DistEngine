/* $Header: GBuffer3.hlsl                                            6/26/21 20:55p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : GBuffer3.hlsl                                                *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/

#ifndef GBUFFER3_INCLUDE
#define GBUFFER3_INCLUDE

#include "GBufferFunction.hlsl"

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
    float3 PosW                                         : POSITION1;
    float3 View                                         : POSITION2;
};

void InitializeInputData(VertexOut input, half3 normalTS, out InputData inputData)
{
    inputData                                           = (InputData)0;
    half3 viewDirWS                                     = SafeNormalize(input.View);
    inputData.NormalW                                   = normalTS;
    inputData.ViewW                                     = viewDirWS;
    inputData.ShadowCoord                               = CalcShadowFactor(input.ShadowPosH);
    half3 reflectVector                                 = reflect(-input.View, input.NormalW);
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

	float4 texC                                         = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
	vout.TexC                                           = mul(texC, matData.MatTransform).xy;

    vout.ShadowPosH                                     = mul(float4(vout.PosW,1), gShadowTransform);

    vout.View                                           = normalize(GetCameraPositionWS() - vout.PosW);

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    SurfaceData surfaceData; 
    InitializeStandardLitSurfaceData(pin.TexC, pin.NormalW, pin.TangentW, surfaceData);
    InputData inputData;
    InitializeInputData(pin, surfaceData.Normal, inputData); 
    float3 outColor                                     = RED_SBS_GlobalIllumination(surfaceData.Albedo.rgb, surfaceData.Metallic, surfaceData.Smoothness, surfaceData.Occlusion, inputData.NormalW, inputData.ViewW);
    return                                              half4(outColor, ShadowFactory(inputData));
}

#endif