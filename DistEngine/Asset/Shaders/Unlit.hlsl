/* $Header: UnlitPass.hlsl                                           6/26/21 20:55p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : UnlitPass.hlsl                                               *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#ifndef UNLIT_INCLUDE
#define UNLIT_INCLUDE

#include "Core.hlsl"

struct VertexIn
{ 
	float3 PosL                                         : POSITION;
};

struct VertexOut
{
	float4 PosH                                         : SV_POSITION;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout                                      = (VertexOut)0.0f;
    vout.PosH                                           = TransformObjectToHClip(vin.PosL);
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    PBRMaterialData matData                             = gMaterialData[gMaterialIndex];
    return matData.DiffuseColor;
}
#endif