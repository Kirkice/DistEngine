/* $Header: /common/distMath/include/Int.h				8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Int.h													  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once
#include "../../Core/include/PCH.h"
#include "../../Core/include/Assert.h"

namespace math
{
	// Signed 32-bit integer vector classes
	struct Int2
	{
		int32 x;
		int32 y;

		Int2();
		Int2(int32 x, int32 y);

		bool operator==(Int2 other) const;
		bool operator!=(Int2 other) const;
	};

	struct Int3
	{
		int32 x;
		int32 y;
		int32 z;

		Int3();
		Int3(int32 x, int32 y, int32 z);
	};

	struct Int4
	{
		int32 x;
		int32 y;
		int32 z;
		int32 w;

		Int4();
		Int4(int32 x, int32 y, int32 z, int32 w);
	};
}
