/* $Header: FxAA.hlsl                                                7/21/22 20:06p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : FxAA.hlsl                                                    *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/

#ifndef FXAA_INCLUDE
#define FXAA_INCLUDE

#include "Core.hlsl"

#define ScreenParams float4(1920, 1080, 1.000521, 1.000926)
#define AbsoluteLumaThreshold gFxaaParames.x
#define RelativeLumaThreshold gFxaaParames.y
#define SubpixelBlending gFxaaParames.z
#define DebugMode gFxaaParames.w

#define EXTRA_EDGE_STEPS 10
#define EDGE_STEP_SIZES 1.0, 1.0, 1.0, 1.0, 1.5, 2.0, 2.0, 2.0, 2.0, 4.0
#define LAST_EDGE_STEP_GUESS 8.0

static const float edgeStepSizes[EXTRA_EDGE_STEPS] = { EDGE_STEP_SIZES };

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

inline float4 GetSource(float2 screenUV) {
    return gRenderTarget.Sample(gsamLinearClamp,screenUV);
}

inline float rgb2luma(half3 color)
{
    return dot(color, half3(0.299, 0.587, 0.114));
}

inline float GetLuma (float2 uv, float uOffset = 0.0, float vOffset = 0.0) {
	uv += float2(uOffset, vOffset) * float2(1 / ScreenParams.x, 1 / ScreenParams.y);
	return rgb2luma(GetSource(uv));
}

struct LumaNeighborhood {
	float m, n, e, s, w, ne, se, sw, nw;
	float highest, lowest, range;
};

LumaNeighborhood GetLumaNeighborhood (float2 uv) {
	LumaNeighborhood luma;
	luma.m = GetLuma(uv);
	luma.n = GetLuma(uv, 0.0, 1.0);
	luma.e = GetLuma(uv, 1.0, 0.0);
	luma.s = GetLuma(uv, 0.0, -1.0);
	luma.w = GetLuma(uv, -1.0, 0.0);
	luma.ne = GetLuma(uv, 1.0, 1.0);
	luma.se = GetLuma(uv, 1.0, -1.0);
	luma.sw = GetLuma(uv, -1.0, -1.0);
	luma.nw = GetLuma(uv, -1.0, 1.0);

	luma.highest = max(max(max(max(luma.m, luma.n), luma.e), luma.s), luma.w);
	luma.lowest = min(min(min(min(luma.m, luma.n), luma.e), luma.s), luma.w);
	luma.range = luma.highest - luma.lowest;
	return luma;
}

inline bool IsHorizontalEdge (LumaNeighborhood luma) {
	float horizontal =
		2.0 * abs(luma.n + luma.s - 2.0 * luma.m) +
		abs(luma.ne + luma.se - 2.0 * luma.e) +
		abs(luma.nw + luma.sw - 2.0 * luma.w);
	float vertical =
		2.0 * abs(luma.e + luma.w - 2.0 * luma.m) +
		abs(luma.ne + luma.nw - 2.0 * luma.n) +
		abs(luma.se + luma.sw - 2.0 * luma.s);
	return horizontal >= vertical;
}

struct FXAAEdge {
	bool isHorizontal;
	float pixelStep;
	float lumaGradient, otherLuma;
};

FXAAEdge GetFXAAEdge (LumaNeighborhood luma) {
	FXAAEdge edge;
	edge.isHorizontal = IsHorizontalEdge(luma);
	float lumaP, lumaN;
	if (edge.isHorizontal) {
		edge.pixelStep = 1 / ScreenParams.y;
		lumaP = luma.n;
		lumaN = luma.s;
	}
	else {
		edge.pixelStep = 1 / ScreenParams.x;
		lumaP = luma.e;
		lumaN = luma.w;
	}
	float gradientP = abs(lumaP - luma.m);
	float gradientN = abs(lumaN - luma.m);

	if (gradientP < gradientN) {
		edge.pixelStep = -edge.pixelStep;
		edge.lumaGradient = gradientN;
		edge.otherLuma = lumaN;
	}
	else {
		edge.lumaGradient = gradientP;
		edge.otherLuma = lumaP;
	}
	
	return edge;
}

inline bool CanSkipFXAA (LumaNeighborhood luma) {
	return luma.range < max(AbsoluteLumaThreshold, RelativeLumaThreshold * luma.highest);
}

inline float GetSubpixelBlendFactor (LumaNeighborhood luma) {
	float filter = 2.0 * (luma.n + luma.e + luma.s + luma.w);
	filter += luma.ne + luma.nw + luma.se + luma.sw;
	filter *= 1.0 / 12.0;
	filter = abs(filter - luma.m);
	filter = saturate(filter / luma.range);
	filter = smoothstep(0, 1, filter);
	return filter * filter * SubpixelBlending;
}

inline float GetEdgeBlendFactor (LumaNeighborhood luma, FXAAEdge edge, float2 uv) {
	float2 edgeUV = uv;
	float2 uvStep = 0.0;
	if (edge.isHorizontal) {
		edgeUV.y += 0.5 * edge.pixelStep;
		uvStep.x = 1 / ScreenParams.x;
	}
	else {
		edgeUV.x += 0.5 * edge.pixelStep;
		uvStep.y = 1 / ScreenParams.y;
	}

	float edgeLuma = 0.5 * (luma.m + edge.otherLuma);
	float gradientThreshold = 0.25 * edge.lumaGradient;
			
	float2 uvP = edgeUV + uvStep;
	float lumaDeltaP = GetLuma(uvP) - edgeLuma;
	bool atEndP = abs(lumaDeltaP) >= gradientThreshold;

	for (int i = 0; i < EXTRA_EDGE_STEPS && !atEndP; i++) {
		uvP += uvStep * edgeStepSizes[i];
		lumaDeltaP = GetLuma(uvP) - edgeLuma;
		atEndP = abs(lumaDeltaP) >= gradientThreshold;
	}
	if (!atEndP) {
		uvP += uvStep * LAST_EDGE_STEP_GUESS;
	}

	float2 uvN = edgeUV - uvStep;
	float lumaDeltaN = GetLuma(uvN) - edgeLuma;
	bool atEndN = abs(lumaDeltaN) >= gradientThreshold;

	for (int i = 0; i < EXTRA_EDGE_STEPS && !atEndN; i++) {
		uvN -= uvStep * edgeStepSizes[i];
		lumaDeltaN = GetLuma(uvN) - edgeLuma;
		atEndN = abs(lumaDeltaN) >= gradientThreshold;
	}
	if (!atEndN) {
		uvN -= uvStep * LAST_EDGE_STEP_GUESS;
	}

	float distanceToEndP, distanceToEndN;
	if (edge.isHorizontal) {
		distanceToEndP = uvP.x - uv.x;
		distanceToEndN = uv.x - uvN.x;
	}
	else {
		distanceToEndP = uvP.y - uv.y;
		distanceToEndN = uv.y - uvN.y;
	}

	float distanceToNearestEnd;
	bool deltaSign;
	if (distanceToEndP <= distanceToEndN) {
		distanceToNearestEnd = distanceToEndP;
		deltaSign = lumaDeltaP >= 0;
	}
	else {
		distanceToNearestEnd = distanceToEndN;
		deltaSign = lumaDeltaN >= 0;
	}

	if (deltaSign == (luma.m - edgeLuma >= 0)) {
		return 0.0;
	}
	else {
		return 0.5 - distanceToNearestEnd / (distanceToEndP + distanceToEndN);
	}
}

float4 PS(VertexOut pin) : SV_Target
{
	LumaNeighborhood luma = GetLumaNeighborhood(pin.TexC);

	if (CanSkipFXAA(luma)) {
		if(DebugMode > 0.5)
			return float4(0,0,0,1);
			
		return GetSource(pin.TexC);
	}	
	FXAAEdge edge = GetFXAAEdge(luma);

	float blendFactor = max(
		GetSubpixelBlendFactor(luma), GetEdgeBlendFactor(luma, edge, pin.TexC)
	);

	float2 blendUV = pin.TexC;
	if (edge.isHorizontal) {
		blendUV.y += blendFactor * edge.pixelStep;
	}
	else {
		blendUV.x += blendFactor * edge.pixelStep;
	}

	return GetSource(blendUV);
}

#endif
