/***********************************************************************************************
 ***				      G R A P H I C S C O R E  ---  D I S T E N G I N E                  ***
 ***********************************************************************************************
 *                                                                                             *
 *                                   Project Name : GraphicsCore							   *
 *                                                                                             *
 *                                   File Name : PointLight.cpp								   *
 *                                                                                             *
 *                                    Programmer : Kirk                                        *
 *                                                                                             *
 *                                     Date : 2022/8/12                                        *
 *                                                                                             *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "PointLight.h"
#include <string>

PointLight::PointLight(std::string _name = "PointLight") : Light(_name)
{
	rangeFactory = 1;
	intensity = 1;
	color = Color(1.0f, 1.0f, 1.0f, 1.0f);
}

PointLight::PointLight(Color color, std::string _name = "PointLight") : Light(_name)
{
	rangeFactory = 1;
	intensity = 1;
	this->color = color;
}

PointLight::PointLight(Color color, float intens, std::string _name = "PointLight") : Light(_name)
{
	rangeFactory = 1;
	intensity = intens;
	this->color = color;
}

PointLight::PointLight(Color color, float intens, float range, std::string _name = "PointLight") : Light(_name)
{
	rangeFactory = range;
	intensity = intens;
	this->color = color;
}

PointLight::~PointLight()
{
}