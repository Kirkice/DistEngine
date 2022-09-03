/***********************************************************************************************
 ***				      G R A P H I C S C O R E  ---  D I S T E N G I N E                  ***
 ***********************************************************************************************
 *                                                                                             *
 *                                   Project Name : GraphicsCore							   *
 *                                                                                             *
 *                                   File Name : DirectionLight.cpp	                           *
 *                                                                                             *
 *                                    Programmer : Kirk                                        *
 *                                                                                             *
 *                                     Date : 2022/8/12                                        *
 *                                                                                             *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "DirectionLight.h"
#include <string>


DirectionLight::DirectionLight(std::string _name = "DirectionLight") : Component(_name)
{
	isMainLight = false;
	color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	intensity = 1;
}

DirectionLight::DirectionLight(bool isMain, std::string _name = "DirectionLight") : Component(_name)
{
	isMainLight = isMain;
	color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	intensity = 1;
}

DirectionLight::DirectionLight(Color color, std::string _name = "DirectionLight") : Component(_name)
{
	isMainLight = false;
	color = color;
	intensity = 1;
}

DirectionLight::DirectionLight(bool isMain, Color color, std::string _name = "DirectionLight") : Component(_name)
{
	isMainLight = isMain;
	color = color;
	intensity = 1;
}

DirectionLight::DirectionLight(bool isMain, Color color, float inten, std::string _name = "DirectionLight") : Component(_name)
{
	isMainLight = isMain;
	color = color;
	intensity = inten;
}

DirectionLight::~DirectionLight()
{
}