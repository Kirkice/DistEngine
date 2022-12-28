/* $Header: EdgeDetection.hlsl                                       6/26/21 20:55p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : EdgeDetection.hlsl                                           *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#ifndef EDGE_DECTION_INCLUDE
#define EDGE_DECTION_INCLUDE

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

float3 sobel(float stepx, float stepy, float2 center)
{														
	float3 topLeft 										= gRenderTarget.Sample(gsamLinearClamp, center + float2(-stepx, stepy)).rgb;
	float3 bottomLeft 									= gRenderTarget.Sample(gsamLinearClamp, center + float2(-stepx, -stepy)).rgb;
	float3 topRight 									= gRenderTarget.Sample(gsamLinearClamp, center + float2(stepx, stepy)).rgb;
	float3 bottomRight 									= gRenderTarget.Sample(gsamLinearClamp, center + float2(stepx, -stepy)).rgb;
	float3 Gx 											= -1.0 * topLeft + 1.0 * bottomRight;
	float3 Gy 											= -1.0 * topRight + 1.0 * bottomLeft;
	float3 sobelGradient 								= sqrt((Gx * Gx) + (Gy * Gy));
	return sobelGradient;
}

float4 PS(VertexOut pin) : SV_Target
{
	PostprocessingData matData                          = gPostprocessingData[31];
	
	float4 sceneColor 									= gRenderTarget.Sample(gsamLinearClamp, pin.TexC);
	float3 sobelGradient 								= sobel(matData.EdgeWdith / 1920, matData.EdgeWdith / 1080, pin.TexC);
	float outLineArea 									= step(0.1,sobelGradient.r);
	float3 backgroundColor 								= lerp(matData.BackgroundColor.rgb * sceneColor.rgb, sceneColor.rgb, matData.BackgroundFade) * (1 - outLineArea);
	float3 edgeColor 									= lerp(sceneColor.rgb, sobelGradient.rgb * matData.OutLineColor, matData.EdgeNeonFade) * outLineArea;
	return 												float4(edgeColor + backgroundColor,1);
}

#endif

