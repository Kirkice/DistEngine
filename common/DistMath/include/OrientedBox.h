/* $Header: /common/distMath/include/OrientedBox.h	       8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : OrientedBox.h								              *
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
	// 3D OBB
	struct OrientedBox
	{
		Vector4 m_position;	//<! position, m_position.W is length of third edge
		Vector4 m_edge1;	//<! forward, edge1 - normalized, length in w component
		Vector4 m_edge2;	//<! right, edge2 - normalized, length in w component
							//<! up, edge3 is m_edge1 x m_edge2 * m_position.W
		// Undefined constructor
		OrientedBox() = default;

		// Copy
		OrientedBox(const OrientedBox& box);

		// Create from root position and set of direction
		OrientedBox(const Vector4& pos, const Vector4& forward, const Vector4& right, const Vector4& up);

		// Get base edge 1
		Vector4 GetEdge1() const;

		// Get base edge 2
		Vector4 GetEdge2() const;

		// Get height edge 
		Vector4 GetEdge3() const;

		// Get (compute) orientation
		EulerAngles GetOrientation() const;

		// Get center of mass
		Vector4 GetMassCenter() const;

		// Get mass (assumes density of 1)
		float GetMass() const;

		// Return translated by a vector
		OrientedBox operator+(const Vector4& dir) const;

		// Return translated by a -vector
		OrientedBox operator-(const Vector4& dir) const;

		// Translate by a vector
		OrientedBox& operator+=(const Vector4& dir);

		// Translate by a -vector
		OrientedBox& operator-=(const Vector4& dir);

		// Check if the box is empty 
		bool IsEmpty() const;

		// Check if bounding box contains point
		bool Contains(const Vector4& point) const;

		// Intersect segment with this oriented box, returns point of entry
		bool IntersectSegment(const Segment& segment, Vector4& enterPoint) const;

		// Intersect ray with this oriented box, returns distance to entry point
		bool IntersectRay(const Vector4& origin, const Vector4& direction, float& enterDistFromOrigin) const; 

		// Intersect ray with this oriented box, returns point of entry
		bool IntersectRay(const Vector4& origin, const Vector4& direction, Vector4& enterPoint) const;
	};
}

#include "OrientedBox.cpp"