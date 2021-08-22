
/* $Header: /common/distMath/src/Uint.cpp	            8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Uint.cpp												  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/

#include "../include/Uint.h"
#include "../include/DistMathPublic.h"

using namespace DirectX;
using namespace DirectX::PackedVector;

namespace math
{
	// == Uint2 =======================================================================================
	Uint2::Uint2() : x(0), y(0)
	{
	}

	Uint2::Uint2(uint32 x_, uint32 y_) : x(x_), y(y_)
	{
	}

	bool Uint2::operator==(Uint2 other) const
	{
		return x == other.x && y == other.y;
	}

	bool Uint2::operator!=(Uint2 other) const
	{
		return x != other.x || y != other.y;
	}

	// == Int3 =======================================================================================
	Uint3::Uint3() : x(0), y(0), z(0)
	{
	}

	Uint3::Uint3(uint32 x_, uint32 y_, uint32 z_) : x(x_), y(y_), z(z_)
	{
	}

	bool Uint3::operator==(const Uint3& other) const
	{
		return x == other.x && y == other.y && z == other.z;
	}

	bool Uint3::operator!=(const Uint3& other) const
	{
		return x != other.x || y != other.y || z != other.z;
	}

	// == Uint4 =======================================================================================

	Uint4::Uint4() : x(0), y(0), z(0), w(0)
	{
	}

	Uint4::Uint4(uint32 x_, uint32 y_, uint32 z_, uint32 w_) : x(x_), y(y_), z(z_), w(w_)
	{
	}

	bool Uint4::operator==(const Uint4& other) const
	{
		return x == other.x && y == other.y && z == other.z && w == other.w;
	}

	bool Uint4::operator!=(const Uint4& other) const
	{
		return x != other.x || y != other.y || z != other.z || w != other.w;
	}
}