/* $Header: Bloom.hlsl                                            3/19/22 20:55p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : Bloom.hlsl                                                   *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#ifndef BLOOM_INCLUDE
#define BLOOM_INCLUDE

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



// Brightness function
float Brightness(float3 c)
{
    return 												max(max(c.r, c.g), c.b);
}

// 3-tap median filter
float3 Median(float3 a, float3 b, float3 c)
{
    return 												a + b + c - min(min(a, b), c) - max(max(a, b), c);
}


// RGBM encoding/decoding
half4 EncodeHDR(float3 rgb)
{
    rgb 												*= 0.125;
    float m 											= max(max(rgb.r, rgb.g), max(rgb.b, 1e-6));
    m 													= ceil(m * 255) / 255;
    return 												half4(rgb / m, m);
}

float3 DecodeHDR(half4 rgba)
{
    return 												rgba.rgb * rgba.a * 8;
}


// Downsample with a 4x4 box filter
half3 DownsampleFilter(float2 uv)
{
    float4 d = 											float4(-1, -1, +1, +1);

    half3 s;
    s 													 = DecodeHDR(gRenderTarget.Sample(gsamLinearClamp, uv + d.xy));
    s 													+= DecodeHDR(gRenderTarget.Sample(gsamLinearClamp, uv + d.zy));
    s 													+= DecodeHDR(gRenderTarget.Sample(gsamLinearClamp, uv + d.xw));
    s 													+= DecodeHDR(gRenderTarget.Sample(gsamLinearClamp, uv + d.zw));

    return s * (1.0 / 4);
}

half3 UpsampleFilter(float2 uv, float radius)
{
    // 9-tap bilinear upsampler (tent filter)
	float logh 											= log2(1080) + radius - 8;
    float4 d 											= float4(1, 1, -1, 0) * (0.5f + logh - (int)logh);

    half3 s;
    s 													 = DecodeHDR(gRenderTarget.Sample(gsamLinearClamp, uv + d.xy));
    s 													+= DecodeHDR(gRenderTarget.Sample(gsamLinearClamp, uv + d.wy)) * 2;
    s 													+= DecodeHDR(gRenderTarget.Sample(gsamLinearClamp, uv + d.zy));

    s 													+= DecodeHDR(gRenderTarget.Sample(gsamLinearClamp, uv + d.zw)) * 2;
    s 													+= DecodeHDR(gRenderTarget.Sample(gsamLinearClamp, uv       )) * 4;
    s 													+= DecodeHDR(gRenderTarget.Sample(gsamLinearClamp, uv + d.xw)) * 2;

    s 													+= DecodeHDR(gRenderTarget.Sample(gsamLinearClamp, uv + d.zy));
    s 													+= DecodeHDR(gRenderTarget.Sample(gsamLinearClamp, uv + d.wy)) * 2;
    s 													+= DecodeHDR(gRenderTarget.Sample(gsamLinearClamp, uv + d.xy));

    return s * (1.0 / 16);
}







float4 PS_Prefilter(VertexOut pin) : SV_Target
{
	PostprocessingData matData                          = gPostprocessingData[22];

	float4 s0											= gRenderTarget.Sample(gsamLinearClamp, pin.TexC);
	half3 m 											= s0.rgb;

    // Pixel brightness
    half br 											= Brightness(m);

    // Under-threshold part: quadratic curve
    half rq 											= clamp(br - (matData.Threshold - (matData.Threshold * matData.SoftKnee + 1e-5f)), 0, (matData.Threshold * matData.SoftKnee + 1e-5f) * 2);
    rq 													=(0.25 / (matData.Threshold * matData.SoftKnee + 1e-5f)) * rq * rq;

    // Combine and apply the brightness response curve.
    m 													*= max(rq, br - matData.Threshold) / max(br, 1e-5);

	return EncodeHDR(m);
}

float4 PS_Downsample1(VertexOut pin) : SV_Target
{
	return 												EncodeHDR(DownsampleFilter(pin.TexC));
}

float4 PS_Downsample2(VertexOut pin) : SV_Target
{
	return 												EncodeHDR(DownsampleFilter(pin.TexC));
}


float4 PS_Upsample(VertexOut pin) : SV_Target
{
	PostprocessingData matData                          = gPostprocessingData[22];
    half3 base 											= DecodeHDR(gRenderTarget.Sample(gsamLinearClamp, pin.TexC));
    half3 blur 											= UpsampleFilter(pin.TexC,matData.Radius);
    return 												EncodeHDR(base + blur);
}


float4 PS_Upsample_Final(VertexOut pin) : SV_Target
{
	PostprocessingData matData                          = gPostprocessingData[22];
    half4 base 											= gRenderTarget.Sample(gsamLinearClamp, pin.TexC);
    half3 blur 											= UpsampleFilter(pin.TexC, matData.Radius);
	half3 cout 											= base.rgb + blur * matData.Intensity;
	return 												half4(cout, base.a);
}


#endif

