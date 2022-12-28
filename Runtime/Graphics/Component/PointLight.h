/***********************************************************************************************
 ***				      G R A P H I C S C O R E  ---  D I S T E N G I N E                  ***
 ***********************************************************************************************
 *                                                                                             *
 *                                   Project Name : GraphicsCore							   *
 *                                                                                             *
 *                                   File Name : PointLight.h								   *
 *                                                                                             *
 *                                    Programmer : Kirk                                        *
 *                                                                                             *
 *                                     Date : 2022/8/12                                        *
 *                                                                                             *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#pragma once
#include "DX12Utils.h"
#include "Component.h";
#include "Mathf.h"

using namespace Mathf;

class PointLight : public Component
{
public:
	//	范围
	float rangeFactory;
	//	强度值
	float intensity;
	//	颜色
	Color color;

public:
	PointLight(std::string _name);
	PointLight(Color color, std::string _name);
	PointLight(Color color, float intens, std::string _name);
	PointLight(Color color, float intens, float range, std::string _name);
	~PointLight();
};