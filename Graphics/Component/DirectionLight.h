#pragma once
#ifndef DIRECTION_LIGHT_H
#define DIRECTION_LIGHT_H

#include "Transform.h"
#include "../Core/Mathf/Color.h"
#include "BoundingSphere.h"


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
	DistBound::BoundingSphere bound;

public:
	DirectionLight();
	DirectionLight(Color color);
	DirectionLight(bool isMain);
	DirectionLight(bool isMain, Color color);
	DirectionLight(bool isMain, Color color, float inten);
	~DirectionLight();
};

#endif