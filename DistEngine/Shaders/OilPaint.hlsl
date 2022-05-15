/* $Header: OilPaint.hlsl                                           6/26/21 20:55p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : OilPaint.hlsl                                                *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#ifndef OIL_PAINT_INCLUDE
#define OIL_PAINT_INCLUDE

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
	PostprocessingData matData                          = gPostprocessingData[29];
	
	float2 src_size 									= float2(matData.ResoultionValue / 1080, matData.ResoultionValue / 1920);
	float2 uv 											= pin.TexC;
	float n 											= (matData.OilPaintRadius + 1) * (matData.OilPaintRadius + 1);
	float3 m0 											= float3(0,0,0);
	float3 m1 											= float3(0,0,0);
	float3 s0 											= float3(0,0,0);
	float3 s1 											= float3(0,0,0);
	float3 color 										= float3(0,0,0);

	for (int j = -matData.OilPaintRadius; j <= 0; ++j) {
		for (int k = -matData.OilPaintRadius; k <= 0; ++k) {
			color = gRenderTarget.Sample(gsamLinearClamp, uv + float2(k, j) * src_size).rgb;
			m0 += color;
			s0 += color * color;
		}
	}

	for (int j = 0; j <= matData.OilPaintRadius; ++j) {
		for (int k = 0; k <= matData.OilPaintRadius; ++k) {
			color = gRenderTarget.Sample(gsamLinearClamp, uv + float2(k, j) * src_size).rgb;
			m1 += color;
			s1 += color * color;
		}
	}

	float4 finalColor 									= float4(0,0,0,0);
	float min_sigma2 									= 100;
	m0 													/= n;
	s0 													= abs(s0 / n - m0 * m0);
	float sigma2 										= s0.r + s0.g + s0.b;

	if (sigma2 < min_sigma2)
	{
		min_sigma2 										= sigma2;
		finalColor 										= float4(m0, 1.0);
	}
	m1 													/= n;
	s1 													= abs(s1 / n - m1 * m1);
	sigma2 												= s1.r + s1.g + s1.b;
	if (sigma2 < min_sigma2)
	{
		min_sigma2 										= sigma2;
		finalColor 										= float4(m1, 1.0);
	}

	return finalColor;
}

#endif

