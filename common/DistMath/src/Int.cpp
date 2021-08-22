
/* $Header: /common/distMath/src/Int.cpp	            8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Int.cpp													  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/

#include "../include/Int.h"
#include "../include/DistMathPublic.h"

using namespace DirectX;
using namespace DirectX::PackedVector;

namespace math
{
	// == Int2 =======================================================================================

	Int2::Int2() : x(0), y(0)
	{
	}

	Int2::Int2(int32 x_, int32 y_) : x(x_), y(y_)
	{
	}

	bool Int2::operator==(Int2 other) const
	{
		return x == other.x && y == other.y;
	}

	bool Int2::operator!=(Int2 other) const
	{
		return x != other.x || y != other.y;
	}

	// == Int3 =======================================================================================

	Int3::Int3() : x(0), y(0), z(0)
	{
	}

	Int3::Int3(int32 x_, int32 y_, int32 z_) : x(x_), y(y_), z(z_)
	{
	}

	// == Int4 =======================================================================================

	Int4::Int4() : x(0), y(0), z(0), w(0)
	{
	}

	Int4::Int4(int32 x_, int32 y_, int32 z_, int32 w_) : x(x_), y(y_), z(z_), w(w_)
	{
	}
}