/* $Header: Vignette.hlsl                                           6/26/21 20:55p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : Vignette.hlsl                                                *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#ifndef VIGNETTE_INCLUDE
#define VIGNETTE_INCLUDE

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

float GetOval(float2 uv, float width, float height)
{
	float2 pt 											= uv - float2(0.5,0.5);
	float h 											= (pt.x * pt.x) / (width * width) + (pt.y * pt.y) / (height * height);
	h 													= 1 - h;
	h													= pow(h,10);
	return h;
}

float4 PS(VertexOut pin) : SV_Target
{
	PostprocessingData matData                          = gPostprocessingData[21];

	float oval 											= GetOval(pin.TexC,matData.Width,matData.Height);
	oval 												*= matData.Smooth;
	oval												= saturate(oval);

	float4 source										= gRenderTarget.Sample(gsamLinearClamp, pin.TexC);
	float4 vignetteColor								= lerp(source, matData.VignetteColor, matData.Alpha);
	float4 outColor										= lerp(vignetteColor, source, oval);
	return outColor;
}


#endif

