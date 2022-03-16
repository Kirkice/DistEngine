/* $Header: FinalBlit.hlsl                                           6/26/21 20:55p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : FinalBlit.hlsl                                               *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#ifndef FINALBLIT_INCLUDE
#define FINALBLIT_INCLUDE

#include "Core.hlsl"

struct VertexIn
{ 
	float3 PosL    											: POSITION;
	float2 TexC    											: TEXCOORD;
};

struct VertexOut
{
	float4 PosH    											: SV_POSITION;
	float2 TexC    											: TEXCOORD0;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout                                      	= (VertexOut)0.0f;
    vout.PosH 												= float4(vin.PosL, 1.0f);
	vout.TexC 												= vin.TexC;
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float4 color 											= gRenderTarget.Sample(gsamLinearClamp, pin.TexC) * float4(0,1,1,1); 
	return color;
}

#endif 

