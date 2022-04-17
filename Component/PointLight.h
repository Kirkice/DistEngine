#pragma once
#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include "Transform.h"
#include "../Core/Core/Singleton.h"
#include "../Core/Mathf/Color.h"

namespace Dist
{
	class PointLight : public Transform
	{
	public:
		//	��Χ
		float rangeFactory;
		//	ǿ��ֵ
		float intensity;
		//	��ɫ
		Color color;

	public:
		PointLight();
		PointLight(Color color);
		PointLight(Color color,float intensity);
		PointLight(Color color, float intensity, float rangeFactory);
		~PointLight();
	};
}

#endif