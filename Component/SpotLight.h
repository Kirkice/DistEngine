#pragma once
#ifndef SPOT_LIGHT_H
#define SPOT_LIGHT_H

#include "Transform.h"
#include "BoundingSphere.h"
#include "../Core/Singleton.h"
#include "../Core/Mathf/Color.h"


class SpotLight : public Transform
{

public:
	//	��Χ
	float rangeFactory;
	//	ǿ��ֵ
	float intensity;
	//	���ϽǶ�
	float spotAngle;
	//	��ɫ
	Color color;
	//	������ײ��
	DistBound::BoundingSphere bound;

public:
	SpotLight();
	SpotLight(Color color);
	SpotLight(Color color, float range);
	SpotLight(Color color, float range, float inten);
	SpotLight(Color color, float range, float inten, float Angle);
	~SpotLight();
};

#endif