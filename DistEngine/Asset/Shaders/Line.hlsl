/* $Header: LinePass.hlsl                                           6/26/21 20:55p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : LinePass.hlsl                                            	  *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#ifndef LINE_INCLUDE
#define LINE_INCLUDE

#include "Core.hlsl"

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
    vout.PosH                                           = TransformObjectToHClip(vin.PosL);
    return vout;
}
 
float4 PS(VertexOut pin) : SV_Target
{
    PBRMaterialData matData                             = gMaterialData[gMaterialIndex];
    return matData.DiffuseColor;
}
 
#endif