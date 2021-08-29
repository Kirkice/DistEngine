/* $Header: /common/distMath/include/Point3D.h			8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Point3D.h												  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once
#include "DistMathPublic.h"

namespace math
{
	// 3D Point3D
	struct Point3D
	{
		int32	x;
		int32	y;
		int32	z;

		// Empty point, 0,0,0
		constexpr Point3D();

		// Construct
		constexpr Point3D(int32 _x, int32 _y, int32 _z);

		constexpr Point3D operator-() const;
		constexpr Point3D operator+(const Point3D& p) const;
		constexpr Point3D operator-(const Point3D& p) const;
		constexpr Point3D operator*(const float f) const;
		constexpr Point3D operator/(const float f) const;

		constexpr bool operator==(const Point3D& p) const;
		constexpr bool operator!=(const Point3D& p) const;

		Point3D& operator-=(const Point3D& p);
		Point3D& operator+=(const Point3D& p);
		Point3D& operator*=(const float f);
		Point3D& operator/=(const float f);

		uint32 CalcHash() const;

		// predefined zero point
		constexpr static Point3D ZERO();
	};
}

#include "Point3D.hpp"