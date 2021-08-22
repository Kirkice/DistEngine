/* $Header: /common/distMath/include/Uint.h				8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Uint.h													  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/

#pragma once
#include "../../Core/include/PCH.h"
#include "../../Core/include/Assert.h"

namespace math
{
	// Unsigned 32-bit integer vector classes
	struct Uint2
	{
		uint32 x;
		uint32 y;

		Uint2();
		Uint2(uint32 x, uint32 y);

		bool operator==(Uint2 other) const;
		bool operator!=(Uint2 other) const;
	};

	struct Uint3
	{
		uint32 x;
		uint32 y;
		uint32 z;

		Uint3();
		Uint3(uint32 x, uint32 y, uint32 z);

		bool operator==(const Uint3& other) const;
		bool operator!=(const Uint3& other) const;
	};

	struct Uint4
	{
		uint32 x;
		uint32 y;
		uint32 z;
		uint32 w;

		Uint4();
		Uint4(uint32 x, uint32 y, uint32 z, uint32 w);

		bool operator==(const Uint4& other) const;
		bool operator!=(const Uint4& other) const;
	};
}