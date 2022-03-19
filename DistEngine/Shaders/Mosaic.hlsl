/* $Header: Mosaic.hlsl                                            3/19/22 20:55p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : Mosaic.hlsl                                                  *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#ifndef MOSAIC_INCLUDE
#define MOSAIC_INCLUDE

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

float2 GetMosaicUV( half2 uv, float size)
{
	float pixelScale 									= 1.0 / size;
	float2 uv0 											= half2(pixelScale * 0.6 * floor(uv.x / (pixelScale * 0.6)), (pixelScale) * floor(uv.y / (pixelScale)));
	return uv0;
}

float4 PS(VertexOut pin) : SV_Target
{
	PostprocessingData matData                          = gPostprocessingData[25];
	float4 textureColor 								= gRenderTarget.Sample(gsamLinearClamp, GetMosaicUV(pin.TexC,matData.PixelSize)); 
	return textureColor;
}


#endif

