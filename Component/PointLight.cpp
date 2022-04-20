#include "PointLight.h"
#include "../Core/Mathf/Color.h"
#include <string>

namespace Dist
{
	PointLight::PointLight()
	{
		rangeFactory = 1;
		intensity = 1;
		color = Color(1.0f, 1.0f, 1.0f, 1.0f);
		bound = BoundingSphere(position.x, position.y, position.z, 1);
	}

	PointLight::PointLight(Color color)
	{
		rangeFactory = 1;
		intensity = 1;
		color = color;
	}

	PointLight::PointLight(Color color, float intens)
	{
		rangeFactory = 1;
		intensity = intens;
		color = color;
		bound = BoundingSphere(position.x, position.y, position.z, 1);
	}

	PointLight::PointLight(Color color, float intens, float range)
	{
		rangeFactory = range;
		intensity = intens;
		color = color;
		bound = BoundingSphere(position.x, position.y, position.z, 1);
	}

	PointLight::~PointLight()
	{
	}
}