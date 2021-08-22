/* $Header: /common/distMath/distMathPublic.h	        8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : distMathPublic.h								          *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once

#include "../../Core/include/PCH.h"
#include "../../Core/include/Assert.h"

namespace math
{
	// Constants
	const float DIST_Pi = 3.141592654f;
	const float DIST_Pi2 = 6.283185307f;
	const float DIST_Pi_2 = 1.570796327f;
	const float DIST_Pi_4 = 0.7853981635f;
	const float DIST_InvPi = 0.318309886f;
	const float DIST_InvPi2 = 0.159154943f;

	const float FP16Max = 65000.0f;

	const float FP16Scale = 0.0009765625f;

	// 插值
	template<typename T> T Lerp(const T& x, const T& y, float s)
	{
		return x + (y - x) * s;
	}

	//最小值
	template<typename T> T Min(T a, T b)
	{
		return a < b ? a : b;
	}

	//最大值
	template<typename T> T Max(T a, T b)
	{
		return a < b ? b : a;
	}

	// Clamps在 min   max
	template<typename T> T Clamp(T val, T min, T max)
	{
		Assert_(max >= min);

		if (val < min)
			val = min;
		else if (val > max)
			val = max;
		return val;
	}

	// Satuate clamp 0  1
	template<typename T> T Saturate(T val)
	{
		return Clamp<T>(val, T(0.0f), T(1.0f));
	}

	// 平方
	template<typename T> T Square(T x)
	{
		return x * x;
	}

	//弧度 < - > °
	inline float DEG2RAD(const float x)
	{
		return ((x / 180.f) * DIST_Pi);
	}

	inline float DEG2RAD(const double x)
	{
		return ((x / 180) * (double)DIST_Pi);
	}

	inline float RAD2DEG(const float x)
	{
		return ((x / DIST_Pi) * 180.0f);
	}

	inline float RAD2DEG(const double x)
	{
		return ((x / (double)DIST_Pi) * 180.0);
	}

	inline float DEG2RAD_HALF(const float x)
	{
		return ((x * DIST_Pi) / 360.0f);
	}

	inline float DEG2RAD_HALF(const double x)
	{
		return ((x * (double)DIST_Pi) / 360.0);
	}
}