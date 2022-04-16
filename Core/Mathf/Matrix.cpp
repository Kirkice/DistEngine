#include "Matrix.h"

using namespace DirectX;

namespace Dist
{
	float3x3::float3x3()
	{
		_11 = _22 = _33 = 1.0f;
		_12 = _13 = 0.0f;
		_21 = _23 = 0.0f;
		_31 = _32 = 0.0f;
	}

	float3x3::float3x3(const XMFLOAT3X3& m) 
	{
		*reinterpret_cast<XMFLOAT3X3*>(this) = m;
	}

	float3x3::float3x3(CXMMATRIX m)
	{
		XMStoreFloat3x3(reinterpret_cast<XMFLOAT3X3*>(this), m);
	}

	float3x3::float3x3(const Vector3& r0, const Vector3& r1, const Vector3& r2)
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

	float3x3& float3x3::operator*=(const float3x3& other)
	{
		XMMATRIX result = this->ToSIMD() * other.ToSIMD();
		XMStoreFloat3x3(reinterpret_cast<XMFLOAT3X3*>(this), result);
		return *this;
	}

	float3x3 float3x3::operator*(const float3x3& other) const
	{
		XMMATRIX result = this->ToSIMD() * other.ToSIMD();
		return float3x3(result);
	}

	Vector3 float3x3::operator*(Vector3 vec)
	{
		return Vector3(XMVector3TransformNormal(vec.ToSIMD(), this->ToSIMD()));
	}

	Vector3 float3x3::Up() const
	{
		return Vector3(_21, _22, _23);
	}

	Vector3 float3x3::Down() const
	{
		return Vector3(-_21, -_22, -_23);
	}

	Vector3 float3x3::Left() const
	{
		return Vector3(-_11, -_12, -_13);
	}

	Vector3 float3x3::Right() const
	{
		return Vector3(_11, _12, _13);
	}

	Vector3 float3x3::Forward() const
	{
		return Vector3(_31, _32, _33);
	}

	Vector3 float3x3::Back() const
	{
		return Vector3(-_31, -_32, -_33);
	}

