/* $Header: WhiteBalance.hlsl                                        3/19/22 20:55p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : WhiteBalance.hlsl                                            *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#ifndef BALANCE_INCLUDE
#define BALANCE_INCLUDE

#include "Core.hlsl"

struct VertexIn
{ 
	float3 PosL    										: POSITION;
	float2 TexC    										: TEXCOORD;
};

struct VertexOut
{
	float4 PosH    										: SV_POSITION;
	float2 TexC    										: TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout                                      = (VertexOut)0.0f;
    vout.PosH 											= float4(vin.PosL, 1.0f);
	vout.TexC 											= vin.TexC;
    return vout;
}

float3 WhiteBalance(float3 In, float Temperature, float Tint)
{
	// Range ~[-1.67;1.67] works best
	float t1 											= Temperature * 10 / 6;
	float t2 											= Tint * 10 / 6;

	// Get the CIE xy chromaticity of the reference white point.
	// Note: 0.31271 = x value on the D65 white point
	float x 												= 0.31271 - t1 * (t1 < 0 ? 0.1 : 0.05);
	float standardIlluminantY 								= 2.87 * x - 3 * x * x - 0.27509507;
	float y 												= standardIlluminantY + t2 * 0.05;

	// Calculate the coefficients in the LMS space.
	float3 w1 												= float3(0.949237, 1.03542, 1.08728); // D65 white point

	// CIExyToLMS
	float Y 												= 1;
	float X 												= Y * x / y;
	float Z 												= Y * (1 - x - y) / y;
	float L 												= 0.7328 * X + 0.4296 * Y - 0.1624 * Z;
	float M 												= -0.7036 * X + 1.6975 * Y + 0.0061 * Z;
	float S 												= 0.0030 * X + 0.0136 * Y + 0.9834 * Z;
	float3 w2 												= float3(L, M, S);

	float3 balance 											= float3(w1.x / w2.x, w1.y / w2.y, w1.z / w2.z);

	float3x3 LIN_2_LMS_MAT = {
		3.90405e-1, 5.49941e-1, 8.92632e-3,
		7.08416e-2, 9.63172e-1, 1.35775e-3,
		2.31082e-2, 1.28021e-1, 9.36245e-1
	};

	float3x3 LMS_2_LIN_MAT = {
		2.85847e+0, -1.62879e+0, -2.48910e-2,
		-2.10182e-1,  1.15820e+0,  3.24281e-4,
		-4.18120e-2, -1.18169e-1,  1.06867e+0
	};

		float3 lms 											= mul(LIN_2_LMS_MAT, In);
		lms 												*= balance;
		float3 Out 											= mul(LMS_2_LIN_MAT, lms);
		return Out;
}
			
float4 PS(VertexOut pin) : SV_Target
{
	PostprocessingData matData                          = gPostprocessingData[28];
	float3 col 											= 0.5 + 0.5 * cos(float3(pin.TexC.x,pin.TexC.y,pin.TexC.x) + float3(0, 2, 4));
	float4 sceneColor									= gRenderTarget.Sample(gsamLinearClamp, pin.TexC);
	float3 finalColor 									= WhiteBalance(sceneColor.rgb, matData.Temperature, matData.Tint);
	return float4(finalColor,sceneColor.a);
}


#endif

