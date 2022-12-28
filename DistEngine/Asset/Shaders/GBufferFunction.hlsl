/* $Header: /PBR/GBufferFunction.hlsl                                5/10/22 20:55p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : GBufferFunction.hlsl                                         *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#ifndef GBUFFER_FUNCTION_INCLUDE
#define GBUFFER_FUNCTION_INCLUDE
#include "LitFunction.hlsl"

//  GBuffer 结构
struct PS_OUT
{
    float4 GBuffer0                                         : SV_TARGET0;
    float4 GBuffer1                                         : SV_TARGET1;
    float4 GBuffer2                                         : SV_TARGET2;
    float4 GBuffer3                                         : SV_Target3;
};

// Ref: http://jcgt.org/published/0003/02/01/paper.pdf
// Encode with Oct, this function work with any size of output
// return float between [-1, 1]
float2 PackNormalOctQuadEncode(float3 n)
{
    //float l1norm    = dot(abs(n), 1.0);
    //float2 res0     = n.xy * (1.0 / l1norm);

    //float2 val      = 1.0 - abs(res0.yx);
    //return (n.zz < float2(0.0, 0.0) ? (res0 >= 0.0 ? val : -val) : res0);

    // Optimized version of above code:
    n          *= rcp(dot(abs(n), 1.0));
    float t    = saturate(-n.z);
    return     n.xy + (n.xy >= 0.0 ? t : -t);
}

// Pack float2 (each of 12 bit) in 888
float3 PackFloat2To888(float2 f)
{
    uint2 i     = (uint2)(f * 4095.5);
    uint2 hi    = i >> 8;
    uint2 lo    = i & 255;
    // 8 bit in lo, 4 bit in hi
    uint3 cb    = uint3(lo, hi.x | (hi.y << 4));

    return cb   / 255.0;
}

// Unpack 2 float of 12bit packed into a 888
float2 Unpack888ToFloat2(float3 x)
{
    uint3 i     = (uint3)(x * 255.0);
    // 8 bit in lo, 4 bit in hi
    uint hi     = i.z >> 4;
    uint lo     = i.z & 15;
    uint2 cb    = i.xy | uint2(lo << 8, hi << 8);

    return cb   / 4095.0;
}


half3 PackNormal(half3 n)
{
    float2 octNormalWS          = PackNormalOctQuadEncode(n);                               // values between [-1, +1], must use fp32 on some platforms.
    float2 remappedOctNormalWS  = saturate(octNormalWS * 0.5 + 0.5);                        // values between [ 0, +1]
    return                      half3(PackFloat2To888(remappedOctNormalWS));                // values between [ 0, +1]
}

float ShadowFactory(InputData inputData)
{
    half Shadow                                         = inputData.ShadowCoord;
    return                                              Shadow;
}

PS_OUT BRDFDataToGbuffer(SurfaceData surfaceData, InputData inputData, float ShadowMask, float3 globalIllumination)
{
    float3 packedNormalWS                                   = PackNormal(inputData.NormalW);

    PS_OUT output;
    output.GBuffer0 = float4(surfaceData.Albedo.rgb, 1);                                                                            // diffuse                  diffuse                 diffuse                 1          
    output.GBuffer1 = float4(surfaceData.Metallic, surfaceData.Smoothness,surfaceData.Occlusion,1);                                 // metallic                 smoothness              occlusion               1
    output.GBuffer2 = float4(packedNormalWS, 1);                                                                                    // encoded-normal           encoded-normal          encoded-normal          1
    output.GBuffer3 = float4(globalIllumination, 1);                                                                                // GI                       GI                      GI                      ShadowMask

    return output;
}

#endif