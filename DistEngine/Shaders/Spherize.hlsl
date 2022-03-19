/* $Header: Spherize.hlsl                                            3/19/22 20:55p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : Spherize.hlsl                                                *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#ifndef SPHERIZE_INCLUDE
#define SPHERIZE_INCLUDE

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
	PostprocessingData matData                          = gPostprocessingData[27];
	float2 centered_uv 									= pin.TexC * 2.0 - 1.0;
	float z 											= sqrt(1.0 - saturate(dot(centered_uv.xy * 0.73, centered_uv.xy * 0.73)));
	float2 spherified_uv 								= centered_uv / (z + 1.0);
	float2 uv 											= spherified_uv * 0.5 + 0.5;

	uv 													= lerp(pin.TexC, uv, matData.Spherify);
	float4 col 											= gRenderTarget.Sample(gsamLinearClamp, uv);
	float sqrDist 										= dot(centered_uv.xy, centered_uv.xy);
	float mask 											= 1.0 - sqrDist;
	mask 												= saturate(mask / fwidth(mask));
	col.a 												*= mask;
	return col;
}


#endif

