/* $Header: GBuffer0.hlsl                                            6/26/21 20:55p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : GBuffer0.hlsl                                                *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/

#ifndef GBUFFER_INCLUDE
#define GBUFFER_INCLUDE

#include "GBufferFunction.hlsl"

struct VertexIn
{ 
	float3 PosL                                         : POSITION;
    float3 NormalL                                      : NORMAL;
	float2 TexC                                         : TEXCOORD;
	float3 TangentL                                     : TANGENT;
};

struct VertexOut
{
	float4 PosH                                         : SV_POSITION;
    float3 NormalW                                      : NORMAL;
	float3 TangentW                                     : TANGENT;
	float2 TexC                                         : TEXCOORD;
};


VertexOut VS(VertexIn vin)
{
    VertexOut vout                                      = (VertexOut)0.0f;

	PBRMaterialData matData                             = gMaterialData[gMaterialIndex];

    vout.NormalW                                        = TransformObjectToWorldNormal(vin.NormalL);
	vout.TangentW                                       = TransformObjectToWorld(vin.TangentL);
    vout.PosH                                           = TransformObjectToHClip(vin.PosL);

	float4 texC                                         = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
	vout.TexC                                           = mul(texC, matData.MatTransform).xy;
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    SurfaceData surfaceData; 
    InitializeStandardLitSurfaceData(pin.TexC, pin.NormalW, pin.TangentW, surfaceData);
    return                                              float4(surfaceData.Albedo.rgb, 1);
}

#endif