/* $Header: /common/distMath/include/Box.h	            8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Box.h								                      *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once

#include "DistMathPublic.h"
#include "Vector4.h"

namespace math
{
	struct Sphere;
	struct Segment;
	struct Box
	{
		Vector4	Min;
		Vector4	Max;

		enum EResetState { RESET_STATE };
		enum EMaximize { MAXIMIZE };

		enum ECorner
		{
			xyz = 0,
			xYz,
			XYz,
			Xyz,
			xyZ,
			xYZ,
			XYZ,
			XyZ
		};

		Box() = default;
		Box(const Box & rhs);
		Box(const Vector4 & min, const Vector4 & max);
		Box(const Vector4 & center, float radius);
		Box(EResetState);
		Box(EMaximize);

		bool operator==(const Box & box) const;
		bool operator!=(const Box & box) const;
		bool operator<(const Box & box) const;

		Box& operator=(const Box & rhs) = default;

		// Return box translated by a vector
		Box operator+(const Vector4 & dir) const;

		// Return box translated by a -vector
		Box operator-(const Vector4 & dir) const;

		// Return box scaled box by a vector
		Box operator*(const Vector4 & scale) const;

		// Translate by a vector
		void operator+=(const Vector4 & dir);

		// Translate by a -vector
		void operator-=(const Vector4 & dir);

		// Scale box by a vector
		void operator*=(const Vector4 & scale);

		// Clear to empty box
		Box& Clear();

		// Check if bounding box contains point
		bool Contains(const Vector4 & point) const;

		// Check if bounding box contains point
		bool Contains(const Vector3 & point, float zExt) const;

		// Check if bounding box contains other box
		bool Contains(const Box & box) const;

		// Check if bounding box contains other box, excluding edges
		bool ContainsExcludeEdges(const Box & box) const;

		// Check if bounding box contains other box in 2D (checks only X and Y)
		bool Contains2D(const Box & box) const;

		// Check if bounding box contains point
		bool Contains2D(const Vector3 & point) const;

		// Check if bounding box contains point, excluding edges
		bool ContainsExcludeEdges(const Vector4 & point) const;

		// Check if bounding box touches other box
		bool Touches(const Box & box) const;

		// Check if bounding box touches other box defined with min and max points
		bool Touches(const Vector3 & bMin, const Vector3 & bMax) const;

		// Check if bounding box touches other box; only takes X and Y in consideration
		bool Touches2D(const Box & box) const;

		// Add point to bounding box
		Box& AddPoint(const Vector4 & point);

		// Add point to bounding box
		Box& AddPoint(const Vector3 & point);

		// Add other box to bounding box
		Box& AddBox(const Box & box);

		// Check if bounding box is empty
		// (note that if the bounding box is valid, but at least one dimension is equal to zero, it's not empty then)
		bool IsEmpty() const;
		bool IsOk() const;

		// Calculate box corners
		void CalcCorners(Vector4 * corners) const;

		Vector4 CalcCorner(ECorner corner) const;

		// Calculate box center
		Vector4 CalcCenter() const;

		// Calculate box extents
		Vector4 CalcExtents() const;

		// Calculate box size
		Vector4 CalcSize() const;

		// Calculate box volume
		float CalcVolume() const;

		// Extrude by a direction
		Box& Extrude(const Vector4 & dir);

		// Extrude by a value
		Box& Extrude(const float value);

		// Expand evenly by a direction
		Box& Expand(const Vector4 & dir);

		// Expand evenly by a value
		Box& Expand(const float value);

		// Crop to box
		void Crop(const Box & box);

		// Normalize to unit box space
		Box& Normalize(const Box & unitBox);

		// Distance from point
		float Distance(const Vector4 & pos) const;

		// Squared distance from point
		float SquaredDistance(const Vector4 & pos) const;

		// Squared distance from point; only takes X and Y in consideration
		float SquaredDistance2D(const Vector4 & pos) const;

		// Squared distance from other box
		float SquaredDistance(const Box & box) const;

		// Distance between a point and the farthest point of the box
		float FarthestPointDistance(const Vector4 & pos) const;

		// Squared distance between a point and the farthest point of the box
		float FarthestPointDistanceSquared(const Vector4 & pos) const;

		// Unit Box expanding from 0,0,0 to 1,1,1
		static Box UNIT();

		// Empty Box
		static Box EMPTY();

		// Full Box (contains everything)
		static Box FULL();

	public:
		// Check if the bounding box intersects sphere (Arvo's algorithm)
		bool IntersectSphere(const Sphere & sphere) const;

		// Check if the segment intersects the bounding box 
		bool IntersectSegment(const Segment & segment, Vector4 & enterPoint) const;

		// Check if the segment intersects the bounding box (returns both entry and exit point)
		bool IntersectSegment(const Segment & segment, Vector4 & enterPoint, Vector4 & exitPoint) const;

		// Check if the infinite line intersects the bounding box, returns enter and exit distance on the ray (possibly negative)
		bool IntersectLine(const Vector4 & origin, const Vector4 & direction, float & enterDistFromOrigin, float & exitDistFromOrigin) const;

		// Check if the ray intersects the bounding box, returns enter and exit distance on the ray
		bool IntersectRay(const Vector4 & origin, const Vector4 & direction, float & enterDistFromOrigin, float * exitDistFromOrigin = nullptr) const;

		// Check if the ray intersects the bounding box
		bool IntersectRay(const Vector4 & origin, const Vector4 & direction, Vector4 & enterPoint) const;
	};
}

#include "box.cpp"