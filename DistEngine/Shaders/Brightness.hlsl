/* $Header: Brightness.hlsl                                          3/19/22 20:55p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : Brightness.hlsl                                              *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#ifndef BRIGHTNESS_INCLUDE
#define BRIGHTNESS_INCLUDE

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
	PostprocessingData matData                          = gPostprocessingData[23];
	float4 source										= gRenderTarget.Sample(gsamLinearClamp, pin.TexC);
	float4 finalColor 									= source * matData.Brightness.xxxx;
	float4 avgColor 									= float4(0.5, 0.5, 0.5, 1);
	finalColor 											= lerp(avgColor, finalColor, matData.Contrast);
	return finalColor;
}


#endif

