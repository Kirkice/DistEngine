#include "SpotLight.h"
#include "../Core/Mathf/Color.h"
#include <string>

SpotLight::SpotLight()
{
	rangeFactory = 1;
	intensity = 1;
	spotAngle = 30;
	color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	bound = DistBound::BoundingSphere(position.x, position.y, position.z, 1);
}

SpotLight::SpotLight(Color color)
{
	rangeFactory = 1;
	intensity = 1;
	spotAngle = 30;
	color = color;
	bound = DistBound::BoundingSphere(position.x, position.y, position.z, 1);
}

SpotLight::SpotLight(Color color, float range)
{
	rangeFactory = range;
	intensity = 1;
	spotAngle = 30;
	color = color;
	bound = DistBound::BoundingSphere(position.x, position.y, position.z, 1);
}

SpotLight::SpotLight(Color color, float range, float inten)
{
	rangeFactory = range;
	intensity = inten;
	spotAngle = 30;
	color = color;
	bound = DistBound::BoundingSphere(position.x, position.y, position.z, 1);
}

SpotLight::SpotLight(Color color, float range, float inten, float Angle)
{
	rangeFactory = range;
	intensity = inten;
	spotAngle = Angle;
	color = color;
	bound = DistBound::BoundingSphere(position.x, position.y, position.z, 1);
}

SpotLight::~SpotLight()
{
}