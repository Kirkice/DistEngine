#pragma once
#include "../MathHelper.h"

using namespace DirectX;

namespace Mathf
{
	struct Vector2
	{
		/// <summary>
		/// 构造Vector2 和一些加减乘除
		/// </summary>
		float x, y;

		Vector2();
		Vector2(float x);
		Vector2(float x, float y);
		explicit Vector2(const DirectX::XMFLOAT2& xy);
		explicit Vector2(DirectX::FXMVECTOR xy);

		Vector2& operator+=(const Vector2& other);
		Vector2 operator+(const Vector2& other) const;

		Vector2& operator-=(const Vector2& other);
		Vector2 operator-(const Vector2& other) const;

		Vector2& operator*=(const Vector2& other);
		Vector2 operator*(const Vector2& other) const;

		Vector2& operator*=(float s);
		Vector2 operator*(float s) const;

		Vector2& operator/=(const Vector2& other);
		Vector2 operator/(const Vector2& other) const;

		Vector2& operator/=(float s);
		Vector2 operator/(float s) const;

		bool operator==(const Vector2& other) const;
		bool operator!=(const Vector2& other) const;

		Vector2 operator-() const;

		DirectX::XMVECTOR ToSIMD() const;

		DirectX::XMFLOAT2 ToXMFLOAT2() const;
		//钳制
		static Vector2 Clamp(const Vector2& val, const Vector2& min, const Vector2& max);
		//长度
		static float Length(const Vector2& val);
		//点乘
		float Dot(const Vector2& v) const;
		//叉乘
		float CrossZ(const Vector2& vec) const;
		//正交化
		float Normalize();
		//获得正交向量
		Vector2 Normalized() const;
		//和另一个向量距离
		float DistanceTo(const Vector2& v) const;
		//距离平方
		float DistanceSquaredTo(const Vector2& v) const;
		//旋转
		Vector2 Rotate(float radians) const;
		//转数组
		float* Vector2Array(const Vector2& v);
		//最大值 最小值
		static Vector2 Min(const Vector2& a, const Vector2& b);
		static Vector2 Max(const Vector2& a, const Vector2& b);

		static void StoreVector2(Vector2* pDestination, Vector2 V);

		static void StoreVector2(Vector2* pDestination, XMVECTOR V);
	};
}