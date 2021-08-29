/* $Header: /common/distMath/include/Rect.h			    8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Rect.h												      *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once
#include "DistMathPublic.h"
#include "Point.h"
#include "Box.h"

namespace math
{
	// 2D Rectangle (with left < right and top < bottom)
	struct Rect
	{
		enum EResetState { RESET_STATE };

		int32	m_left;
		int32	m_top;
		int32	m_right;
		int32	m_bottom;

		// Empty rectangle
		Rect() = default;

		// Construct from box XY plane, X values growing left to right and Y values growing up to down
		constexpr Rect(const Box& box);

		// Construct
		constexpr Rect(int32 left, int32 right, int32 top, int32 bottom);

		// Construct from two corners
		constexpr Rect(const Point& tl, const Point& br);

		// Construct an empty rectangle
		Rect(EResetState);

		// All zeroes
		void Clear();

		// Check if rectangle is empty
		constexpr bool IsEmpty() const;

		// Translate all rectangle by x,y
		void Translate(int32 x, int32 y);
		void Translate(const Point& translation);

		// Get translated rectangle
		constexpr Rect GetTranslated(int32 x, int32 y) const;
		constexpr Rect GetTranslated(const Point& translation) const;

		// Trim to another rectangle
		void Trim(const Rect& trimmerRect);

		// Get trimmed to another rectangle
		constexpr Rect GetTrimmed(const Rect& trimmerRect) const;

		// Grow rect by sx, sy in every direction
		void Grow(int32 sx, int32 sy);

		// Get grown rect by sx, sy in every direction
		constexpr Rect GetGrown(int32 sx, int32 sy) const;

		// Add rect to this rect, to get combined bounds
		void Add(const Rect& addRect);

		void Add(const Point& point);

		void Add(const Vector2& point);

		// Check if rectangle intersects with another
		bool Intersects(const Rect& other) const;

		// Check if rectangle completely contains another
		constexpr bool Contains(const Rect& other) const;

		// Check if rectangle completely contains a point
		constexpr bool Contains(const Point& p) const;

		// Check if rectangle completely contains a point
		constexpr bool Contains(const Vector2& p) const;

		// Get width
		constexpr int32 Width() const;

		// Get height
		constexpr int32 Height() const;

		// Get the top left corner
		constexpr Point GetTopLeft() const;

		// Get the top left corner
		constexpr Point GetTopRight() const;

		// Get the bottom right corner
		constexpr Point GetBottomRight() const;

		// Get the bottom right corner
		constexpr Point GetBottomLeft() const;

		// Get center of the rectangle
		constexpr Point GetCenter() const;

		// Get intersection of two rectangles
		static bool Intersection(const Rect& r1, const Rect& r2, Rect& out);

		// Get bounds of the union of two rectangles.
		// In many code-bases (i.e MS C#) this is called just "union", not "unionBounds" etc, so we also use this naming simplification here.
		static Rect Union(const Rect& r1, const Rect& r2);

		// predefined empty Rect 
		constexpr static Rect EMPTY();

		// Comparison operators.
		bool operator==(const Rect& other) const;
		bool operator!=(const Rect& other) const;
	};
}