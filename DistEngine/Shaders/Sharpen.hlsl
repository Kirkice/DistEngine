/* $Header: Sharpen.hlsl                                            3/19/22 20:55p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : Sharpen.hlsl                                                 *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#ifndef SHARPEN_INCLUDE
#define SHARPEN_INCLUDE

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
	PostprocessingData matData                          = gPostprocessingData[26];

	float2 pixelSize 									= float2(0.00052083, 0.00092592);
	float2 halfPixelSize 								= pixelSize * 0.5;

	half4 blur 											=  gRenderTarget.Sample(gsamLinearClamp, pin.TexC + float2(halfPixelSize.x, -pixelSize.y));
	blur 												+= gRenderTarget.Sample(gsamLinearClamp, pin.TexC + float2(-pixelSize.x, -halfPixelSize.y));
	blur 												+= gRenderTarget.Sample(gsamLinearClamp, pin.TexC + float2(pixelSize.x, halfPixelSize.y));
	blur 												+= gRenderTarget.Sample(gsamLinearClamp, pin.TexC + float2(-halfPixelSize.x, pixelSize.y));
	blur 												*= 0.25;

	float4 sceneColor									= gRenderTarget.Sample(gsamLinearClamp, pin.TexC);
	float4 lumaStrength 								= float4(0.222, 0.707, 0.071, 0.0) * matData.SharpenStrength;
	float4 sharp 										= sceneColor - blur;

	sceneColor 											+= clamp(dot(sharp, lumaStrength), - matData.SharpenThreshold, matData.SharpenThreshold);
	return sceneColor;
}


#endif

