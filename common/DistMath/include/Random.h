/* $Header: /common/distMath/include/Random.h	           8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Random.h								                  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/

#pragma once
#include "../../Core/include/PCH.h"
#include "../../Core/include/Assert.h"
#include "Vector2.h"

namespace math
{
	// Random number generation
	class Random
	{

	public:

		void Roll(uint32 numRolls);
		void SeedWithRandomValue();

		uint32 RandomUint();
		float RandomFloat();
		Vector2 RandomVector2();

	private:

		uint32 x = 123456789;
		uint32 y = 987654321;
		uint32 z = 43219876;
		uint32 c = 6543217;
	};
}
