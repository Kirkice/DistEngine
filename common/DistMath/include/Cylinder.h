/* $Header: /common/distMath/include/Cylinder.h	           8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Cylinder.h								                  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once
#include "DistMathPublic.h"
#include "Vector4.h"
#include "EulerAngles.h"
#include "Segment.h"

namespace math
{
	// 3D oriented cylinder
	struct Cylinder
	{
		Vector4 m_positionAndRadius; //<! position = (x,y,z), radius = (w)
		Vector4 m_normalAndHeight;   //<! orientation = (x,y,z), height = (w)

		// Undefined constructor
		Cylinder() = default;

		// Copy
		Cylinder(const Cylinder& cyl);

		// Create cylinder between two positions with given radius
		Cylinder(const Vector4& pos1, const Vector4& pos2, float radius);

		// Create cylinder at given position, pointing along the normal vector with given radius and height
		Cylinder(const Vector4& pos, const Vector4& normal, float radius, float height); //<! normal length has to be normalized ;]

		// Get center of mass for the cylinder
		Vector4 GetMassCenter() const;

		// Compute mass (assumes density of 1)
		float GetMass() const;

		// Get cylinder orientation
		EulerAngles GetOrientation() const;

		// Return translated by a vector
		Cylinder operator+(const Vector4& dir) const;

		// Return translated by a -vector
		Cylinder operator-(const Vector4& dir) const;

		// Translate by a vector
		Cylinder& operator+=(const Vector4& dir);

		// Translate by a -vector
		Cylinder& operator-=(const Vector4& dir);

		// Get height of the cylinder
		float GetHeight() const;

		// Get radius of the cylinder
		float GetRadius() const;

		// Get position at the bottom side of the cylinder
		Vector4 GetPosition() const;

		// Get position at the top side of the cylinder
		Vector4 GetPosition2() const;

		// Get cylinder normal vector (axis of the cylinder)
		Vector4 GetNormal() const;

		// Check if cylinder contains given point
		bool Contains(const Vector4& point) const;

		// Intersect a segment with this cylinder, returns point of entry
		bool IntersectSegment(const Segment& segment, Vector4& enterPoint) const;

		// Intersect ray with this cylinder, returns distance to entry
		bool IntersectRay(const Vector4& origin, const Vector4& direction, float& enterDistFromOrigin) const;

		// Intersect ray with this cylinder, returns point of entry
		bool IntersectRay(const Vector4& origin, const Vector4& direction, Vector4& enterPoint) const;
	};
}
#include "../src/Cylinder.cpp"