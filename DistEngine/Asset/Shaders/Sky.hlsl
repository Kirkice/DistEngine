//=============================================================================
// Sky.fx by Frank Luna (C) 2011 All Rights Reserved.
//=============================================================================

// Include common HLSL code.
#include "Atmosphere.hlsl"

struct VertexIn
{
	float3 PosL    : POSITION;
	float3 NormalL : NORMAL;
	float2 TexC    : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
    float3 PosL : POSITION0;
};

float3 RotateAroundYInDegrees (float3 vertex, float degrees)
{
    float alpha 											= degrees * PI / 180.0;
    float sina, cosa;
    sincos(alpha, sina, cosa);
    float2x2 												m = float2x2(cosa, -sina, sina, cosa);
    return 													float3(mul(m, vertex.xz), vertex.y).xzy;
}

float3 aces_approx(float3 v)
{
    v 														*= 0.6f;
    float a 												= 2.51f;
    float b 												= 0.03f;
    float c 												= 2.43f;
    float d 												= 0.59f;
    float e 												= 0.14f;
    return 													clamp((v*(a*v+b))/(v*(c*v+d)+e), 0.0f, 1.0f);
}

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.PosL = vin.PosL;

	SkyBoxMaterialData matData                             	= gSkyMaterialData[gMaterialIndex];

    float3 rotated 											= RotateAroundYInDegrees(vin.PosL, matData.SkyBoxRotation);
	float4 posW 											= mul(float4(rotated, 1.0f), gWorld);
	posW.xyz 												+= gEyePosW;
	vout.PosH 												= mul(posW, gViewProj).xyww;
	return 													vout;
}


float4 PS(VertexOut pin) : SV_Target
{
	SkyBoxMaterialData matData                             	= gSkyMaterialData[gMaterialIndex];
	float4 outColor											= float4(0,0,0,1);

	GetScatteringSkyColor(pin.PosL.xy, outColor);

	outColor.rgb											= outColor.rgb * matData.SkyBoxTint.rgb * matData.SkyBoxExposure;

	if(matData.ACES > 0.5)
		outColor.rgb 										= aces_approx(outColor.rgb);

	return outColor;  
}

