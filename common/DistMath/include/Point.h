/* $Header: /common/distMath/include/Point.h			8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Point.h													  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once
#include "DistMathPublic.h"

namespace math
{
	// 2D Point
	struct Point
	{
		int32	x;
		int32	y;

		// Empty point, 0,0
		constexpr Point();

		// Construct
		constexpr Point(int32 _x, int32 _y);

		constexpr Point operator-() const;
		constexpr Point operator+(const Point p) const;
		constexpr Point operator-(const Point p) const;
		constexpr Point operator*(const float p) const;
		constexpr Point operator/(const float p) const;

		constexpr bool operator==(const Point p) const; 
		constexpr bool operator!=(const Point p) const;

		Point& operator-=(const Point p);
		Point& operator+=(const Point p);
		Point& operator*=(const float f);
		Point& operator/=(const float f);

		// predefined zero point
		constexpr static Point ZERO();
	};
}

#include "point.cpp"