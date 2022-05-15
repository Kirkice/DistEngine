// Include common HLSL code.
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
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout                                      = (VertexOut)0.0f;

	PBRMaterialData matData                                = gMaterialData[gMaterialIndex];

    float4 posW                                         = mul(float4(vin.PosL, 1.0f), gWorld);
    // posW.xy                                             = posW.xy + normalize(vin.NormalL).xy * float2(matData.OutLineWidth * 0.1, matData.OutLineWidth * 0.1);
    vout.PosH                                           = mul(posW, gViewProj);
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    PBRMaterialData matData                                = gMaterialData[gMaterialIndex];
    return float4(0,0,0,1);
}


