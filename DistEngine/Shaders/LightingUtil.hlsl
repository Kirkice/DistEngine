/* $Header: /PBR/LightingUtil.hlsl                                    6/26/21 20:55p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : LightingUtil.hlsl                                            *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
//引擎传来的灯光结构
#include "Common.hlsl"

///////////////////////////////////////////////////////////////////////////////
//                          Light Helpers                                    //
///////////////////////////////////////////////////////////////////////////////

// Abstraction over Light shading data.
struct Light
{
    half3                                                                           direction;
    half3                                                                           color;
    half                                                                            distanceAttenuation;
    half                                                                            shadowAttenuation;
};

///////////////////////////////////////////////////////////////////////////////
//                        Attenuation Functions                               /
///////////////////////////////////////////////////////////////////////////////

// Matches Unity Vanila attenuation
// Attenuation smoothly decreases to light range.
float DistanceAttenuation(float distanceSqr, half2 distanceAttenuation)
{
    // We use a shared distance attenuation for additional directional and puctual lights
    // for directional lights attenuation will be 1
    float lightAtten                                                                = rcp(distanceSqr);

    half factor                                                                     = distanceSqr * distanceAttenuation.x;
    half smoothFactor                                                               = saturate(1.0h - factor * factor);
    smoothFactor                                                                    = smoothFactor * smoothFactor;
    return                                                                          lightAtten * smoothFactor;
}

half AngleAttenuation(half3 spotDirection, half3 lightDirection, half2 spotAttenuation)
{
    // Spot Attenuation with a linear falloff can be defined as
    // (SdotL - cosOuterAngle) / (cosInnerAngle - cosOuterAngle)
    // This can be rewritten as
    // invAngleRange = 1.0 / (cosInnerAngle - cosOuterAngle)
    // SdotL * invAngleRange + (-cosOuterAngle * invAngleRange)
    // SdotL * spotAttenuation.x + spotAttenuation.y

    // If we precompute the terms in a MAD instruction
    half SdotL                                                                      = dot(spotDirection, lightDirection);
    half atten                                                                      = saturate(SdotL * spotAttenuation.x + spotAttenuation.y);
    return                                                                          atten * atten;
}

///////////////////////////////////////////////////////////////////////////////
//                      Light Abstraction                                    //
///////////////////////////////////////////////////////////////////////////////

Light GetMainLight() 
{
    Light light;
    light.direction                                                                 = -gDirectionLights.Direction.xyz;
    light.distanceAttenuation                                                       = 1.0; // unity_LightData.z is 1 when not culled by the culling mask, otherwise 0.
    light.shadowAttenuation                                                         = 1.0;
    light.color                                                                     = gDirectionLights.Color.rgb * gDirectionLights.Strength;

    return                                                                          light;
}