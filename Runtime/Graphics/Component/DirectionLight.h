/***********************************************************************************************
 ***				      G R A P H I C S C O R E  ---  D I S T E N G I N E                  ***
 ***********************************************************************************************
 *                                                                                             *
 *                                   Project Name : GraphicsCore							   *
 *                                                                                             *
 *                                   File Name : DirectionLight.h	                           *
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

class DirectionLight : public Component
{
public:
	//	�Ƿ�������Դ ������Ӱ
	bool isMainLight;
	//	ǿ��ֵ
	float intensity;
	//	��ɫ
	Color color;

public:
	DirectionLight(std::string _name);
	DirectionLight(Color color, std::string _name);
	DirectionLight(bool isMain, std::string _name);
	DirectionLight(bool isMain, Color color, std::string _name);
	DirectionLight(bool isMain, Color color, float inten, std::string _name);
	~DirectionLight();
};