/* $Header: /common/distMath/include/Color.h	           8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Color.h								                      *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once

#include "DistMathPublic.h"
#include "Vector4.h"
#include "Half.h"

namespace math
{
	struct Color
	{
		union
		{
			struct
			{
				uint8	R, G, B, A;
			};

			uint8	RGBA[4];
		};

		// Empty, uninitialized color
		Color() = default;

		// Create from RGB channels + optional Alpha
		Color(uint8 r, uint8 g, uint8 b, uint8 a = 255);

		// Create from HDR color (NOTE: HDR values are clamped)
		Color(const Vector4& x);
		Color(const float f[4]);

		// Create directly from uint32 color
		explicit Color(uint32 x); 

		// Copy
		bool operator==(const Color& other) const;

		// Inequality
		bool operator!=(const Color& other) const { return R != other.R || G != other.G || B != other.B || A != other.A; }

		// Convert to Vector
		Vector4 ToVector() const;

		// Convert to Vector with gamma->linear conversion
		inline Vector4 ToVectorLinearAccu() const; 
		inline Half4 ToHalfLinearAccu() const;

		// Convert from Vector with linear->gamma conversion
		inline static Color FromVectorLinear(const Vector4& linearColor);
		inline static Color FromVectorLinearAccu(const Vector4& linearColor);

		// Convert to uint32
		uint32 ToUint32() const;

		// Get color with no transparency
		Color FullAlpha() const;

		// Multiply RGB part with scalar, keep Alpha path
		inline static Color Mul3(const Color& a, float b);

		// Multiply all parts by scalar
		inline static Color Mul4(const Color& a, float b);

		// Multiply RGB part with scalar, keep Alpha path
		inline void Mul3(float b);

		// Multiply all parts by scalar
		inline void Mul4(float b);

		// Interpolate two colors
		inline static Color Lerp(float coef, const Color& a, const Color& b);

		inline Color operator*(float f) const;
		inline Color operator+(const Color& c) const;

		// Some predefined colors
		static Color BLACK(uint8 alpha = 255);
		static Color WHITE(uint8 alpha = 255);
		static Color RED(uint8 alpha = 255);
		static Color GREEN(uint8 alpha = 255);
		static Color BLUE(uint8 alpha = 255);
		static Color YELLOW(uint8 alpha = 255);
		static Color CYAN(uint8 alpha = 255);
		static Color MAGENTA(uint8 alpha = 255);
		static Color ORANGE(uint8 alpha = 255);
		static Color LIGHT_RED(uint8 alpha = 255);
		static Color LIGHT_GREEN(uint8 alpha = 255);
		static Color LIGHT_BLUE(uint8 alpha = 255);
		static Color LIGHT_YELLOW(uint8 alpha = 255);
		static Color LIGHT_CYAN(uint8 alpha = 255);
		static Color LIGHT_MAGENTA(uint8 alpha = 255);
		static Color LIGHT_GRAY(uint8 alpha = 255);
		static Color LIGHT_ORANGE(uint8 alpha = 255);
		static Color DARK_RED(uint8 alpha = 255);
		static Color DARK_GREEN(uint8 alpha = 255);
		static Color DARK_BLUE(uint8 alpha = 255);
		static Color DARK_YELLOW(uint8 alpha = 255);
		static Color DARK_CYAN(uint8 alpha = 255);
		static Color DARK_MAGENTA(uint8 alpha = 255);
		static Color DARK_GRAY(uint8 alpha = 255);
		static Color DARK_ORANGE(uint8 alpha = 255);
		static Color BROWN(uint8 alpha = 255);
		static Color GRAY(uint8 alpha = 255);
		static Color NORMAL(uint8 alpha = 255);
		static Color CLEAR();	// name TRANSPARENT doesn't compile yourself
	};
}

#define COLOR_UINT32(R,G,B) (((Uint32)R<<0)|((Uint32)G<<8)|((Uint32)B<<16))

#include "color.cpp"