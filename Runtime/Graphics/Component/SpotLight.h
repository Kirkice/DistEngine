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
#include "DX12Utils.h"
#include "Component.h";
#include "Mathf.h"

using namespace Mathf;

class SpotLight : public Component
{

public:
	//	��Χ
	float rangeFactory;
	//	ǿ��ֵ
	float intensity;
	//	���ϽǶ�
	float spotAngle;
	//	��ɫ
	Color color;

public:
	SpotLight(std::string _name);
	SpotLight(Color color, std::string _name);
	SpotLight(Color color, float range, std::string _name);
	SpotLight(Color color, float range, float inten, std::string _name);
	SpotLight(Color color, float range, float inten, float Angle, std::string _name);
	~SpotLight();
};