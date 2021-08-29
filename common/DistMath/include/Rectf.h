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
	// 2D Rectangle, (with left <= right and top <= bottom)
	struct RectF
	{
		float	m_left;
		float	m_top;
		float	m_right;
		float	m_bottom;

		// Empty rectangle
		RectF() = default;

		// Construct from box XY plane, X values growing left to right and Y values growing up to down
		constexpr RectF(const Box& box);

		// Construct
		constexpr RectF(float left, float right, float top, float bottom);

		// Get width
		constexpr float Width() const;

		// Get height
		constexpr float Height() const;

		// Get center point
		Vector2 GetCenter() const;

		// Get the top left corner
		Vector2 GetTopLeft() const;

		// Get the top left corner
		Vector2 GetTopRight() const;

		// Get the bottom right corner
		Vector2 GetBottomRight() const;

		// Get the bottom right corner
		Vector2 GetBottomLeft() const;

		// Check if the rectangle has proper width and height
		constexpr bool IsEmpty() const;

		// Set to invalid/empty state, so you can use Add, AddRect on it.
		void Clear();

		// Check if rectangle intersects with another
		bool Intersects(const RectF& other) const;

		// Check if point is in the rectangle
		bool Intersects(Vector2 pos) const;

		// Get intersection of two rectangles
		static bool Intersection(const RectF& r1, const RectF& r2, RectF& out);

		// Check if rectangle completely contains another
		constexpr bool Contains(const RectF& other) const;

		// Check if rectangle completely contains a point
		constexpr bool Contains(const Vector2& p) const;

		// Expand so it contains other rectangle
		RectF& AddRect(const RectF& other);

		void Add(const Vector2& point);

		void SanityCheck() const;

		// Get bounds of the union of two rectangles.
		// In many code-bases (i.e MS C#) this is called just "union", not "unionBounds" etc, so we also use this naming simplification here.
		static RectF Union(const RectF& r1, const RectF& r2);

		// predefined empty Rect
		constexpr static RectF EMPTY();
	};
}