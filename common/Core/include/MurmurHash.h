/* $Header: /common/Core/include/MurmurHash.h			8/25/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : MurmurHash.h											  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once
#include "../../Core/include/PCH.h"

namespace core
{
	struct Hash
	{
		uint64 A;
		uint64 B;

		Hash() : A(0), B(0) {}
		Hash(uint64 a, uint64 b) : A(a), B(b) {}

		std::wstring ToString() const;

		bool operator==(const Hash& other)
		{
			return A == other.A && B == other.B;
		}
	};

	Hash GenerateHash(const void* key, int32 len, uint32 seed = 0);
	Hash CombineHashes(Hash a, Hash b);
}