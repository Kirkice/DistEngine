#include "Random.h"

using namespace DirectX;
using namespace std;

namespace Dist
{
	void Random::Roll(uint32_t numRolls)
	{
		for (uint32_t i = 0; i < numRolls; ++i)
			RandomUint();
	}

	void Random::SeedWithRandomValue()
	{
		random_device device;

		x = device();
		y = device();
		while (y == 0)
			y = device();
		z = device();
		c = device() % 698769068 + 1;
	}

	uint32_t Random::RandomUint()
	{
		x = 314527869 * x + 1234567;
		y ^= y << 5;
		y ^= y >> 7;
		y ^= y << 22;
		uint64_t t = 4294584393ull * z + c;
		c = uint32_t(t >> 32);
		z = uint32_t(t);

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