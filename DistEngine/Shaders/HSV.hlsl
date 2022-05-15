/* $Header: Decolor.hlsl                                            3/19/22 20:55p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : Decolor.hlsl                                                *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#ifndef DECOLOR_INCLUDE
#define DECOLOR_INCLUDE

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
	PostprocessingData matData                          = gPostprocessingData[24];
	float4 source										= gRenderTarget.Sample(gsamLinearClamp, pin.TexC);

    float3 rgb 											= source.rgb;
    float4 k 											= float4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    float4 p 											= lerp(float4(rgb.bg, k.wz), float4(rgb.gb, k.xy), step(rgb.b, rgb.g));

    // 比较r和max(b,g)
    float4 q 											= lerp(float4(p.xyw, rgb.r), float4(rgb.r, p.yzx), step(p.x, rgb.r));
    float d 											= q.x - min(q.w, q.y);
    float e 											= 1.0e-10;
    float3 hsv 											= float3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);

    hsv.x 												= hsv.x + matData.Hue;
    hsv.y 												= hsv.y + matData.Saturation;
    hsv.z 												= hsv.z + matData.Value;
    rgb 												= saturate(3.0*abs(1.0-2.0*frac(hsv.x+float3(0.0,-1.0/3.0,1.0/3.0)))-1); //明度和饱和度为1时的颜色
    rgb 												= (lerp(float3(1,1,1),rgb,hsv.y)*hsv.z); // hsv
	source.rgb											= rgb;
	
	return source;
}


#endif

