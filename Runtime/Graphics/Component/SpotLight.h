/***********************************************************************************************
 ***				      G R A P H I C S C O R E  ---  D I S T E N G I N E                  ***
 ***********************************************************************************************
 *                                                                                             *
 *                                   Project Name : GraphicsCore							   *
 *                                                                                             *
 *                                   File Name : SpotLight.h								   *
 *                                                                                             *
 *                                    Programmer : Kirk                                        *
 *                                                                                             *
 *                                     Date : 2022/8/12                                        *
 *                                                                                             *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#pragma once
#include "Light.h"

class SpotLight : public Light
{

public:
	//	范围
	float rangeFactory;
	//	聚光角度
	float spotAngle;

public:
	SpotLight(std::string _name);
	SpotLight(Color color, std::string _name);
	SpotLight(Color color, float range, std::string _name);
	SpotLight(Color color, float range, float inten, std::string _name);
	SpotLight(Color color, float range, float inten, float Angle, std::string _name);
	~SpotLight();
};
