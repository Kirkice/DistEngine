/* $Header: /common/distMath/include/FixedCapsule.h	       8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : FixedCapsule.h								              *
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
	/********************************/
	/* FixedCapsule					*/
	/*	__							*/
	/* /  \							*/
	/* |  |	<--- Point B			*/
	/* |  |							*/
	/* |  |	<--- Point A			*/
	/* \__/							*/
	/*	 <--- Position				*/
	/********************************/

	// Upright standing capsule defined via position, radius and height, height doesn't include two caps
	struct FixedCapsule
	{
		Vector4 PointRadius;
		float Height;

		// Undefined constructor
		FixedCapsule() = default;

		// Define via capsule point (base), radius and height
		FixedCapsule(const Vector4& point, float radius, float height);

		// Get center of mass (center of capsule)
		Vector4 GetMassCenter() const;

		// Get mass, assuming density 1.0f 
		float GetMass() const;

		// Change parameters
		void Set(const Vector4& point, float radius, float height); 

		// Gets capsule's position
		Vector4 GetPosition() const;

		// Get generalized orientation (zero in case of capsules)
		EulerAngles GetOrientation() const;

		// Calc capsule's start position
		Vector4 CalcPointA() const;

		// Calc capsule's end position
		Vector4 CalcPointB() const;

		// Gets capsule's radius
		float GetRadius() const;

		// Gets capsule's height
		float GetHeight() const;

		// Return box translated by a vector
		FixedCapsule operator+(const Vector4& dir) const;

		// Return box translated by a -vector
		FixedCapsule operator-(const Vector4& dir) const;

		// Translate by a vector
		FixedCapsule& operator+=(const Vector4& dir);

		// Translate by a -vector
		FixedCapsule& operator-=(const Vector4& dir);

		// Check if capsule contains point
		bool Contains(const Vector4& point) const;

		// Check if sphere contains sphere
		bool Contains(const Sphere& sphere) const;
	};
}

#include "fixedCapsule.cpp"