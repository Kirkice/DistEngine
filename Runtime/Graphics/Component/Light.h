#pragma once
#include "Component.h"
#include "Mathf.h"

using namespace Mathf;

class Light : public Component
{
public:
	//	强度值
	float intensity;
	//	颜色
	Color color;

	Light(std::string name) : Component(name), intensity(1.0f), color(1.0f, 1.0f, 1.0f, 1.0f) {}
	virtual ~Light() = default;
};
