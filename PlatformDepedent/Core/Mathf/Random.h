#pragma once
#include "Vector2.h"
#include <random>

namespace Mathf
{
    // Random number generation
    class Random
    {

    public:

        void Roll(uint32_t numRolls);
        void SeedWithRandomValue();

        uint32_t RandomUint();
        float RandomFloat();
        Vector2 RandomVector2();

    private:

        uint32_t x = 123456789;
        uint32_t y = 987654321;
        uint32_t z = 43219876;
        uint32_t c = 6543217;
    };
}