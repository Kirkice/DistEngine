/* $Header: Atmosphere.hlsl                                           7/8/22 22:35p KirkZhu $ */
/*--------------------------------------------------------------------------------------------* 
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : Atmosphere.hlsl                                              *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/ 
#include "Core.hlsl"  

#define smooth(x) x*x*(3.0-2.0*x)

#define EnableScattering gParames01.x
#define Height gParames01.y
#define SkyColor gSkyColor
#define zenithOffset gParames01.z
#define density gParames01.w
#define multiScatterPhase gParames02.x
#define anisotropicIntensity gParames02.y

float zenithDensity(float x)
{
    return density / pow(max(x - zenithOffset, 0.35e-2), 0.75);
}
            
float3 getSkyAbsorption(float3 x, float y)
{
	float3 absorption = x * -y;
	absorption = exp2(absorption) * 2.0;
	return absorption;
}

float getSunPoint(float2 p, float2 lp)
{
	return smoothstep(0.1, 0.026, distance(p, lp)) * 80.0;
}

float getRayleigMultiplier(float2 p, float2 lp)
{
	return 1.0 + pow(1.0 - clamp(distance(p, lp), 0.0, 1.0), 2.0) * PI * 0.5;
}

float getMie(float2 p, float2 lp)
{
	float disk = clamp(1.0 - pow(distance(p, lp), 0.1), 0.0, 1.0);
	return disk*disk*(3.0 - 2.0 * disk) * 2.0 * PI;
}

float3 getAtmosphericScattering(float2 p, float2 lp)
{
	float3 GetSkyColor = SkyColor * (1 + anisotropicIntensity);
	float2 correctedLp = lp / max(1920, 1080) * float2(1920,1080);	
	float zenith = zenithDensity(p.y);
	float sunPointDistMult =  clamp(length(max(correctedLp.y + multiScatterPhase - zenithOffset, 0.0)), 0.0, 1.0);
	float rayleighMult = getRayleigMultiplier(p, correctedLp);
	float3 absorption = getSkyAbsorption(GetSkyColor, zenith);
    float3 sunAbsorption = getSkyAbsorption(GetSkyColor, zenithDensity(correctedLp.y + multiScatterPhase));
	float3 sky = GetSkyColor * zenith * rayleighMult;
	float3 sun = getSunPoint(p, correctedLp) * absorption;
	float3 mie = getMie(p, correctedLp) * sunAbsorption;
	
	float3 totalSky = lerp(sky * absorption, sky / (sky + 0.5), sunPointDistMult);
    totalSky += sun + mie;
	totalSky *= sunAbsorption * 0.5 + 0.5 * length(sunAbsorption);
	
	return totalSky;
}

float3 jodieReinhardTonemap(float3 c)
{
    float l = dot(c, float3(0.2126, 0.7152, 0.0722));
    float3 tc = c / (c + 1.0);
    return lerp(c / (l + 1.0), tc, tc);
}

inline void GetScatteringSkyColor(float2 uv, out float4 outColor)
{
	if(EnableScattering < 0.5)
		return;
		
    float2 setUV = uv * 4;
	float2 position = setUV * 2.0;
	float2 lightPosition = float2(1.0, Height);

	float3 scatteringSky = getAtmosphericScattering(position, lightPosition) * PI;
	scatteringSky = jodieReinhardTonemap(scatteringSky);
	scatteringSky = pow(scatteringSky, float3(2.2,2.2,2.2)); //Back to linear
	outColor = float4(scatteringSky,1);
}