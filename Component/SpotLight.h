#pragma once
#ifndef SPOT_LIGHT_H
#define SPOT_LIGHT_H

#include "Transform.h"
#include "../Core/Mathf/BoundingSphere.h"
#include "../Core/Core/Singleton.h"
#include "../Core/Mathf/Color.h"

namespace Dist
{
	class SpotLight : public Transform
	{

	public:
		//	范围
		float rangeFactory;
		//	强度值
		float intensity;
		//	开合角度
		float spotAngle;
		//	颜色
		Color color;
		//	球形碰撞器
		BoundingSphere bound;

	public:
		SpotLight();
		SpotLight(Color color);
		SpotLight(Color color, float range);
		SpotLight(Color color, float range, float inten);
		SpotLight(Color color, float range, float inten, float Angle);
		~SpotLight();
	};
}

#endif