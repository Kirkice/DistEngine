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
	}

	PointLight::PointLight(Color color)
	{
		rangeFactory = 1;
		intensity = 1;
		color = color;
	}

	PointLight::PointLight(Color color, float intensity)
	{
		rangeFactory = 1;
		intensity = intensity;
		color = color;
	}

	PointLight::PointLight(Color color, float intensity, float rangeFactory)
	{
		rangeFactory = rangeFactory;
		intensity = intensity;
		color = color;
	}

	PointLight::~PointLight()
	{
	}
}