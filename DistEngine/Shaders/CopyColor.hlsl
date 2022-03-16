/* $Header: RGBSplit.hlsl                                           6/26/21 20:55p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : CopyColor.hlsl                                               *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#ifndef COPYCOLOR_INCLUDE
#define COPYCOLOR_INCLUDE

Texture2D gRenderTarget    									: register(t0);
Texture2D gNULL												: register(t1);

SamplerState gsamPointClamp 								: register(s0);
SamplerState gsamLinearClamp 								: register(s1);
SamplerState gsamDepthMap 									: register(s2);
SamplerState gsamLinearWrap 								: register(s3);


struct VertexIn
{ 
	float3 PosL    											: POSITION;
	float2 TexC    											: TEXCOORD;
};

struct VertexOut
{
	float4 PosH    											: SV_POSITION;
	float2 TexC    											: TEXCOORD0;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout                                      	= (VertexOut)0.0f;
    vout.PosH 												= float4(vin.PosL, 1.0f);
	vout.TexC 												= vin.TexC;
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float4 color 											= gRenderTarget.Sample(gsamLinearClamp, pin.TexC); 
	return color;
}

#endif

