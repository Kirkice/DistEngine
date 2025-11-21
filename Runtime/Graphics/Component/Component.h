#pragma once
#include <string>

class Component
{
protected:
	std::string m_strComponent;

public:
	Component(std::string _name) : m_strComponent(_name) {}
	virtual ~Component() = default;
	virtual const std::string& getName() const
	{
		return m_strComponent;
	}
};