	float float3x3::Get(int row, int column) const
	{
		if (row == 1)
		{
			if (column == 1)
			{
				return _11;
			}
			else if (column == 2)
			{
				return _12;
			}
			else if (column == 3)
			{
				return _13;
			}
			else
			{
				return 0;
			}
		}
		else if(row == 2)
		{
			if (column == 1)
			{
				return _21;
			}
			else if (column == 2)
			{
				return _22;
			}
			else if (column == 3)
			{
				return _23;
			}
			else
			{
				return 0;
			}
		}
		else if(row == 3)
		{
			if (column == 1)
			{
				return _31;
			}
			else if (column == 2)
			{
				return _32;
			}
			else if (column == 3)
			{
				return _33;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}

	void float3x3::SetXBasis(const Vector3& x)
	{
		_11 = x.x;
		_12 = x.y;
		_13 = x.z;
	}

	void float3x3::SetYBasis(const Vector3& y)
	{
		_21 = y.x;
		_22 = y.y;
		_23 = y.z;
	}

	void float3x3::SetZBasis(const Vector3& z)
	{
		_31 = z.x;
		_32 = z.y;
		_33 = z.z;
	}

	float3x3 float3x3::Transpose(const float3x3& m)
	{
		return float3x3(XMMatrixTranspose(m.ToSIMD()));
	}

	float3x3 float3x3::Invert(const float3x3& m)
	{
		XMVECTOR det;
		return float3x3(XMMatrixInverse(&det, m.ToSIMD()));
	}

	float3x3 float3x3::ScaleMatrix(float s)
	{
		float3x3 m;
		m._11 = m._22 = m._33 = s;
		return m;
	}

	float3x3 float3x3::ScaleMatrix(const Vector3& s)
	{
		float3x3 m;
		m._11 = s.x;
		m._22 = s.y;
		m._33 = s.z;
		return m;
	}

	float3x3 float3x3::RotationAxisAngle(const Vector3& axis, float angle)
	{
		return float3x3(XMMatrixRotationAxis(axis.ToSIMD(), angle));
	}

	float3x3 float3x3::RotationEuler(float x, float y, float z)
	{
		return float3x3(XMMatrixRotationRollPitchYaw(x, y, z));
	}

	float3x3 float3x3::SetFromToRotation(const Vector3& from, const Vector3& to)
	{ 
		float3x3 m;
		Vector3 v = Vector3::Cross(from, to);
		float e = Vector3::Dot(from, to);
		const float kEpsilon = 0.000001f;
		if (e > 1.0 - kEpsilon)     /* "from" almost or equal to "to"-vector? */
		{
			/* return identity */
			m._11 = 1.0; m._12 = 0.0; m._13 = 0.0;
			m._21 = 0.0; m._22 = 1.0; m._23 = 0.0;
			m._31 = 0.0; m._32 = 0.0; m._33 = 1.0;
		}
		else if (e < -1.0 + kEpsilon) /* "from" almost or equal to negated "to"? */
		{
			float invlen;
			float fxx, fyy, fzz, fxy, fxz, fyz;
			float uxx, uyy, uzz, uxy, uxz, uyz;
			float lxx, lyy, lzz, lxy, lxz, lyz;
			/* left=CROSS(from, (1,0,0)) */
			Vector3 left(0.0f, from[2], -from[1]);
			if (Vector3::Dot(left, left) < kEpsilon) /* was left=CROSS(from,(1,0,0)) a good choice? */
			{
				/* here we now that left = CROSS(from, (1,0,0)) will be a good choice */
				left.x = -from.z; left.y = 0.0; left.z = from.x;
			}
			/* normalize "left" */
			invlen = 1.0f / sqrt(Vector3::Dot(left, left));
			left.x *= invlen;
			left.y *= invlen;
			left.z *= invlen;
			Vector3 up = Vector3::Cross(left, from);
			/* now we have a coordinate system, i.e., a basis;    */
			/* M=(from, up, left), and we want to rotate to:      */
			/* N=(-from, up, -left). This is done with the matrix:*/
			/* N*M^T where M^T is the transpose of M              */
			fxx = -from[0] * from[0]; fyy = -from[1] * from[1]; fzz = -from[2] * from[2];
			fxy = -from[0] * from[1]; fxz = -from[0] * from[2]; fyz = -from[1] * from[2];

			uxx = up[0] * up[0]; uyy = up[1] * up[1]; uzz = up[2] * up[2];
			uxy = up[0] * up[1]; uxz = up[0] * up[2]; uyz = up[1] * up[2];

			lxx = -left[0] * left[0]; lyy = -left[1] * left[1]; lzz = -left[2] * left[2];
			lxy = -left[0] * left[1]; lxz = -left[0] * left[2]; lyz = -left[1] * left[2];
			/* symmetric matrix */
			m._11 = fxx + uxx + lxx; m._12 = fxy + uxy + lxy; m._13 = fxz + uxz + lxz;
			m._21 = m._12;   m._22 = fyy + uyy + lyy; m._23 = fyz + uyz + lyz;
			m._31 = m._13;   m._32 = m._23;   m._33 = fzz + uzz + lzz;
		}
		else  /* the most common case, unless "from"="to", or "from"=-"to" */
		{
			/* ...otherwise use this hand optimized version (9 mults less) */
			float hvx, hvz, hvxy, hvxz, hvyz;
			float h = (1.0f - e) / Vector3::Dot(v, v);
			hvx = h * v[0];
			hvz = h * v[2];
			hvxy = hvx * v[1];
			hvxz = hvx * v[2];
			hvyz = hvz * v[1];
			m._11 = e + hvx * v[0]; m._12 = hvxy - v[2];     m._13 = hvxz + v[1];
			m._21 = hvxy + v[2];  m._22 = e + h * v[1] * v[1]; m._23 = hvyz - v[0];
			m._31 = hvxz - v[1];  m._32 = hvyz + v[0];     m._33 = e + hvz * v[2];
		}
		return m;
	}

	// Right handed
	bool float3x3::LookRotationToMatrix(const Vector3& viewVec, const Vector3& upVec, float3x3* m)
	{
		Vector3 z = viewVec;
		// compute u0
		float mag = Vector3::Magnitude(z);
		if (mag < EPSILON)
		{
			m = new float3x3();
			return false;
		}
		z /= mag;

		Vector3 x = Vector3::Cross(upVec, z);
		mag = Vector3::Magnitude(x);
		if (mag < EPSILON)
		{
			m = new float3x3();
			return false;
		}
		x /= mag;

		Vector3 y(Vector3::Cross(z, x));
		if (!Dist::CompareApproximately(Vector3::SqrMagnitude(y), 1.0F)) 
			return false;

		m->SetXBasis(x);
		m->SetYBasis(y);
		m->SetZBasis(z);
		return true;
	}

	XMMATRIX float3x3::ToSIMD() const
	{
		return XMLoadFloat3x3(reinterpret_cast<const XMFLOAT3X3*>(this));
	}



	float4x4::float4x4()
	{
		_11 = _22 = _33 = _44 = 1.0f;
		_12 = _13 = _14 = 0.0f;
		_21 = _23 = _24 = 0.0f;
		_31 = _32 = _34 = 0.0f;
		_41 = _42 = _43 = 0.0f;
	}

	float4x4::float4x4(const XMFLOAT4X4& m)
	{
		*reinterpret_cast<XMFLOAT4X4*>(this) = m;
	}

	float4x4::float4x4(CXMMATRIX m)
	{
		XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(this), m);
	}

	float4x4::float4x4(const Vector4& r0, const Vector4& r1, const Vector4& r2, const Vector4& r3)
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

	float4x4& float4x4::operator*=(const float4x4& other)
	{
		XMMATRIX result = this->ToSIMD() * other.ToSIMD();
		XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(this), result);
		return *this;
	}

