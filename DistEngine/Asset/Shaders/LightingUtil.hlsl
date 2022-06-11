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

#define LIGHTS_HALF_MIN 6.103515625e-5 
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
    half                                                                            strength;
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
    light.color                                                                     = gDirectionLights.Color.rgb * gDirectionLights.Strength * gDirectionLights.Active;
    light.strength                                                                  = gDirectionLights.Strength;
    return                                                                          light;
}

// Matches Unity Vanila attenuation
// Attenuation smoothly decreases to light range.
float UNITY_DistanceAttenuation(float distanceSqr, half2 distanceAttenuation)
{
    // We use a shared distance attenuation for additional directional and puctual lights
    // for directional lights attenuation will be 1
    float lightAtten                                                                = rcp(distanceSqr);
    half smoothFactor                                                               = saturate(distanceSqr * distanceAttenuation.x + distanceAttenuation.y);

    return                                                                          lightAtten * smoothFactor;
}

Light GetPointLight(float3 positionWS)
{
    float lightRangeSqr                                                             = gPointLights.rangeFactory * gPointLights.rangeFactory;
    float fadeStartDistanceSqr                                                      = 0.8f * 0.8f * lightRangeSqr;
    float fadeRangeSqr                                                              = (fadeStartDistanceSqr - lightRangeSqr);
    float lightRangeSqrOverFadeRangeSqr                                             = -lightRangeSqr / fadeRangeSqr;
    float oneOverLightRangeSqr                                                      = 1.0f / max(0.0001f, (gPointLights.rangeFactory * gPointLights.rangeFactory));

    float2 lightAttenuation                                                         = float2(oneOverLightRangeSqr,lightRangeSqrOverFadeRangeSqr);
    // Directional lights store direction in lightPosition.xyz and have .w set to 0.0.
    // This way the following code will work for both directional and punctual lights.
        // Directional lights store direction in lightPosition.xyz and have .w set to 0.0.
    // This way the following code will work for both directional and punctual lights.
    float3 lightVector                                                              = gPointLights.Position.xyz - positionWS;
    float distanceSqr                                                               = max(dot(lightVector, lightVector), LIGHTS_HALF_MIN);

    half3 lightDirection                                                            = half3(lightVector * rsqrt(distanceSqr));
    half attenuation                                                                = UNITY_DistanceAttenuation(distanceSqr, lightAttenuation.xy);

    Light light;
    light.direction                                                                 = lightDirection;
    light.distanceAttenuation                                                       = attenuation;
    light.shadowAttenuation                                                         = 1;
    light.color                                                                     = gPointLights.Color.rgb * gPointLights.Strength * gPointLights.Active;
    light.strength                                                                  = 1;
    return                                                                          light;
}