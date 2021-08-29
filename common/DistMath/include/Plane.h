/* $Header: /common/distMath/include/Plane.h			8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Plane.h													  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once
#include "DistMathPublic.h"
#include "Box.h"

namespace math
{
	struct Vector4;

	/// Plane in 3D space, implemented using 4D vector
	struct Plane
	{
		Vector4 NormalDistance;

	public:
		enum ESide
		{
			PS_None = 0,
			PS_Front = 1,
			PS_Back = 2,
			PS_Both = PS_Front | PS_Back
		};

	public:
		Plane() {};
		Plane(const Vector4& normal, const Vector4& point);
		Plane(const Vector4& normal, const float& distance);
		Plane(const Vector4& p1, const Vector4& p2, const Vector4& p3);

		void SetPlane(const Vector4& normalAndDistance) { NormalDistance = normalAndDistance; }
		void SetPlane(const Vector4& normal, const Vector4& point);
		void SetPlane(const Vector4& p1, const Vector4& p2, const Vector4& p3);

		float DistanceTo(const Vector4& point) const;
		static float DistanceTo(const Vector4& plane, const Vector4& point);
		ESide GetSide(const Vector4& point) const;
		ESide GetSide(const Box& box) const;
		ESide GetSide(const Vector4& boxCenter, const Vector4& boxExtents) const; 
		const Vector4& GetVectorRepresentation() const { return NormalDistance; }

		Vector4 Project(const Vector4& point) const;
		bool FrontIntersectLine(const Vector4& origin, const Vector4& direction, Vector4& intersectionPoint, float& intersectionDistance) const;
		ESide IntersectLine(const Vector4& origin, const Vector4& direction, Vector4& intersectionPoint, float& intersectionDistance) const;
		static bool IntersectPlanes(const Plane& p0, const Plane& p1, Vector4& outOrigin, Vector4& outDirection);

		bool IsOk() const;

		Plane operator-() const;
	};
}

#include "Plane.cpp"