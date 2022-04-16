#pragma once
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "../Core/Core/Singleton.h"

namespace Dist
{
	class GameObject : public Singleton<GameObject>
	{
	public:
		//	GOµÄÃû×Ö
		std::string											name;

	public:
		GameObject() {};
		~GameObject();

		virtual void tick(float delta_time);

		void               setName(std::string name)		{ this->name = name; }
		const std::string& getName()						const { return name; }

		void destory();
	};
}