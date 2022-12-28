/* $Header: GizmoPass.hlsl                                           6/26/21 20:55p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : GizmoPass.hlsl                                               *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#ifndef GIZMOPASS_INCLUDE
#define GIZMOPASS_INCLUDE

#include "Core.hlsl"
#include "LightingUtil.hlsl"

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

    float3 ori                                          = TransformObjectToWorld(float3(0,0,0));
    ori                                                 = TransformWorldToView(ori);

    float3 vt                                           = TransformObjectToWorld(vin.PosL);
    vt.y                                                = vt.z;
    vt.z                                                = 0;
    vt.xyz                                              += ori.xyz;
    vout.PosH                                           = TransformWViewToHClip(vt);
	vout.TexC                                           = vin.TexC.xy;

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    PBRMaterialData matData                             = gMaterialData[gMaterialIndex];
    uint diffuseMapIndex                                = matData.DiffuseMapIndex;
    float4 color                                        = gGizmoTextureMaps[diffuseMapIndex].Sample(gsamAnisotropicWrap, pin.TexC);
    clip(color.a - 0.5);
    color                                               = gGizmoTextureMaps[diffuseMapIndex].Sample(gsamAnisotropicWrap, pin.TexC) * float4(gDirectionLights.Color,1);
    return color;
}

#endif