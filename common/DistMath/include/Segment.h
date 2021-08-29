/* $Header: /common/distMath/include/Segment.h			   8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Segment.h												  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once
#include "DistMathPublic.h"
#include "Point.h"
#include "Box.h"

namespace math
{
	struct Vector4;

	// 3D segment first point is stored in m_origin, second point is m_origin + m_direction (it's more convenient for most of the computations)
	struct Segment
	{
		Vector4 m_origin;
		Vector4 m_direction;

		Segment() = default;
		Segment(const Segment& other);
		Segment(const Vector4& origin, const Vector4& direction);

		// Return translated by a vector
		Segment operator+(const Vector4& dir) const;

		// Return translated by a -vector
		Segment operator-(const Vector4& dir) const;

		// Translate by a vector
		void operator+=(const Vector4& dir);

		// Translate by a -vector
		void operator-=(const Vector4& dir);

		// Get segment starting position
		Vector4 GetPosition() const;

		// Calculate shortest distance between two segments
		static float SegmentDistance(const Segment& s0, const Segment& s1, float& t0, float& t1);
	};
}