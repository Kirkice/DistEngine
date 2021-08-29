/* $Header: /common/distMath/include/CutCone.h	           8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : CutCone.h								                  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once
#include "DistMathPublic.h"
#include "Vector4.h"

namespace math
{
	struct CutCone
	{
		Vector4 m_positionAndRadius1; //<! position = (x,y,z), radius1 = (w)
		Vector4 m_normalAndRadius2; //<! orientation = (x,y,z), radius2 = (w)
		float m_height;

		// Undefined constructor
		CutCone() = default; 

		// Copy
		CutCone(const CutCone& cone);

		// Create from two points and two radiuses
		CutCone(const Vector4& pos1, const Vector4& pos2, float radius1, float radius2);

		// Create a cone rooted at position and oriented along specified normal
		CutCone(const Vector4& pos, const Vector4& normal, float radius1, float radius2, float height); //<! normal length has to be 1

		// Return translated by a vector
		CutCone operator+(const Vector4& dir) const;

		// Return translated by a -vector
		CutCone operator-(const Vector4& dir) const;

		// Translate by a vector
		CutCone& operator+=(const Vector4& dir);

		// Translate by a -vector
		CutCone& operator-=(const Vector4& dir);

		// Get center of mass for the cone
		Vector4 GetMassCenter() const;

		// Compute mass (assumes density of 1)
		float GetMass() const;

		// Get cone height
		float GetHeight() const;

		// Get radius at first point
		float GetRadius1() const;

		// Get radius at second point
		float GetRadius2() const;

		// Get root position (base)
		Vector4 GetPosition() const;

		// Get second point (tip)
		Vector4 GetPosition2() const;

		// Get cone axis (normal)
		Vector4 GetNormal() const;

		// Get orientation in space
		EulerAngles GetOrientation() const;

		// Check if shape contains given point
		bool Contains(const Vector4& point) const;

		// Intersect segment with this shape, returns point of entry
		bool IntersectSegment(const Segment& segment, Vector4& enterPoint) const;

		// Intersect ray with this shape, returns distance to entry
		bool IntersectRay(const Vector4& origin, const Vector4& direction, float& enterDistFromOrigin) const;

		// Intersect ray with this shape, returns point of entry
		bool IntersectRay(const Vector4& origin, const Vector4& direction, Vector4& enterPoint) const;
	};
}

#include "cutCone.cpp"