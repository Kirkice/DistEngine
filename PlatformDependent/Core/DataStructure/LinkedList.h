#pragma once
#include<iostream>

template<typename T>
struct  Node
{
	T  Value;
	Node<T>* NextNode;
};

template<class T>
class LinkedList
{
public:

	Node<T>* List;
	//创建链表
	LinkedList(unsigned int Count)
	{
		Node<T>* pHeader = (Node<T>*)malloc(sizeof(Node<T>));
		if (pHeader == nullptr)
		{
			List = nullptr;
			return;
		}
		pHeader->Value = 445145145;
		pHeader->NextNode = nullptr;
		Node<T>* pTail = pHeader;
		for (unsigned int i = 0; i < Count - 1; i++)
		{
			Node<T>* pNode = (Node<T>*)malloc(sizeof(Node<T>));
			if (pNode != nullptr)
			{
				pNode->Value = 0;
				pNode->NextNode = nullptr;
				pTail->NextNode = pNode;
				pTail = pNode;
			}
		}
		List = pHeader;
		return;
	}

public:

	//获取长度
	unsigned int GetLength()
	{
		Node<T>* p = List;
		int Count = 0;
		do
		{
			Count++;
			p = p->NextNode;
		} while (p != nullptr);
		return Count;
	}

	//获取索引对应节点指针
	Node<T>* GetNodePointer(unsigned int Index)
	{
		Node<T>* p = List;
		for (unsigned int i = 0; i < Index; i++)
		{
			p = p->NextNode;
		}
		return p;
	}

	//获取索引对应节点值
	T GetNodeValue(unsigned int Index)
	{
		if (Index >= GetLength()) return 0;
		Node<T>* p = List;
		for (unsigned int i = 0; i < Index; i++)
		{
			p = p->NextNode;
		}
		return p->Value;
	}

	//遍历链表
	void Traverse()
	{
		Node<T>* p = List;
		do
		{
			cout << p->Value << endl;
			p = p->NextNode;
		} while (p != nullptr);
	}

	//插入值
	void Insert(unsigned int Index, T Value)
	{
		if (Index >= GetLength()) return;
		Node<T>* AddedNode = (Node<T>*)malloc(sizeof(Node<T>));
		if (AddedNode != nullptr)
		{
			AddedNode->Value = Value;
			Node<T>* p = GetNodePointer(Index - 1);
			Node<T>* q = GetNodePointer(Index);
			p->NextNode = AddedNode;
			AddedNode->NextNode = q;
		}
	}

	//向链表最后插入值
	void Push_Back(T Value)
	{
		Node<T>* AddedNode = (Node<T>*)malloc(sizeof(Node<T>));
		if (AddedNode != nullptr)
		{
			AddedNode->Value = Value;
			AddedNode->NextNode = nullptr;
			Node<T>* p = GetNodePointer(GetLength() - 1);
			p->NextNode = AddedNode;
		}
	}

	//删除链表最后的节点
	void Pop_Back()
	{
		Node<T>* p = GetNodePointer(GetLength() - 2);
		free(p->NextNode);
		p->NextNode = nullptr;
	}

	//删除链表特定索引对应节点
	void Delete(unsigned int Index)
	{
		if (Index == GetLength() - 1)
		{
			cout << "e" << endl;
			Node<T>* p = GetNodePointer(Index - 1);
			cout << p->NextNode << endl;
			p->NextNode = nullptr;
			free(p->NextNode);
		}
		else
		{
			Node<T>* p = GetNodePointer(Index - 1);
			Node<T>* Del = p->NextNode;
			Node<T>* q = p->NextNode->NextNode;
			p->NextNode = q;
			free(Del);
		}
	}

	//删除链表
	void Dispose()
	{
		Node<T>* p = List;
		Node<T>* q = p;
		while (p->NextNode != nullptr)
		{
			p = p->NextNode;
			free(q);
			q = p->NextNode;
		}
	}

};