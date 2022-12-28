/***********************************************************************************************
 ***				      G R A P H I C S C O R E  ---  D I S T E N G I N E                  ***
 ***********************************************************************************************
 *                                                                                             *
 *                                   Project Name : GraphicsCore							   *
 *                                                                                             *
 *                                   File Name : GameObject.cpp	                               *
 *                                                                                             *
 *                                    Programmer : Kirk                                        *
 *                                                                                             *
 *                                     Date : 2022/8/12                                        *
 *                                                                                             *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "GameObject.h"

GameObject::GameObject(string name) : Component(name)
{
}

GameObject::~GameObject()
{
	vector<Component*>::iterator it = mComponents.begin();
	while (it != mComponents.end())
	{
		if (*it != NULL)
		{
			cout << "---delete" << (*it)->getName() + "---" << endl;
			delete* it;
			*it = NULL;
		}
		mComponents.erase(it);
		it = mComponents.begin();
	}
}

//	设置名字
void  GameObject::setName(std::string name)
{
	this->name = name;
}


//	获取名字
const std::string& GameObject::getName()const
{
	return name;
}

//	添加组件
void GameObject::AddComponent(Component* com)
{
	mComponents.push_back(com);
}

//	移除组件
void GameObject::RemoveComponent(Component* com)
{
	for (vector<Component*>::iterator it = mComponents.begin(); it != mComponents.end(); ++it)
	{
		if ((*it)->getName() == com->getName())
		{
			if (*it != NULL)
			{
				delete* it;
				*it = NULL;
			}
			mComponents.erase(it);
			break;
		}
	}
}