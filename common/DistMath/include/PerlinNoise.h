/* $Header: /common/distMath/include/PerlinNoise.h	       8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : PerlinNoise.h								              *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once
#include "DistMathPublic.h"

namespace math
{
	class PerlinNoise
	{
	private:

		int32 p[512];

		float Fade(float t) const
		{
			return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
		}

		float Grad(int32 hash, float x, float y, float z) const
		{
			const int32 h = hash & 15;
			const float u = h < 8 ? x : y;
			const float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
			return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
		}

	public:

		explicit PerlinNoise(uint32 seed);

		void Seed(uint32 seed);


		// @return value [ -1 ; 1 ]
		float Get(float x) const
		{
			return Get(x, 0.0f, 0.0f);
		}

		// @return value [ -1 ; 1 ]
		float Get(float x, float y) const
		{
			return Get(x, y, 0.0f);
		}

		// @return value [ -1 ; 1 ]
		float Get(float x, float y, float z) const; 

		// @return value [ -1 ; 1 ]
		float GetOctave(float x, int32 octaves) const;
		float GetOctave(float x, float y, int32 octaves) const;
		float GetOctave(float x, float y, float z, int32 octaves) const;
	};
}
