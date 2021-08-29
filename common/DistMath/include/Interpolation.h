/* $Header: /common/distMath/include/Interpolation.h	   8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Interpolation.h								              *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once
#include "DistMathPublic.h"
#include "vector4.h"
#include "Color.h"

namespace math
{
	template<typename T>
	class Interpolation
	{
	public:
		static T LinearSlow(const T& src, const T& dst, const float t)
		{
			return src * (1.f - t) + dst * t;
		}

		static T Linear(const T& src, const T& dst, const float t)
		{
			return src * (1.f - t) + dst * t;
		}

		static T CubicHermite(const T& p0, const T& t0, const T& t1, const T& p1, const float t)
		{
			const float t_cubed = t * t * t;
			const float t_sq = t * t;
			return p0 * (2.f * t_cubed - 3.f * t_sq + 1.f) + t0 * (t_cubed - 2.f * t_sq + t) + p1 * (-2.f * t_cubed + 3.f * t_sq) + t1 * (t_cubed - t_sq);
		}

		static T QuadraticBezier(const T& p0, const T& c0, const T& p1, const float t)
		{
			const float p0_coef = (1.f - t) * (1.f - t);
			const float c0_coef = 2.f * t * (1.f - t);

			return (p0 * p0_coef) + (c0 * c0_coef) + p1 * (t * t);
		}

		static T CubicBezier(const T& p0, const T& c0, const T& c1, const T& p1, const float t)
		{
			const float p0_coef = (1.f - t) * (1.f - t) * (1.f - t);
			const float c0_coef = 3.f * t * (1.f - t) * (1.f - t);
			const float c1_coef = 3.f * t * t * (1.f - t);

			return (p0 * p0_coef) + (c0 * c0_coef) + (c1 * c1_coef) + p1 * (t * t * t);
		}
	};

	template<>
	struct Interpolation<Vector4>
	{
		static Vector4 LinearSlow(const Vector4& src, const Vector4& dst, const float t);
		static Vector4 Linear(const Vector4& src, const Vector4& dst, const float t);
		static Vector4 CubicHermite(const Vector4& p0, const Vector4& p1, const Vector4& p2, const Vector4& p3, const float t);
		static Vector4 QuadraticBezier(const Vector4& p0, const Vector4& c0, const Vector4& p1, const float t);
		static Vector4 CubicBezier(const Vector4& p0, const Vector4& p1, const Vector4& p2, const Vector4& p3, const float t);
	};

	template<>
	struct  Interpolation<Color>
	{
		static Color LinearSlow(const Color& src, const Color& dst, const float t);
		static Color Linear(const Color& src, const Color& dst, const float t);
		static Color CubicHermite(const Color& p0, const Color& p1, const Color& p2, const Color& p3, const float t);
		static Color QuadraticBezier(const Color& p0, const Color& p1, const Color& p2, const float t);
		static Color CubicBezier(const Color& p0, const Color& p1, const Color& p2, const Color& p3, const float t);
	};

	extern  void Linear_SIMD(float out[4], const float src[4], const float dst[4], const float t[4]);
	extern  void CubicHermite_SIMD(float out[4], const float A[4], const float B[4], const float C[4], const float D[4], const float t[4]);
	extern  void QuadraticBezier_SIMD(float out[4], const float p0[4], const float p1[4], const float p2[4], const float t[4]);
	extern  void CubicBezier_SIMD(float out[4], const float p0[4], const float p1[4], const float p2[4], const float p3[4], const float t[4]);

}