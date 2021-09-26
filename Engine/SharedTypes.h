
/* $Header: /Engine/SharedTypes.h						8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : SharedTypes.h											  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#include "../common/DistMath/include/DistMath.h"

#pragma once

typedef math::Vector2 float2;
typedef math::Vector3 float3;
typedef math::Vector4 float4;

typedef uint32 uint;
typedef math::Uint2 uint2;
typedef math::Uint3 uint3;
typedef math::Uint4 uint4;

typedef math::Int2 int2;
typedef math::Int3 int3;
typedef math::Int4 int4;

struct Material
{
	uint Albedo;
	uint Normal;
	uint Roughness;
	uint Metallic;
	uint Opacity;
	uint Emissive;
};

struct SpotLight
{
	float3 Position;
	float AngularAttenuationX;
	float3 Direction;
	float AngularAttenuationY;
	float3 Intensity;
	float Range;
};

struct ClusterBounds
{
	float3 Position;
	math::Quaternion Orientation;
	float3 Scale;
	uint2 ZBounds;
};

struct GeometryInfo
{
	uint VtxOffset;
	uint IdxOffset;
	uint MaterialIdx;
	uint PadTo16Bytes;
};