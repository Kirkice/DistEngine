#pragma once
#ifndef DIRECTION_LIGHT_H
#define DIRECTION_LIGHT_H

#include "Transform.h"
#include "../Core/Core/Singleton.h"
#include "../Core/Mathf/Color.h"
#include "../Core/Mathf/BoundingSphere.h"

namespace Dist
{
	class DirectionLight : public Transform
	{
	public:
		//	�Ƿ�������Դ ������Ӱ
		bool isMainLight;
		//	ǿ��ֵ
		float intensity;
		//	��ɫ
		Color color;
		//	������ײ��
		BoundingSphere bound;

	public:
		DirectionLight();
		DirectionLight(Color color);
		DirectionLight(bool isMain);
		DirectionLight(bool isMain, Color color);
		DirectionLight(bool isMain, Color color, float inten);
		~DirectionLight();
	};
}

#endif