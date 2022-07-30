/* $Header: FxAA.hlsl                                                7/21/22 20:06p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : FxAA.hlsl                                                    *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/

#ifndef FXAA_INCLUDE
#define FXAA_INCLUDE

#include "Core.hlsl"

#define FXAA_SPAN_MAX           (8.0)
#define FXAA_REDUCE_MUL         (1.0 / 8.0)
#define FXAA_REDUCE_MIN         (1.0 / 128.0)

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

half3 Fetch(float2 coords, float2 offset)
{
    float2 uv 											= coords + offset;
    return 												gRenderTarget.Sample(gsamLinearClamp, uv).xyz;
}

half3 Load(int2 icoords, int idx, int idy)
{
    float2 uv 											= (icoords + int2(idx, idy));
    return 												gRenderTarget.Sample(gsamLinearClamp, uv).xyz;
}

half Luminance(half3 inputColor)
{
	return 												0.2125 * inputColor.r + 0.7154 * inputColor.g + 0.0721 * inputColor.b;
}

float3 Min3(float a, float b, float c)
{
	return min(min(a, b), c);
}

float3 Max3(float a, float b, float c)
{
	return max(max(a, b), c);
}

float4 PS(VertexOut pin) : SV_Target
{
	PostprocessingData matData                          = gPostprocessingData[gMaterialIndex];
	float4 outColor 								    = gRenderTarget.Sample(gsamLinearClamp, pin.TexC); 

    float2 positionSS 									= pin.TexC;

    half3 color 										= Load(positionSS, 0, 0).xyz;
    // Edge detection
    half3 rgbNW 										= Load(positionSS, -1, -1);
    half3 rgbNE 										= Load(positionSS,  1, -1);
    half3 rgbSW 										= Load(positionSS, -1,  1);
    half3 rgbSE 										= Load(positionSS,  1,  1);

    rgbNW 												= saturate(rgbNW);
    rgbNE 												= saturate(rgbNE);
    rgbSW 												= saturate(rgbSW);
    rgbSE 												= saturate(rgbSE);
    color 												= saturate(color);

    half lumaNW 										= Luminance(rgbNW);
    half lumaNE 										= Luminance(rgbNE);
    half lumaSW 										= Luminance(rgbSW);
    half lumaSE 										= Luminance(rgbSE);
    half lumaM 											= Luminance(color);

    float2 dir;
    dir.x 												= -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y 												= ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    half lumaSum 										= lumaNW + lumaNE + lumaSW + lumaSE;
    float dirReduce 									= max(lumaSum * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
    float rcpDirMin 									= rcp(min(abs(dir.x), abs(dir.y)) + dirReduce);

    dir 												= min((FXAA_SPAN_MAX).xx, max((-FXAA_SPAN_MAX).xx, dir * rcpDirMin));

    // Blur
    half3 rgb03 										= Fetch(positionSS, dir * (0.0 / 3.0 - 0.5));
    half3 rgb13 										= Fetch(positionSS, dir * (1.0 / 3.0 - 0.5));
    half3 rgb23 										= Fetch(positionSS, dir * (2.0 / 3.0 - 0.5));
    half3 rgb33 										= Fetch(positionSS, dir * (3.0 / 3.0 - 0.5));

    rgb03 												= saturate(rgb03);
    rgb13 												= saturate(rgb13);
    rgb23 												= saturate(rgb23);
    rgb33 												= saturate(rgb33);

    half3 rgbA 											= 0.5 * (rgb13 + rgb23);
    half3 rgbB 											= rgbA * 0.5 + 0.25 * (rgb03 + rgb33);

    half lumaB 											= Luminance(rgbB);

    half lumaMin 										= Min3(lumaM, lumaNW, Min3(lumaNE, lumaSW, lumaSE));
    half lumaMax 										= Max3(lumaM, lumaNW, Max3(lumaNE, lumaSW, lumaSE));

    color 												= ((lumaB < lumaMin) || (lumaB > lumaMax)) ? rgbA : rgbB;

	return 												float4(color,1);  
}

#endif