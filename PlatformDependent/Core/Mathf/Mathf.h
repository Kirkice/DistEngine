#pragma once
#include "../MathHelper.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Random.h"
#include "Matrix.h"
#include "Quaternion.h"


namespace Mathf
{
	template<typename T> void Swap(T& a, T& b)
	{
		T tmp = a;
		a = b;
		b = tmp;
	}

	template<typename T> void Shuffle(std::vector<T>& values, Random& randomGenerator)
	{
		const uint64_t count = values.size();
		for (uint64_t i = 0; i < count; ++i)
		{
			uint64_t other = i + (randomGenerator.RandomUint() % (count - i));
			Swap(values[i], values[other]);
		}
	}

	template<typename T> void Shuffle(T* values, uint64_t count, Random& randomGenerator)
	{
		for (uint64_t i = 0; i < count; ++i)
		{
			uint64_t other = i + (randomGenerator.RandomUint() % (count - i));
			Swap(values[i], values[other]);
		}
	}

	inline Vector3 Saturate(Vector3 val)
	{
		Vector3 result;
		result.x = Clamp<float>(val.x, 0.0f, 1.0f);
		result.y = Clamp<float>(val.y, 0.0f, 1.0f);
		result.z = Clamp<float>(val.z, 0.0f, 1.0f);
		return result;
	}

	// Rounds a float		
	inline float Round(float r)
	{
		return (r > 0.0f) ? std::floorf(r + 0.5f) : std::ceilf(r - 0.5f);
	}

	//����С������
	inline float Frac(float x)
	{
		float intPart;
		return std::modf(x, &intPart);
	}

	inline float Smoothstep(float start, float end, float x)
	{
		x = Saturate((x - start) / (end - start));
		return x * x * (3.0f - 2.0f * x);
	}

	inline Vector3 Pow(Vector3 x, float y)
	{
		return Vector3(std::pow(x.x, y), std::pow(x.y, y), std::pow(x.z, y));
	}

	// linear -> sRGB conversion
	inline Vector3 LinearTosRGB(Vector3 color)
	{
		Vector3 x = color * 12.92f;
		Vector3 y = Pow(color, 1.0f / 2.4f) * 1.055 - 0.055f;

		Vector3 clr = color;
		clr.x = color.x < 0.0031308f ? x.x : y.x;
		clr.y = color.y < 0.0031308f ? x.y : y.y;
		clr.z = color.z < 0.0031308f ? x.z : y.z;

		return clr;
	}

	// sRGB -> linear conversion
	inline Vector3 SRGBToLinear(Vector3 color)
	{
		Vector3 x = color / 12.92f;
		Vector3 y = Pow((color + 0.055f) / 1.055f, 2.4f);

		Vector3 clr = color;
		clr.x = color.x <= 0.04045f ? x.x : y.x;
		clr.y = color.y <= 0.04045f ? x.y : y.y;
		clr.z = color.z <= 0.04045f ? x.z : y.z;

		return clr;
	}

	//��������
	inline float ComputeLuminance(Vector3 color)
	{
		return Vector3::Dot(color, Vector3(0.299f, 0.587f, 0.114f));
	}

	//�����굽�ѿ�������ϵ
	inline void SphericalToCartesianXYZYUP(float r, float theta, float phi, Vector3& xyz)
	{
		xyz.x = r * std::cosf(phi) * std::sinf(theta);
		xyz.y = r * std::cosf(theta);
		xyz.z = r * std::sinf(theta) * std::sinf(phi);
	}

	//�ѿ�������ϵ��������
	inline Vector3 SphericalToCartesian(float azimuth, float elevation)
	{
		Vector3 xyz;
		xyz.x = std::cos(azimuth) * std::cos(elevation);
		xyz.y = std::sin(elevation);
		xyz.z = std::sin(azimuth) * std::cos(elevation);
		return xyz;
	}

	//�ѿ�������ϵ��������
	inline Vector2 CartesianToSpherical(const Vector3& xyz)
	{
		float elevation = std::asin(xyz.y);

		float azimuth = std::atan2(xyz.z, xyz.x);
		if (azimuth < 0.0f)
			azimuth = 2.0f * Pi + azimuth;

		return Vector2(azimuth, elevation);
	}

	inline Vector3 QuaternionToEuler(const Quaternion& q)
	{
		Vector3 euler = Vector3();
		const double Epsilon = 0.0009765625f;
		const double Threshold = 0.5f - Epsilon;

		double TEST = q.w * q.y - q.x * q.z;

		if (TEST < -Threshold || TEST > Threshold) // 奇异姿态,俯仰角为±90°
		{
			int sign = Sign(TEST);

			euler.z = -2 * sign * (double)atan2(q.x, q.w); // yaw

			euler.y = sign * (Pi / 2.0); // pitch

			euler.x = 0; // roll

		}
		else
		{
			euler.x = atan2(2 * (q.y * q.z + q.w * q.x), q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z);
			euler.y = asin(-2 * (q.x * q.z - q.w * q.y));
			euler.z = atan2(2 * (q.x * q.y + q.w * q.z), q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z);
		}

		return euler;
	}

}