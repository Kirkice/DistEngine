/* $Header: /common/distMath/include/EulerAngles.h	       8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : EulerAngles.h								              *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once
#include "DistMathPublic.h"
#include "Vector4.h"
#include "Matrix.h"
#include "Quaternion.h"

namespace math
{
	struct Matrix;
	struct Quaternion;

	// Euler angles, rotations are CCW, in degrees, order: Y X Z
	struct EulerAngles
	{
		EulerAngles() = default;
		EulerAngles(const EulerAngles& ea);
		EulerAngles(const float roll, const float pitch, const float yaw);
		EulerAngles(const Vector3& v);
		EulerAngles(const float f[3]);

		EulerAngles operator-() const;
		EulerAngles operator+(const EulerAngles& a) const;
		EulerAngles operator-(const EulerAngles& a) const;
		EulerAngles operator*(const EulerAngles& a) const;
		EulerAngles operator/(const EulerAngles& a) const;

		EulerAngles operator+(const float a) const;
		EulerAngles operator-(const float a) const;
		EulerAngles operator*(const float a) const;
		EulerAngles operator/(const float a) const;

		EulerAngles& operator+=(const EulerAngles& a);
		EulerAngles& operator-=(const EulerAngles& a);
		EulerAngles& operator*=(const EulerAngles& a);
		EulerAngles& operator/=(const EulerAngles& a);

		EulerAngles& operator+=(const float a);
		EulerAngles& operator-=(const float a);
		EulerAngles& operator*=(const float a);
		EulerAngles& operator/=(const float a);

		bool operator==(const EulerAngles& a) const;
		bool operator!=(const EulerAngles& a) const;

		bool AlmostEquals(const EulerAngles& a, const float epsilon = 0.01f) const;

		// Normalize angle [0 360]
		static float NormalizeAngle(float angle);
		EulerAngles& Normalize();

		// Normalize angle [-180 180]
		static float NormalizeAngle180(float angle);

		// Converts angle to an angle nearest to referenceAngle by adding/subtracting 360
		static float ToNearestAngle(float angle, const float referenceAngle);

		// Compute yaw angle from various stuff
		static float YawFromXY(const float x, const float y);
		static double YawFromXY(const double x, const double y); 
		static Vector4 YawToVector(const float yaw);
		static Vector2 YawToVector2(const float yaw);

		// Shortest angular distance between two angles (aware of 360 wrap around problem)
		static float AngleDistance(const float a, const float b);

		// Shortest angular distance between two set of Euler angles (aware of 360 wrap around problem)
		static EulerAngles AngleDistance(const EulerAngles& a, const EulerAngles& b);

		// Lame interpolation of angle
		static float Interpolate(const float a, const float b, const float weight);

		// Lame interpolation of Euler angles
		static EulerAngles Interpolate(const EulerAngles& a, const EulerAngles& b, const float weight);

		// Interpolate to a given set of angles
		void Interpolate(const EulerAngles& a, const float weight);

	public:
		// Convert this Euler rotation to matrix form
		Matrix ToMatrix() const;

		// Convert this Euler rotation to matrix form. Alternative version, eliminates the use of the operator=
		void ToMatrix(Matrix& out_matrix) const;

		// Calculate angle vectors
		void ToAngleVectors(Vector4* forward, Vector4* right, Vector4* up) const; 

		// Calculate quaternion
		Quaternion ToQuat() const;

		// Transform point directly by the rotation (SLOW!)
		Vector4 TransformPoint(const Vector4& a) const;

		// Transform vector directly by the rotation (SLOW!)
		Vector4 TransformVector(const Vector4& a) const;

	public:
		// Predefined values
		static EulerAngles ZEROS();

		float Roll = 0.f;			//!< Rotation in degrees around the Y axis
		float Pitch = 0.f;			//!< Rotation in degrees around the X axis
		float Yaw = 0.f;			//!< Rotation in degrees around the Z axis
	};
}