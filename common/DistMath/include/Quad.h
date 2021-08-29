/* $Header: /common/distMath/include/Quad.h			    8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Quad.h												      *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once
#include "DistMathPublic.h"
#include "Vector4.h"

namespace math
{
	struct Quad
	{
	public:
		Vector4 m_points[4];

		// Create initialized
		Quad() = default;

		// Copy
		Quad(const Quad& other);

		// Create from points
		Quad(const Vector4& p1, const Vector4& p2, const Vector4& p3, const Vector4& p4);

		// Get root position (assumed to be at vertex 0)
		Vector4 GetPosition() const;

		// Return translated by a vector
		Quad operator+(const Vector4& dir) const;

		// Return translated by a -vector
		Quad operator-(const Vector4& dir) const;

		// Translate by a vector
		Quad& operator+=(const Vector4& dir);

		// Translate by a -vector
		Quad& operator-=(const Vector4& dir);

		void SanityCheck() const;

		// Check if ray intersect quad: 0 not found, 1 enters and exits, 2 only exits (enterPoint same as origin)
		bool IntersectSegment(const Segment& segment, Vector4& enterPoint);

		// Check if ray intersects quad: same as above but returns distance from origin along the ray
		bool IntersectRay(const Vector4& origin, const Vector4& direction, float& enterDistFromOrigin) const;

		// Check edge-quad intersection, returns number of intersection points
		bool IntersectRay(const Vector4& origin, const Vector4& direction, Vector4& enterPoint) const; 
	};
}

#include "Quad.cpp"