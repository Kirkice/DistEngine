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

#define Scale 											0.97

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
	// float2 newTextureCoordinate	 						= float2((Scale - 1.0) * 0.5 + setUV.x / Scale,(Scale - 1.0) *0.5 + setUV.y /Scale);

	// float4 textureColor 								= gRenderTarget.Sample(gsamLinearClamp, setUV); 

	// float4 shiftColor1 									= gRenderTarget.Sample(gsamLinearClamp, newTextureCoordinate + float2(-0.05 * (Scale - 1.0), - 0.05 *(Scale - 1.0)));
	// float4 shiftColor2 									= gRenderTarget.Sample(gsamLinearClamp, newTextureCoordinate + float2(-0.1 * (Scale - 1.0), - 0.1 *(Scale - 1.0)));
	// float3 blendFirstColor 								= float3(textureColor.r , textureColor.g, shiftColor1.b);
	// float3 blend3DColor 								= float3(shiftColor2.r, blendFirstColor.g, blendFirstColor.b);
	// return 												float4(blend3DColor,textureColor.a); 


	// float2 dir 											= setUV - float2(0.5,0.5);
	// float4 outColor 									= float4(1,1,1,1);
	// //采样SAMPLE_COUNT次
	// for (int j = 0; j < 20; ++j)
	// {
	// 	float2 uv 										= setUV + 0.01 * dir * j;
	// 	outColor 										+= gRenderTarget.Sample(gsamLinearClamp, uv);
	// }
	// //取平均值
	// outColor /= 20;
	// return outColor;

	float4 textureColor 								= gRenderTarget.Sample(gsamLinearClamp, GetMosaicUV(pin.TexC)); 
	return textureColor;
}

#endif