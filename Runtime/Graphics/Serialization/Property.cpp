//
// Created by Administrator on 2022/12/6.
//

#include "property.h"

template <typename T>
void State::add_property(string name)
{
	if (!props[name])
		props[name] = new specific_property<T>;
}

template <typename T>
void State::set_property(string name, T& value)
{
	if (!props[name])
		return;
	specific_property<T>* p;
	p = reinterpret_cast<specific_property<T>*>(props[name]);
	p->setValue(value);
}

template <typename T>
void State::set_property(string name, T&& value)
{
	if (!props[name])
		return;
	specific_property<T>* p;
	p = reinterpret_cast<specific_property<T>*>(props[name]);
	p->setValue(value);
}

template <typename T>
T State::get_value(string name) {
	return ((specific_property<T>*)props[name])->getValue();
}

vector<string> State::get_properties()
{
	vector<string> res;
	unordered_map<string, property*>::iterator iter = props.begin();
	while (iter != props.end())
	{
		res.push_back(iter->first);
		iter++;
	}
	return res;
}