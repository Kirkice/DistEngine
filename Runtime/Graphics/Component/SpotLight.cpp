/***********************************************************************************************
 ***				      G R A P H I C S C O R E  ---  D I S T E N G I N E                  ***
 ***********************************************************************************************
 *                                                                                             *
 *                                   Project Name : GraphicsCore							   *
 *                                                                                             *
 *                                   File Name : SpotLight.cpp								   *
 *                                                                                             *
 *                                    Programmer : Kirk                                        *
 *                                                                                             *
 *                                     Date : 2022/8/12                                        *
 *                                                                                             *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "SpotLight.h"
#include <string>

SpotLight::SpotLight(std::string _name = "SpotLight") : Component(_name)
{
	rangeFactory = 1;
	intensity = 1;
	spotAngle = 30;
	color = Color(1.0f, 1.0f, 1.0f, 1.0f);
}

SpotLight::SpotLight(Color color, std::string _name = "SpotLight") : Component(_name)
{
	rangeFactory = 1;
	intensity = 1;
	spotAngle = 30;
	color = color;
}

SpotLight::SpotLight(Color color, float range, std::string _name = "SpotLight") : Component(_name)
{
	rangeFactory = range;
	intensity = 1;
	spotAngle = 30;
	color = color;
}

SpotLight::SpotLight(Color color, float range, float inten, std::string _name = "SpotLight") : Component(_name)
{
	rangeFactory = range;
	intensity = inten;
	spotAngle = 30;
	color = color;
}

SpotLight::SpotLight(Color color, float range, float inten, float Angle, std::string _name = "SpotLight") : Component(_name)
{
	rangeFactory = range;
	intensity = inten;
	spotAngle = Angle;
	color = color;
}

SpotLight::~SpotLight()
{
}