	float4x4 float4x4::operator*(const float4x4& other) const
	{
		XMMATRIX result = this->ToSIMD() * other.ToSIMD();
		return float4x4(result);
	}

	Vector3 float4x4::operator*(Vector3 vec)
	{
		return Vector3(XMVector3Transform(vec.ToSIMD(), this->ToSIMD()));
	}

	Vector4 float4x4::operator*(Vector4 vec)
	{
		return Vector4(XMVector4Transform(vec.ToSIMD(), this->ToSIMD()));
	}

	Vector3 float4x4::Up() const
	{
		return Vector3(_21, _22, _23);
	}

	Vector3 float4x4::Down() const
	{
		return Vector3(-_21, -_22, -_23);
	}

	Vector3 float4x4::Left() const
	{
		return Vector3(-_11, -_12, -_13);
	}

	Vector3 float4x4::Right() const
	{
		return Vector3(_11, _12, _13);
	}

	Vector3 float4x4::Forward() const
	{
		return Vector3(_31, _32, _33);
	}

	Vector3 float4x4::Back() const
	{
		return Vector3(-_31, -_32, -_33);
	}

	Vector3 float4x4::Translation() const
	{
		return Vector3(_41, _42, _43);
	}

	void float4x4::SetTranslation(const Vector3& t)
	{
		_41 = t.x;
		_42 = t.y;
		_43 = t.z;
	}

	void float4x4::SetXBasis(const Vector3& x)
	{
		_11 = x.x;
		_12 = x.y;
		_13 = x.z;
	}

	void float4x4::SetYBasis(const Vector3& y)
	{
		_21 = y.x;
		_22 = y.y;
		_23 = y.z;
	}

	void float4x4::SetZBasis(const Vector3& z)
	{
		_31 = z.x;
		_32 = z.y;
		_33 = z.z;
	}

	void float4x4::Scale(const Vector3& scale)
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

	float4x4 float4x4::Transpose(const float4x4& m)
	{
		return float4x4(XMMatrixTranspose(m.ToSIMD()));
	}

	float4x4 float4x4::Invert(const float4x4& m)
	{
		XMVECTOR det;
		return float4x4(XMMatrixInverse(&det, m.ToSIMD()));
	}

	float4x4 float4x4::RotationAxisAngle(const Vector3& axis, float angle)
	{
		return float4x4(XMMatrixRotationAxis(axis.ToSIMD(), angle));
	}

	float4x4 float4x4::RotationEuler(float x, float y, float z)
	{
		return float4x4(XMMatrixRotationRollPitchYaw(x, y, z));
	}

	float4x4 float4x4::ScaleMatrix(float s)
	{
		float4x4 m;
		m._11 = m._22 = m._33 = s;
		return m;
	}

	float4x4 float4x4::ScaleMatrix(const Vector3& s)
	{
		float4x4 m;
		m._11 = s.x;
		m._22 = s.y;
		m._33 = s.z;
		return m;
	}

	float4x4 float4x4::TranslationMatrix(const Vector3& t)
	{
		float4x4 m;
		m.SetTranslation(t);
		return m;
	}

	bool float4x4::operator==(const float4x4& other) const
	{
		const float* ours = reinterpret_cast<const float*>(this);
		const float* theirs = reinterpret_cast<const float*>(&other);
		for (uint64_t i = 0; i < 16; ++i)
			if (ours[i] != theirs[i])
				return false;
		return true;
	}

	bool float4x4::operator!=(const float4x4& other) const
	{
		const float* ours = reinterpret_cast<const float*>(this);
		const float* theirs = reinterpret_cast<const float*>(&other);
		for (uint64_t i = 0; i < 16; ++i)
			if (ours[i] != theirs[i])
				return true;
		return false;
	}

	XMMATRIX float4x4::ToSIMD() const 
	{
		return XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(this));
	}

	float3x3 float4x4::To3x3() const
	{
		return float3x3(Right(), Up(), Forward());
	}
}