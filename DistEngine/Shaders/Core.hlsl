/* $Header: /Core/Core.hlsl                                        6/26/21 20:55p KirkZhu $ */
/*--------------------------------------------------------------------------------------------* 
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : Core.hlsl                                                    *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/ 
#include "Common.hlsl"
#include "SH.hlsl"

#ifndef CORE_INCLUDED
#define CORE_INCLUDED

#define real                                                                                float
#define real2                                                                               float2
#define real3                                                                               float3
#define real4                                                                               float4

#define real2x2                                                                             float2x2
#define real2x3                                                                             float2x3
#define real3x2                                                                             float3x2 
#define real3x3                                                                             float3x3
#define real3x4                                                                             float3x4
#define real4x3                                                                             float4x3
#define real4x4                                                                             float4x4

static const float3 kDielectricSpecular                                                     = float3(0.04, 0.04, 0.04);

// ----------------------------------------------------------------------------
// Common math functions
// ----------------------------------------------------------------------------

real DegToRad(real deg)
{
    return                                                                                  deg * (PI / 180.0);
}

real RadToDeg(real rad)
{
    return                                                                                  rad * (180.0 / PI);
}

bool IsPower2(uint x)
{
    return                                                                                  (x & (x - 1)) == 0;
}

float PositivePow(float base, float power)
{
    return                                                                                  pow(abs(base), power);
}
// Input [0, 1] and output [0, PI/2]
// 9 VALU
real FastACosPos(real inX)
{
    real x                                                                                  = abs(inX);
    real res                                                                                = (0.0468878 * x + -0.203471) * x + 1.570796; // p(x)
    res                                                                                     *= sqrt(1.0 - x);

    return res;
}

// Ref: https://seblagarde.wordpress.com/2014/12/01/inverse-trigonometric-functions-gpu-optimization-for-amd-gcn-architecture/
// Input [-1, 1] and output [0, PI]
// 12 VALU
real FastACos(real inX)
{
    real res                                                                                = FastACosPos(inX);

    return                                                                                  (inX >= 0) ? res : PI - res; // Undo range reduction
}

// Same cost as Acos + 1 FR
// Same error
// input [-1, 1] and output [-PI/2, PI/2]
real FastASin(real x)
{
    return                                                                                  HALF_PI - FastACos(x);
}

// max absolute error 1.3x10^-3
// Eberly's odd polynomial degree 5 - respect bounds
// 4 VGPR, 14 FR (10 FR, 1 QR), 2 scalar
// input [0, infinity] and output [0, PI/2]
real FastATanPos(real x)
{
    real t0                                                                                 = (x < 1.0) ? x : 1.0 / x;
    real t1                                                                                 = t0 * t0;
    real poly                                                                               = 0.0872929;
    poly                                                                                    = -0.301895 + poly * t1;
    poly                                                                                    = 1.0 + poly * t1;
    poly                                                                                    = poly * t0;
    return                                                                                  (x < 1.0) ? poly : HALF_PI - poly;
}

// 4 VGPR, 16 FR (12 FR, 1 QR), 2 scalar
// input [-infinity, infinity] and output [-PI/2, PI/2]
real FastATan(real x)
{
    real t0                                                                                 = FastATanPos(abs(x));
    return                                                                                  (x < 0.0) ? -t0 : t0;
}

//CORE
// Normalize that account for vectors with zero length
float3 SafeNormalize(float3 inVec)
{
    float dp3                                                                               = max(FLT_MIN, dot(inVec, inVec));
    return                                                                                  inVec * rsqrt(dp3);
}

// Division which returns 1 for (inf/inf) and (0/0).
// If any of the input parameters are NaNs, the result is a NaN.
real SafeDiv(real numer, real denom)
{
    return                                                                                  (numer != denom) ? numer / denom : 1;
}

// Assumes that (0 <= x <= Pi).
real SinFromCos(real cosX)
{
    return                                                                                  sqrt(saturate(1 - cosX * cosX));
}

// Dot product in spherical coordinates.
real SphericalDot(real cosTheta1, real phi1, real cosTheta2, real phi2)
{
    return                                                                                  SinFromCos(cosTheta1) * SinFromCos(cosTheta2) * cos(phi1 - phi2) + cosTheta1 * cosTheta2;
}

// [start, end] -> [0, 1] : (x - start) / (end - start) = x * rcpLength - (start * rcpLength)
float Remap01(float x, float rcpLength, float startTimesRcpLength)
{
    return                                                                                  saturate(x * rcpLength - startTimesRcpLength);
}

// [start, end] -> [1, 0] : (end - x) / (end - start) = (end * rcpLength) - x * rcpLength
float Remap10(float x, float rcpLength, float endTimesRcpLength)
{
    return                                                                                  saturate(endTimesRcpLength - x * rcpLength);
}

// Remap: [0.5 / size, 1 - 0.5 / size] -> [0, 1]
real2 RemapHalfTexelCoordTo01(real2 coord, real2 size)
{
    const                                                                                   real2 rcpLen              = size * rcp(size - 1);
    const                                                                                   real2 startTimesRcpLength = 0.5 * rcp(size - 1);

    return                                                                                  Remap01(coord, rcpLen, startTimesRcpLength);
}

// Remap: [0, 1] -> [0.5 / size, 1 - 0.5 / size]
real2 Remap01ToHalfTexelCoord(real2 coord, real2 size)
{
    const real2 start                                                                       = 0.5 * rcp(size);
    const real2 len                                                                         = 1 - rcp(size);

    return                                                                                  coord * len + start;
}

// smoothstep that assumes that 'x' lies within the [0, 1] interval.
real Smoothstep01(real x)
{
    return                                                                                  x * x * (3 - (2 * x));
}

real Smootherstep01(real x)
{
  return                                                                                    x * x * x * (x * (x * 6 - 15) + 10);
}

real Smootherstep(real a, real b, real t)
{
    real r                                                                                  = rcp(b - a);
    real x                                                                                  = Remap01(t, r, a * r);
    return                                                                                  Smootherstep01(x);
}

float3 NLerp(float3 A, float3 B, float t)
{
    return                                                                                  normalize(lerp(A, B, t));
}

float Length2(float3 v)
{
    return                                                                                  dot(v, v);
}

real Pow4(real x)
{
    return                                                                                  (x * x) * (x * x);
}


// ----------------------------------------------------------------------------
// Texture format sampling
// ----------------------------------------------------------------------------

float2 DirectionToLatLongCoordinate(float3 unDir)
{
    float3 dir                                                                              = normalize(unDir);
    // coordinate frame is (-Z, X) meaning negative Z is primary axis and X is secondary axis.
    return                                                                                  float2(1.0 - 0.5 * INV_PI * atan2(dir.x, -dir.z), asin(dir.y) * INV_PI + 0.5);
}

float3 LatlongToDirectionCoordinate(float2 coord)
{
    float theta                                                                             = coord.y * PI;
    float phi                                                                               = (coord.x * 2.f * PI - PI*0.5f);

    float cosTheta                                                                          = cos(theta);
    float sinTheta                                                                          = sqrt(1.0 - min(1.0, cosTheta*cosTheta));
    float cosPhi                                                                            = cos(phi);
    float sinPhi                                                                            = sin(phi);

    float3 direction                                                                        = float3(sinTheta*cosPhi, cosTheta, sinTheta*sinPhi);
    direction.xy                                                                            *= -1.0;
    return                                                                                  direction;
}

//----------------------------------------------------------------------------------------------------

//SPACE  TRANSFORM
float3 GetCameraPositionWS()
{
    return                                                             (float3)gEyePosW.xyz;
}

float4x4 GetObjectToWorldMatrix()
{
    return                                                              gWorld;
}

float4x4 GetWorldToObjectMatrix()
{
    return                                                              gInvWorld;
}

float4x4 GetWorldToViewMatrix()
{
    return                                                              gView;
}

// Transform to homogenous clip space
float4x4 GetWorldToHClipMatrix()
{
    return                                                              gViewProj;
}

// Transform to homogenous clip space
float4x4 GetViewToHClipMatrix()
{
    return                                                              gProj;
}

float3 TransformObjectToWorld(float3 positionOS)
{
    return                                                              mul(float4(positionOS, 1.0), GetObjectToWorldMatrix()).xyz;
}

float3 TransformWorldToObject(float3 positionWS)
{
    return                                                              mul(float4(positionWS, 1.0), GetWorldToObjectMatrix()).xyz;
}

float3 TransformWorldToView(float3 positionWS)
{
    return                                                              mul(float4(positionWS, 1.0), GetWorldToViewMatrix()).xyz;
}

// Transforms position from object space to homogenous space
float4 TransformObjectToHClip(float3 positionOS)
{
    return                                                              mul(mul(float4(positionOS, 1.0), GetObjectToWorldMatrix()),GetWorldToHClipMatrix());
}

// Tranforms position from world space to homogenous space
float4 TransformWorldToHClip(float3 positionWS)
{
    return                                                              mul(float4(positionWS, 1.0), GetWorldToHClipMatrix());
}

// Tranforms position from view space to homogenous space
float4 TransformWViewToHClip(float3 positionVS)
{
    return                                                              mul(float4(positionVS, 1.0), GetViewToHClipMatrix());
}

// Normalize to support uniform scaling
float3 TransformObjectToWorldDir(float3 dirOS, bool doNormalize = true)
{
    float3 dirWS                                                        = mul(dirOS, (float3x3)GetObjectToWorldMatrix());
    if (doNormalize)
        return                                                          SafeNormalize(dirWS);

    return                                                              dirWS;
}

// Normalize to support uniform scaling
float3 TransformWorldToObjectDir(float3 dirWS, bool doNormalize = true)
{
    float3 dirOS                                                        = mul(dirWS, (float3x3)GetWorldToObjectMatrix());
    if (doNormalize)
        return                                                          normalize(dirOS);

    return                                                              dirOS;
}

// Tranforms vector from world space to view space
float3 TransformWorldToViewDir(float3 dirWS, bool doNormalize = false)
{
    float3 dirVS                                                        = mul(dirWS, (float3x3)GetWorldToViewMatrix()).xyz;
    if (doNormalize)
        return                                                          normalize(dirVS);

    return                                                              dirVS; 
}

// Tranforms vector from world space to homogenous space
float3 TransformWorldToHClipDir(float3 directionWS, bool doNormalize = false)
{
    float3 dirHCS                                                       = mul(directionWS, (float3x3)GetWorldToHClipMatrix()).xyz;
    if (doNormalize)
        return                                                          normalize(dirHCS);

    return                                                              dirHCS;
}

// Transforms normal from object to world space
float3 TransformObjectToWorldNormal(float3 normalOS, bool doNormalize = true)
{
    float3 normalWS                                                     = mul(normalOS, (float3x3)GetObjectToWorldMatrix());
    if (doNormalize)
        return                                                          normalize(normalWS);

    return                                                              normalWS;
}

// Transforms normal from world to object space
float3 TransformWorldToObjectNormal(float3 normalWS, bool doNormalize = true)
{
    // Normal need to be multiply by inverse transpose
    float3 normalOS                                                     = mul(normalWS, (float3x3)GetWorldToObjectMatrix());
    if (doNormalize)
        return                                                          SafeNormalize(normalOS);

    return                                                              normalOS;
}

float3x3 CreateTangentToWorld(float3 normal, float3 tangent, float3 flipSign)
{
    // For odd-negative scale transforms we need to flip the sign
    float3 bitangent                                                    = cross(normal, tangent);

    return                                                              float3x3(tangent, bitangent, normal);
}

float3 TransformTangentToWorld(float3 dirTS, float3x3 tangentToWorld)
{
    // Note matrix is in row major convention with left multiplication as it is build on the fly
    return                                                              mul(dirTS, tangentToWorld);
}

// This function does the exact inverse of TransformTangentToWorld() and is
// also decribed within comments in mikktspace.h and it follows implicitly
// from the scalar triple product (google it).
float3 TransformWorldToTangent(float3 dirWS, float3x3 tangentToWorld)
{
    // Note matrix is in row major convention with left multiplication as it is build on the fly
    float3 row0                                                         = tangentToWorld[0];
    float3 row1                                                         = tangentToWorld[1];
    float3 row2                                                         = tangentToWorld[2];
    
    // these are the columns of the inverse matrix but scaled by the determinant
    float3 col0                                                         = cross(row1, row2);
    float3 col1                                                         = cross(row2, row0);
    float3 col2                                                         = cross(row0, row1);
    
    float determinant                                                   = dot(row0, col0);
    float sgn                                                           = determinant<0.0 ? (-1.0) : 1.0;
    
    // inverse transposed but scaled by determinant
    // Will remove transpose part by using matrix as the first arg in the mul() below
    // this makes it the exact inverse of what TransformTangentToWorld() does.
    float3x3 matTBN_I_T                                                 = float3x3(col0, col1, col2);
    
    return                                                              SafeNormalize( sgn * mul(dirWS, matTBN_I_T) );
}

float3 TransformTangentToObject(float3 dirTS, float3x3 tangentToWorld)
{
    // Note matrix is in row major convention with left multiplication as it is build on the fly
    float3 normalWS                                                     = TransformTangentToWorld(dirTS, tangentToWorld);
    return                                                              TransformWorldToObjectNormal(normalWS);
}

float3 TransformObjectToTangent(float3 dirOS, float3x3 tangentToWorld)
{
    // Note matrix is in row major convention with left multiplication as it is build on the fly

    // don't normalize, as normalWS will be normalized after TransformWorldToTangent 
    float3 normalWS                                                     = TransformObjectToWorldNormal(dirOS, false);
    
    // transform from world to tangent
    return                                                              TransformWorldToTangent(normalWS, tangentToWorld);
}

#endif