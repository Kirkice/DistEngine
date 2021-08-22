
/* $Header: /common/distMath/src/Matrix.cpp	            8/21/21 18:26p Kirk 			    $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*						Project Name : DistEngine                                             *
*                                                                                             *
*						File Name : Matrix.cpp												  *
*                                                                                             *
*                       Programmer : Kirk                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/

#include "../include/Matrix.h"
#include "../include/DistMathPublic.h"

using namespace DirectX;
using namespace DirectX::PackedVector;

namespace math
{
	Float3x3::Float3x3()
	{
		_11 = _22 = _33 = 1.0f;
		_12 = _13 = 0.0f;
		_21 = _23 = 0.0f;
		_31 = _32 = 0.0f;
	}

	Float3x3::Float3x3(const XMFLOAT3X3& m)
	{
		*reinterpret_cast<XMFLOAT3X3*>(this) = m;
	}

	Float3x3::Float3x3(CXMMATRIX m)
	{
		XMStoreFloat3x3(reinterpret_cast<XMFLOAT3X3*>(this), m);
	}

	Float3x3::Float3x3(const Vector3& r0, const Vector3& r1, const Vector3& r2)
	{
		_11 = r0.x;
		_12 = r0.y;
		_13 = r0.z;

		_21 = r1.x;
		_22 = r1.y;
		_23 = r1.z;

		_31 = r2.x;
		_32 = r2.y;
		_33 = r2.z;
	}

	Float3x3& Float3x3::operator*=(const Float3x3& other)
	{
		XMMATRIX result = this->ToSIMD() * other.ToSIMD();
		XMStoreFloat3x3(reinterpret_cast<XMFLOAT3X3*>(this), result);
		return *this;
	}

	Float3x3 Float3x3::operator*(const Float3x3& other) const
	{
		XMMATRIX result = this->ToSIMD() * other.ToSIMD();
		return Float3x3(result);
	}

	Vector3 Float3x3::Up() const
	{
		return Vector3(_21, _22, _23);
	}

	Vector3 Float3x3::Down() const
	{
		return Vector3(-_21, -_22, -_23);
	}

	Vector3 Float3x3::Left() const
	{
		return Vector3(-_11, -_12, -_13);
	}

	Vector3 Float3x3::Right() const
	{
		return Vector3(_11, _12, _13);
	}

	Vector3 Float3x3::Forward() const
	{
		return Vector3(_31, _32, _33);
	}

	Vector3 Float3x3::Back() const
	{
		return Vector3(-_31, -_32, -_33);
	}

	void Float3x3::SetXBasis(const Vector3& x)
	{
		_11 = x.x;
		_12 = x.y;
		_13 = x.z;
	}

	void Float3x3::SetYBasis(const Vector3& y)
	{
		_21 = y.x;
		_22 = y.y;
		_23 = y.z;
	}

	void Float3x3::SetZBasis(const Vector3& z)
	{
		_31 = z.x;
		_32 = z.y;
		_33 = z.z;
	}

	Float3x3 Float3x3::Transpose(const Float3x3& m)
	{
		return Float3x3(XMMatrixTranspose(m.ToSIMD()));
	}

	Float3x3 Float3x3::Invert(const Float3x3& m)
	{
		XMVECTOR det;
		return Float3x3(XMMatrixInverse(&det, m.ToSIMD()));
	}

	Float3x3 Float3x3::ScaleMatrix(float s)
	{
		Float3x3 m;
		m._11 = m._22 = m._33 = s;
		return m;
	}

	Float3x3 Float3x3::ScaleMatrix(const Vector3& s)
	{
		Float3x3 m;
		m._11 = s.x;
		m._22 = s.y;
		m._33 = s.z;
		return m;
	}

	Float3x3 Float3x3::RotationAxisAngle(const Vector3& axis, float angle)
	{
		return Float3x3(XMMatrixRotationAxis(axis.ToSIMD(), angle));
	}

	Float3x3 Float3x3::RotationEuler(float x, float y, float z)
	{
		return Float3x3(XMMatrixRotationRollPitchYaw(x, y, z));
	}

	XMMATRIX Float3x3::ToSIMD() const
	{
		return XMLoadFloat3x3(reinterpret_cast<const XMFLOAT3X3*>(this));
	}

	Float4x4::Float4x4()
	{
		_11 = _22 = _33 = _44 = 1.0f;
		_12 = _13 = _14 = 0.0f;
		_21 = _23 = _24 = 0.0f;
		_31 = _32 = _34 = 0.0f;
		_41 = _42 = _43 = 0.0f;
	}

	Float4x4::Float4x4(const XMFLOAT4X4& m)
	{
		*reinterpret_cast<XMFLOAT4X4*>(this) = m;
	}

	Float4x4::Float4x4(CXMMATRIX m)
	{
		XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(this), m);
	}

	Float4x4::Float4x4(const Vector4& r0, const Vector4& r1, const Vector4& r2, const Vector4& r3)
	{
		_11 = r0.x;
		_12 = r0.y;
		_13 = r0.z;
		_14 = r0.w;

		_21 = r1.x;
		_22 = r1.y;
		_23 = r1.z;
		_24 = r1.w;

		_31 = r2.x;
		_32 = r2.y;
		_33 = r2.z;
		_34 = r2.w;

		_41 = r3.x;
		_42 = r3.y;
		_43 = r3.z;
		_44 = r3.w;
	}

	Float4x4& Float4x4::operator*=(const Float4x4& other)
	{
		XMMATRIX result = this->ToSIMD() * other.ToSIMD();
		XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(this), result);
		return *this;
	}

	Float4x4 Float4x4::operator*(const Float4x4& other) const
	{
		XMMATRIX result = this->ToSIMD() * other.ToSIMD();
		return Float4x4(result);
	}

	Vector3 Float4x4::Up() const
	{
		return Vector3(_21, _22, _23);
	}

	Vector3 Float4x4::Down() const
	{
		return Vector3(-_21, -_22, -_23);
	}

	Vector3 Float4x4::Left() const
	{
		return Vector3(-_11, -_12, -_13);
	}

	Vector3 Float4x4::Right() const
	{
		return Vector3(_11, _12, _13);
	}

	Vector3 Float4x4::Forward() const
	{
		return Vector3(_31, _32, _33);
	}

	Vector3 Float4x4::Back() const
	{
		return Vector3(-_31, -_32, -_33);
	}

	Vector3 Float4x4::Translation() const
	{
		return Vector3(_41, _42, _43);
	}

	void Float4x4::SetTranslation(const Vector3& t)
	{
		_41 = t.x;
		_42 = t.y;
		_43 = t.z;
	}

	void Float4x4::SetXBasis(const Vector3& x)
	{
		_11 = x.x;
		_12 = x.y;
		_13 = x.z;
	}

	void Float4x4::SetYBasis(const Vector3& y)
	{
		_21 = y.x;
		_22 = y.y;
		_23 = y.z;
	}

	void Float4x4::SetZBasis(const Vector3& z)
	{
		_31 = z.x;
		_32 = z.y;
		_33 = z.z;
	}

	void Float4x4::Scale(const Vector3& scale)
	{
		_11 *= scale.x;
		_12 *= scale.x;
		_13 *= scale.x;

		_21 *= scale.y;
		_22 *= scale.y;
		_23 *= scale.y;

		_31 *= scale.z;
		_32 *= scale.z;
		_33 *= scale.z;
	}

	Float4x4 Float4x4::Transpose(const Float4x4& m)
	{
		return Float4x4(XMMatrixTranspose(m.ToSIMD()));
	}

	Float4x4 Float4x4::Invert(const Float4x4& m)
	{
		XMVECTOR det;
		return Float4x4(XMMatrixInverse(&det, m.ToSIMD()));
	}

	Float4x4 Float4x4::RotationAxisAngle(const Vector3& axis, float angle)
	{
		return Float4x4(XMMatrixRotationAxis(axis.ToSIMD(), angle));
	}

	Float4x4 Float4x4::RotationEuler(float x, float y, float z)
	{
		return Float4x4(XMMatrixRotationRollPitchYaw(x, y, z));
	}

	Float4x4 Float4x4::ScaleMatrix(float s)
	{
		Float4x4 m;
		m._11 = m._22 = m._33 = s;
		return m;
	}

	Float4x4 Float4x4::ScaleMatrix(const Vector3& s)
	{
		Float4x4 m;
		m._11 = s.x;
		m._22 = s.y;
		m._33 = s.z;
		return m;
	}

	Float4x4 Float4x4::TranslationMatrix(const Vector3& t)
	{
		Float4x4 m;
		m.SetTranslation(t);
		return m;
	}

	bool Float4x4::operator==(const Float4x4& other) const
	{
		const float* ours = reinterpret_cast<const float*>(this);
		const float* theirs = reinterpret_cast<const float*>(&other);
		for (uint64 i = 0; i < 16; ++i)
			if (ours[i] != theirs[i])
				return false;
		return true;
	}

	bool Float4x4::operator!=(const Float4x4& other) const
	{
		const float* ours = reinterpret_cast<const float*>(this);
		const float* theirs = reinterpret_cast<const float*>(&other);
		for (uint64 i = 0; i < 16; ++i)
			if (ours[i] != theirs[i])
				return true;
		return false;
	}

	XMMATRIX Float4x4::ToSIMD() const
	{
		return XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(this));
	}

	Float3x3 Float4x4::To3x3() const
	{
		return Float3x3(Right(), Up(), Forward());
	}
}