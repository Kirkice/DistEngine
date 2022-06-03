/* $Header: GBuffer2.hlsl                                            6/26/21 20:55p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : GBuffer2.hlsl                                                *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/

#ifndef GBUFFER2_INCLUDE
#define GBUFFER2_INCLUDE

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
	vout.TexC                                           = vin.TexC;
    return vout; 
}

float4 PS(VertexOut pin) : SV_Target
{
    SurfaceData surfaceData; 
    InitializeStandardLitSurfaceData(pin.TexC, pin.NormalW, pin.TangentW, surfaceData);

    float3 packedNormalWS                               = PackNormal(surfaceData.Normal);
    return                                              half4(packedNormalWS, 1); 
}

#endif