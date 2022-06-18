#pragma once
#include <type_traits>


template<typename T>
class Singleton
{
protected:
	Singleton() = default;
	~Singleton() = default;
	Singleton(const Singleton&);
	Singleton& operator = (const Singleton&);

public:
	static T& getInstance() noexcept(std::is_nothrow_constructible<T>::value)
	{
		static T instance;
		return instance;
	}
};