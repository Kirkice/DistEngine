
/* $Header: /common/distMath/include/Matrix.cpp	           8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Matrix.cpp												  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#pragma once

#include "../../Core/include/PCH.h"
#include "../../Core/include/Assert.h"
#include "Vector3.h"
#include "Vector4.h"

namespace math
{
	struct Float3x3
	{
		float _11, _12, _13;
		float _21, _22, _23;
		float _31, _32, _33;

		Float3x3();
		explicit Float3x3(const DirectX::XMFLOAT3X3& m);
		explicit Float3x3(DirectX::CXMMATRIX m);
		Float3x3(const Vector3& r0, const Vector3& r1, const Vector3& r2);

		Float3x3& operator*=(const Float3x3& other);
		Float3x3 operator*(const Float3x3& other) const;

		Vector3 Up() const;
		Vector3 Down() const;
		Vector3 Left() const;
		Vector3 Right() const;
		Vector3 Forward() const;
		Vector3 Back() const;

		void SetXBasis(const Vector3& x);
		void SetYBasis(const Vector3& y);
		void SetZBasis(const Vector3& z);

		static Float3x3 Transpose(const Float3x3& m);
		static Float3x3 Invert(const Float3x3& m);
		static Float3x3 ScaleMatrix(float s);
		static Float3x3 ScaleMatrix(const Vector3& s);
		static Float3x3 RotationAxisAngle(const Vector3& axis, float angle);
		static Float3x3 RotationEuler(float x, float y, float z);

		DirectX::XMMATRIX ToSIMD() const;
	};

	struct Float4x4
	{
		float _11, _12, _13, _14;
		float _21, _22, _23, _24;
		float _31, _32, _33, _34;
		float _41, _42, _43, _44;

		Float4x4();
		explicit Float4x4(const DirectX::XMFLOAT4X4& m);
		explicit Float4x4(DirectX::CXMMATRIX m);
		Float4x4(const Vector4& r0, const Vector4& r1, const Vector4& r2, const Vector4& r3);

		Float4x4& operator*=(const Float4x4& other);
		Float4x4 operator*(const Float4x4& other) const;

		Vector3 Up() const;
		Vector3 Down() const;
		Vector3 Left() const;
		Vector3 Right() const;
		Vector3 Forward() const;
		Vector3 Back() const;

		Vector3 Translation() const;
		void SetTranslation(const Vector3& t);

		void SetXBasis(const Vector3& x);
		void SetYBasis(const Vector3& y);
		void SetZBasis(const Vector3& z);

		void Scale(const Vector3& scale);

		static Float4x4 Transpose(const Float4x4& m);
		static Float4x4 Invert(const Float4x4& m);
		static Float4x4 RotationAxisAngle(const Vector3& axis, float angle);
		static Float4x4 RotationEuler(float x, float y, float z);
		static Float4x4 ScaleMatrix(float s);
		static Float4x4 ScaleMatrix(const Vector3& s);
		static Float4x4 TranslationMatrix(const Vector3& t);

		bool operator==(const Float4x4& other) const;
		bool operator!=(const Float4x4& other) const;

		DirectX::XMMATRIX ToSIMD() const;
		Float3x3 To3x3() const;
	};
}