
/* $Header: /common/distMath/src/Random.cpp	            8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Random.cpp												  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/

#include "../include/Random.h" 
#include "../include/Vector2.h"

using namespace DirectX;
using namespace DirectX::PackedVector;

namespace math
{
	void Random::Roll(uint32 numRolls)
	{
		for (uint32 i = 0; i < numRolls; ++i)
			RandomUint();
	}

	void Random::SeedWithRandomValue()
	{
		std::random_device device;

		x = device();
		y = device();
		while (y == 0)
			y = device();
		z = device();
		c = device() % 698769068 + 1;
	}

	uint32 Random::RandomUint()
	{
		x = 314527869 * x + 1234567;
		y ^= y << 5;
		y ^= y >> 7;
		y ^= y << 22;
		uint64 t = 4294584393ull * z + c;
		c = uint32(t >> 32);
		z = uint32(t);

		return x + y + z;
	}

	float Random::RandomFloat()
	{
		return RandomUint() / 4294967296.0f;
	}

	Vector2 Random::RandomVector2()
	{
		return Vector2(RandomFloat(), RandomFloat());
	}
}