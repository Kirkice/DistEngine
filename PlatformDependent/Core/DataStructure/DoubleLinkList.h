#pragma once
#include<iostream>
using namespace std;

template<class T>
class DoubleListNode
{
public:
	T Date;
	DoubleListNode<T>* last;
	DoubleListNode<T>* next;
public:
	DoubleListNode(const T& Date) :last(nullptr), next(nullptr), Date(Date) {}
};

template<class T>
class DoubleList
{
	DoubleListNode<T>* Head;
public:
	DoubleList(const T& Date) :Head(nullptr) { Head = new DoubleListNode<T>(Date); }
	~DoubleList();
public:
	void insertion(T Date);//头插
	int find(T Date);//查找
	void amend(T Old, T New);//修改
	void remove(T Date);//删除
private:
	bool _find(T Date);//是否找到
};

template<class T>
inline DoubleList<T>::~DoubleList()
{
	DoubleListNode<T>* p = Head;
	while (true)
	{
		if (p->next == nullptr)
		{
			while (p != Head)
			{
				p = p->last;
				delete p->next;
				p->next = nullptr;
			}
			if (p == Head)
			{
				Head = nullptr;
				delete p;
				p = nullptr;
			}
			else
			{
				delete p;
				p = nullptr;
				delete Head;
				Head = nullptr;
			}
			return;
		}
		else p = p->next;
	}
}

template<class T>
inline void DoubleList<T>::insertion(T Date)
{
	DoubleListNode<T>* p = new DoubleListNode<T>(Date);
	if (Head->next == nullptr)
	{
		Head->next = p;
		p->last = Head;
		return;
	}
	else
	{
		p->next = Head->next;
		p->last = Head->next->last;
		Head->next->last = p;
		Head->next = p;
		return;
	}
}

template<class T>
inline int DoubleList<T>::find(T Date)
{
	int x = 0;
	if (_find)
	{
		DoubleListNode<T>* p = Head;
		while (p != nullptr)
		{
			if (p->Date == Date)
			{
				return x;
			}
			p = p->next;
			x++;
		}
	}
	else return -78494;
}

template<class T>
inline void DoubleList<T>::amend(T Old, T New)
{
	if (_find(Old))
	{
		DoubleListNode<T>* p = Head;
		while (p != nullptr)
		{
			if (p->Date == Old)
			{
				p->Date = New;
				return;
			}
			p = p->next;
		}
	}
	else return;
}

template<class T>
inline void DoubleList<T>::remove(T Date)
{
	if (_find(Date))
	{
		DoubleListNode<T>* p = Head;
		DoubleListNode<T>* p1 = p->next;
		while (p1 != nullptr)
		{
			if (p1->Date == Date)
			{
				p->next = p1->next;
				p1->next->last = p;
				delete p1;
				p1 = nullptr;
				return;
			}
			p = p->next;
			p1 = p1->next;
		}
	}
	else return;
}

template<class T>
inline bool DoubleList<T>::_find(T Date)
{
	DoubleListNode<T>* p = Head;
	while (p != nullptr)
	{
		if (p->Date == Date)
		{
			return true;
		}
		p = p->next;
	}
	return false;
}