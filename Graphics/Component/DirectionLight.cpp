#include "DirectionLight.h"
#include "../Core/Mathf/Color.h"
#include <string>


DirectionLight::DirectionLight()
{
	isMainLight = false;
	color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	intensity = 1;
	bound = DistBound::BoundingSphere(position.x, position.y, position.z, 1);
}

DirectionLight::DirectionLight(bool isMain)
{
	isMainLight = isMain;
	color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	intensity = 1;
	bound = DistBound::BoundingSphere(position.x, position.y, position.z, 1);
}

DirectionLight::DirectionLight(Color color)
{
	isMainLight = false;
	color = color;
	intensity = 1;
	bound = DistBound::BoundingSphere(position.x, position.y, position.z, 1);
}

DirectionLight::DirectionLight(bool isMain, Color color)
{
	isMainLight = isMain;
	color = color;
	intensity = 1;
	bound = DistBound::BoundingSphere(position.x, position.y, position.z, 1);
}

DirectionLight::DirectionLight(bool isMain, Color color, float inten)
{
	isMainLight = isMain;
	color = color;
	intensity = inten;
	bound = DistBound::BoundingSphere(position.x, position.y, position.z, 1);
}

DirectionLight::~DirectionLight()
{
}