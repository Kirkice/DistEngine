//
// Created by KirkH on 2022/12/4.
//

#ifndef PROPERTY_PROPERTY_H
#define PROPERTY_PROPERTY_H
#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>

using namespace std;

class property {
public:
	property() {}
	virtual ~property() {}
};

template <typename T>
class specific_property : public property {
private:
	T data;
public:
	specific_property() :property() {}
	void setValue(T& _data) { data = _data; }
	T getValue() { return data; }
};

class State {
private:
	unordered_map<string, property*> props;
public:
	template <typename T>
	void add_property(string name);
	template <typename T>
	void set_property(string name, T& value);
	template <typename T>
	void set_property(string name, T&& value);
	template <typename T>
	T get_value(string name);
	vector<string> get_properties();
};

#endif //PROPERTY_PROPERTY_H
