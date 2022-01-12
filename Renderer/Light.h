#pragma once
#include "d3dApp.h"
#include "Mathf.h"

using namespace Mathf;

#define MaxLights 16

namespace Renderer
{
	//DirectionLight
	struct DirectionLight
	{
		Vector3 Direction = { 0.0f, 0.0f, 0.0f };
		float Strength = 1;
		Vector3 Color = { 0.5f, 0.5f, 0.5f };
		float CastShadow = 1;
		Vector3 Position = { 0.0f, 0.0f, 0.0f };
		float Active = 1;
	};

	//PointLight
	struct PointLight
	{
		Vector3 Color = { 0.5f, 0.5f, 0.5f };
		float rangeFactory = 1.0;
		Vector3 Position = { 0.0f, 0.0f, 0.0f };
		float Strength = 1;
		float Active = 0;
	};

	//SpotLight
	struct SpotLight
	{
		Vector3 Color = { 0.5f, 0.5f, 0.5f };
		float rangeFactory = 15.0f;
		Vector3 Position = { 0.0f, 0.0f, 0.0f };
		float spotLightsStrength = 1;
		Vector3 Direction = { 0.0f, -1.0f, 0.0f }; 
		float spotAngle = 30;
		float spotLightsActive = 0;
	};
}