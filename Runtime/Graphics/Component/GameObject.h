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
	//	GO������
	std::string	name;
	bool		Enable;

public:

	GameObject(std::string name);
	~GameObject();

	//	��������
	void setName(std::string name);

	//	��ȡ����
	const std::string& getName()const;

	//	������
	void AddComponent(Component* com);

	//	�Ƴ����
	void RemoveComponent(Component* com);

	//	��ȡ���
	template<typename TComponent>
	TComponent* GetComponent(int index)
	{
		if ((size_t)index > mComponents.size())
			return NULL;
		return static_cast<TComponent*>(mComponents[index - 1]);
	}
};
