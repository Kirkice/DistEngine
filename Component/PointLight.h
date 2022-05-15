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
	//	范围
	float rangeFactory;
	//	强度值
	float intensity;
	//	颜色
	Color color;
	//	球形碰撞器
	DistBound::BoundingSphere bound;

public:
	PointLight();
	PointLight(Color color);
	PointLight(Color color, float intens);
	PointLight(Color color, float intens, float range);
	~PointLight();
};

#endif