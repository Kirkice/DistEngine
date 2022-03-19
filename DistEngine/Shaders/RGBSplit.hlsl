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
	PostprocessingData matData                          = gPostprocessingData[19];
	float Scale											= 1 - matData.Strength;
	float2 newTextureCoordinate	 						= float2((Scale - 1.0) * 0.5 + pin.TexC.x / Scale,(Scale - 1.0) *0.5 + pin.TexC.y /Scale);

	float4 textureColor 								= gRenderTarget.Sample(gsamLinearClamp, newTextureCoordinate); 

	float4 shiftColor1 									= gRenderTarget.Sample(gsamLinearClamp, newTextureCoordinate + float2(-0.05 * (Scale - 1.0), - 0.05 *(Scale - 1.0)));
	float4 shiftColor2 									= gRenderTarget.Sample(gsamLinearClamp, newTextureCoordinate + float2(-0.1 * (Scale - 1.0), - 0.1 *(Scale - 1.0)));
	float3 blendFirstColor 								= float3(textureColor.r , textureColor.g, shiftColor1.b);
	float3 blend3DColor 								= float3(shiftColor2.r, blendFirstColor.g, blendFirstColor.b);
	return 												float4(blend3DColor,textureColor.a); 
}

#endif

