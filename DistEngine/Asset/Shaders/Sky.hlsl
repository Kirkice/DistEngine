//=============================================================================
// Sky.fx by Frank Luna (C) 2011 All Rights Reserved.
//=============================================================================

// Include common HLSL code.
#include "Core.hlsl"
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

#define Height gParames01.y
#define GroundColor gGroundColor//float3(0.32, 0.37, 0.47)
#define HEIGHT_RAY gParames01.z
#define HEIGHT_MIE gParames01.w
#define HEIGHT_ABSORPTION gParames02.x
#define ABSORPTION_FALLOFF gParames02.y


float4 PS(VertexOut pin) : SV_Target
{
	SkyBoxMaterialData matData                             	= gSkyMaterialData[gMaterialIndex];
	// float3 color 											= gCubeMap.Sample(gsamLinearWrap, pin.PosL).rgb;


	// color													= color * (1 - gParames01).xxx + gParames01.xxx * scatteringColor;

    float2 setUV = pin.PosL.xy * 4;
	float2 position = setUV * 2.0;
	float2 lightPosition = float2(1.0, Height);

	float3 color 											= getAtmosphericScattering(position, lightPosition) * PI;
	color 													= jodieReinhardTonemap(color);
	color 													= pow(color, float3(2.2,2.2,2.2)); //Back to linear
	color													= color * matData.SkyBoxTint.rgb * matData.SkyBoxExposure;
	if(matData.ACES > 0.5)
		color 												= aces_approx(color);

	return float4(color,1);  
}

