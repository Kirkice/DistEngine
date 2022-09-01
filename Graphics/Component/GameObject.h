#pragma once
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>
#include "GameTimer.h"

class GameObject
{
public:
	//	GOµÄÃû×Ö
	std::string											name;
	bool												Enable;
public:
	GameObject() { Enable = true; };
	~GameObject();

	virtual void tick(const GameTimer& gt);

	void               setName(std::string name) { this->name = name; }
	const std::string& getName()						const { return name; }

	void destory();
};