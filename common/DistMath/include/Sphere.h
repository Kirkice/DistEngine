/* $Header: /common/distMath/include/Sphere.h			   8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Sphere.h												  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once
#include "DistMathPublic.h"
#include "Vector4.h"
#include "EulerAngles.h"

namespace math
{
	// 3D sphere
	struct Sphere
	{
		Vector4	CenterRadius;

		Sphere();
		Sphere(const Vector4& geometry);
		Sphere(const Vector4& center, float radius);
		Sphere(float c0, float c1, float c2, float radius);

		// Get sphere's ceneter
		Vector4 GetCenterOfVolume() const { return CenterRadius; }

		// Get sphere's volume
		float GetVolume() const { return 4 * DIST_Pi * CenterRadius.w * CenterRadius.w * CenterRadius.w / 3; }

		// Return translated by a vector
		Sphere operator+(const Vector4& dir) const;

		// Return translated by a -vector
		Sphere operator-(const Vector4& dir) const;

		// Translate by a vector
		void operator+=(const Vector4& dir);

		// Translate by a -vector
		void operator-=(const Vector4& dir);

		// Get generalized orientation (added to all shapes)
		EulerAngles GetOrientation() const { return EulerAngles::ZEROS(); }

		// Gets sphere's center position
		Vector4 GetCenter() const;

		// Gets square of sphere's radius
		float GetSquareRadius() const;

		// Gets sphere's radius
		float GetRadius() const;

		// Gets shortest distance to point (==0 if point lies on sphere's boundary, <0 if point is inside sphere)
		float GetDistance(const Vector4& point) const;

		// Gets shortest distance from this sphere's boundary to another sphere's boundary (<0 if spheres intersect)
		float GetDistance(const Sphere& sphere) const;

		// Check if sphere contains point
		bool Contains(const Vector4& point) const;

		// Check if sphere wholly contains other sphere
		bool Contains(const Sphere& sphere) const;

		// Check if sphere touches other sphere
		bool Touches(const Sphere& sphere) const;

		// Check ray-sphere intersection; returns the number of intersection points (0, 1 or 2); calculated enterPoint is clamped to origin
		uint32 IntersectRay(const Vector4& origin, const Vector4& direction, Vector4& enterPoint, Vector4& exitPoint) const;

		// Check edge-sphere intersection; returns number of intersection points (0, 1 or 2); calculated intersection points are clamped within a and b
		uint32 IntersectEdge(const Vector4& a, const Vector4& b, Vector4& intersectionPoint0, Vector4& intersectionPoint1) const;

		// Check line-sphere intersection; returns the number of intersection points (0, 1 or 2);
		// t1 and t2 are parameters of intersection points for line equation 'origin + t * direction' (direction does not need to be normalized!)
		uint32 IntersectLineParametric(const Vector4& origin, const Vector4& direction, float& out_t1, float& out_t2) const;

		// Inflates the sphere so it contains the given point and previous sphere. Results in the smallest possible;
		void AddPoint(const Vector4& point);
	};
}