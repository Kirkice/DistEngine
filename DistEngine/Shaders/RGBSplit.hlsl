/* $Header: RGBSplit.hlsl                                           6/26/21 20:55p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : RGBSplit.hlsl                                                *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#ifndef RGBSPLIT_INCLUDE
#define RGBSPLIT_INCLUDE

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
    float3 NormalW                                      : NORMAL;
	float3 TangentW                                     : TANGENT;
	float2 TexC                                         : TEXCOORD0;
    float4 PosS                                         : TEXCOORD1;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout                                      = (VertexOut)0.0f;
    vout.PosH                                           = TransformObjectToHClip(vin.PosL);
    vout.PosS                                           = mul(float4(TransformObjectToWorld(vin.PosL),1), gViewProjTex);
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
return gRenderTarget.Sample(gsamLinearClamp, float2(pin.PosS.xy / pin.PosS.w)); 
}

#endif