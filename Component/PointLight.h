#pragma once
#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include "Transform.h"
#include "BoundingSphere.h"
#include "../Core/Singleton.h"
#include "../Core/Mathf/Color.h"


class PointLight : public Transform
{
public:
	//	��Χ
	float rangeFactory;
	//	ǿ��ֵ
	float intensity;
	//	��ɫ
	Color color;
	//	������ײ��
	DistBound::BoundingSphere bound;

public:
	PointLight();
	PointLight(Color color);
	PointLight(Color color, float intens);
	PointLight(Color color, float intens, float range);
	~PointLight();
};

#endif