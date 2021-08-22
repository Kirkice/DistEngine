/* $Header: /common/distMath//include/Half.h			8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Half.h													  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once
#include "../../Core/include/PCH.h"
#include "../../Core/include/Assert.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

namespace math
{
	// Conversion classes for reduced-precision representations
	struct Half2
	{
		uint16 x;
		uint16 y;

		Half2() : x(0), y(0)
		{
		}

		Half2(uint16 x, uint16 y) : x(x), y(y)
		{
		}

		Half2(float x, float y)
		{
			DirectX::PackedVector::XMStoreHalf2(reinterpret_cast<DirectX::PackedVector::XMHALF2*>(this), DirectX::XMVectorSet(x, y, 0.0f, 0.0f));
		}

		explicit Half2(const Vector2& v)
		{
			DirectX::PackedVector::XMStoreHalf2(reinterpret_cast<DirectX::PackedVector::XMHALF2*>(this), v.ToSIMD());
		}

		DirectX::XMVECTOR ToSIMD() const
		{
			return DirectX::PackedVector::XMLoadHalf2(reinterpret_cast<const DirectX::PackedVector::XMHALF2*>(this));
		}

		Vector2 ToVector2() const
		{
			return Vector2(ToSIMD());
		}
	};

	struct Half4
	{
		uint16 x;
		uint16 y;
		uint16 z;
		uint16 w;

		Half4() : x(0), y(0), z(0), w(0)
		{
		}

		Half4(uint16 x, uint16 y, uint16 z, uint16 w) : x(x), y(y), z(z), w(w)
		{
		}

		Half4(float x, float y, float z, float w)
		{
			DirectX::PackedVector::XMStoreHalf4(reinterpret_cast<DirectX::PackedVector::XMHALF4*>(this), DirectX::XMVectorSet(x, y, z, w));
		}

		explicit Half4(const Vector4& v)
		{
			DirectX::PackedVector::XMStoreHalf4(reinterpret_cast<DirectX::PackedVector::XMHALF4*>(this), v.ToSIMD());
		}

		DirectX::XMVECTOR ToSIMD() const
		{
			return DirectX::PackedVector::XMLoadHalf4(reinterpret_cast<const DirectX::PackedVector::XMHALF4*>(this));
		}

		Vector3 ToFloat3() const
		{
			return Vector3(ToSIMD());
		}

		Vector4 ToFloat4() const
		{
			return Vector4(ToSIMD());
		}
	};
}