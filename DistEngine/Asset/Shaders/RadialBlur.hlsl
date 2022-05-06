/* $Header: RadialBlur.hlsl                                           6/26/21 20:55p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : RadialBlur.hlsl                                              *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#ifndef RADIAL_BLUR_INCLUDE
#define RADIAL_BLUR_INCLUDE

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


float2 GetMosaicUV( half2 uv)
{
	float pixelScale = 1.0 / 150;
	float2 uv0 = half2(pixelScale * 1 * floor(uv.x / (pixelScale *1)), (pixelScale * 1) * floor(uv.y / (pixelScale * 1)));
	return uv0;
}

float4 PS(VertexOut pin) : SV_Target
{
	PostprocessingData matData                          = gPostprocessingData[20];

	float2 dir 											= pin.TexC - float2(0.5,0.5);
	float4 outColor 									= float4(1,1,1,1);
	//采样SAMPLE_COUNT次
	for (int j = 0; j < 60; ++j)
	{
		float2 uv 										= pin.TexC + matData.BlurFactory * dir * j;
		outColor 										+= gRenderTarget.Sample(gsamLinearClamp, uv);
	}

	//取平均值
	outColor /= 60;
	return outColor;

	// float4 textureColor 								= gRenderTarget.Sample(gsamLinearClamp, GetMosaicUV(pin.TexC)); 
	// return textureColor;

	// float4 color 										= gCameraOpaqueTexture.Sample(gsamLinearClamp, pin.TexC); 
}

#endif

