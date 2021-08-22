/* $Header: /common/distMath//include/UByte.h			8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : UByte.h													  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once
#include "../../Core/include/PCH.h"
#include "../../Core/include/Assert.h"
#include "Vector4.h"

namespace math
{
	struct UByte4N
	{
		uint32 Bits;

		UByte4N() : Bits(0)
		{
		}

		explicit UByte4N(uint32 bits) : Bits(bits)
		{
		}

		UByte4N(uint8 x, uint8 y, uint8 z, uint8 w)
		{
			Bits = x | (y << 8) | (z << 16) | (w << 14);
		}

		UByte4N(float x, float y, float z, float w)
		{
			DirectX::PackedVector::XMStoreUByteN4(reinterpret_cast<DirectX::PackedVector::XMUBYTEN4*>(this), DirectX::XMVectorSet(x, y, z, w));
		}

		explicit UByte4N(const Vector4& v)
		{
			DirectX::PackedVector::XMStoreUByteN4(reinterpret_cast<DirectX::PackedVector::XMUBYTEN4*>(this), v.ToSIMD());
		}

		DirectX::XMVECTOR ToSIMD() const
		{
			return DirectX::PackedVector::XMLoadUByteN4(reinterpret_cast<const DirectX::PackedVector::XMUBYTEN4*>(this));
		}

		Vector4 ToFloat4() const
		{
			return Vector4(ToSIMD());
		}
	};

	struct UShort4N
	{
		uint64 Bits;

		UShort4N() : Bits(0)
		{
		}

		explicit UShort4N(uint32 bits) : Bits(bits)
		{
		}

		UShort4N(uint16 x, uint16 y, uint16 z, uint16 w)
		{
			Bits = x | (y << 8ull) | (z << 16ull) | (w << 14ull);
		}

		UShort4N(float x, float y, float z, float w)
		{
			DirectX::PackedVector::XMStoreUShortN4(reinterpret_cast<DirectX::PackedVector::XMUSHORTN4*>(this), DirectX::XMVectorSet(x, y, z, w));
		}

		explicit UShort4N(const Vector4& v)
		{
			DirectX::PackedVector::XMStoreUShortN4(reinterpret_cast<DirectX::PackedVector::XMUSHORTN4*>(this), v.ToSIMD());
		}

		DirectX::XMVECTOR ToSIMD() const
		{
			return DirectX::PackedVector::XMLoadUShortN4(reinterpret_cast<const DirectX::PackedVector::XMUSHORTN4*>(this));
		}

		Vector4 ToFloat4() const
		{
			return Vector4(ToSIMD());
		}
	};
}