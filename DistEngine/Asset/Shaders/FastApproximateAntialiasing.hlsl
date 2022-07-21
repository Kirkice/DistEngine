/* $Header: FxAA.hlsl                                                7/21/22 20:06p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : FxAA.hlsl                                                    *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/

#ifndef FXAA_INCLUDE
#define FXAA_INCLUDE

#include "Core.hlsl"

struct VertexIn
{ 
	float3 PosL    										: POSITION;
	float2 TexC    										: TEXCOORD;
};

struct VertexOut
{
	float4 PosH    										: SV_POSITION;
	float2 TexC    										: TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout                                      = (VertexOut)0.0f;
    vout.PosH 											= float4(vin.PosL, 1.0f);
	vout.TexC 											= vin.TexC;
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	PostprocessingData matData                          = gPostprocessingData[gMaterialIndex];
	float4 outColor 								    = gRenderTarget.Sample(gsamLinearClamp, pin.TexC); 
	return 												outColor;  
}

#endif