#pragma once
#ifndef DIRECTION_LIGHT_H
#define DIRECTION_LIGHT_H

#include "Transform.h"
#include "../Core/Core/Singleton.h"
#include "../Core/Mathf/Color.h"

namespace Dist
{
	class DirectionLight : public Transform
	{
	public:
		//	是否是主光源 产生阴影
		bool isMainLight;
		//	强度值
		float intensity;
		//	颜色
		Color color;

	public:
		DirectionLight();
		DirectionLight(Color color);
		DirectionLight(bool isMain);
		DirectionLight(bool isMain, Color color);
		DirectionLight(bool isMain, Color color, float intensity);
		~DirectionLight();
	};
}

#endif