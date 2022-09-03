/***********************************************************************************************
 ***				      G R A P H I C S C O R E  ---  D I S T E N G I N E                  ***
 ***********************************************************************************************
 *                                                                                             *
 *                                   Project Name : GraphicsCore							   *
 *                                                                                             *
 *                                   File Name : GameObject.h	                               *
 *                                                                                             *
 *                                    Programmer : Kirk                                        *
 *                                                                                             *
 *                                     Date : 2022/8/12                                        *
 *                                                                                             *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#pragma once
#include "DX12Utils.h"
#include "Component.h"

class GameObject : public Component
{

private:
	std::vector<Component*>  mComponents;

public:
	//	GO的名字
	std::string	name;
	bool		Enable;

public:

	GameObject(std::string name);
	~GameObject();

	//	设置名字
	void setName(std::string name);

	//	获取名字
	const std::string& getName()const;

	//	添加组件
	void AddComponent(Component* com);

	//	移除组件
	void RemoveComponent(Component* com);

	//	获取组件
	template<typename TComponent>
	TComponent* GetComponent(int index)
	{
		if ((size_t)index > mComponents.size())
			return NULL;
		return static_cast<TComponent*>(mComponents[index - 1]);
	}
};
