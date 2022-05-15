/* $Header: Relief.hlsl                                           	 6/26/21 20:55p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : Relief.hlsl                                                  *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#ifndef RELIEF_INCLUDE
#define RELIEF_INCLUDE

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
	PostprocessingData matData                          = gPostprocessingData[30];
	
	float3 m_n 											= gRenderTarget.Sample(gsamLinearClamp, pin.TexC).rgb;
	float3 m_l 											= gRenderTarget.Sample(gsamLinearClamp, pin.TexC + float2(0.00052083, 0.00092592)).rgb;
	float3 diff 										= abs(m_n - m_l);
	float max 											= diff.r > diff.g ? diff.r : diff.g;
	max 												= max > diff.b ? max : diff.b;
	float gray 											= clamp(max + 0.35, 0, 1);
	float4 c 											= float4(gray.xxx, 1);
	return c;
}

#endif

