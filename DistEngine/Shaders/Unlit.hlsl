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

	PBRMaterialData matData                             = gMaterialData[gMaterialIndex];
	float4 texC                                         = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
	vout.TexC                                           = mul(texC, matData.MatTransform).xy;

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    PBRMaterialData matData                             = gMaterialData[gMaterialIndex];
    uint diffuseMapIndex                                = matData.DiffuseMapIndex;
    float4 color                                        = gTextureMaps[diffuseMapIndex].Sample(gsamAnisotropicWrap, pin.TexC);
    return color;
}
#endif