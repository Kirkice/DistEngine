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
	//��������
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

	//��ȡ����
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

	//��ȡ������Ӧ�ڵ�ָ��
	Node<T>* GetNodePointer(unsigned int Index)
	{
		Node<T>* p = List;
		for (unsigned int i = 0; i < Index; i++)
		{
			p = p->NextNode;
		}
		return p;
	}

	//��ȡ������Ӧ�ڵ�ֵ
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

	//��������
	void Traverse()
	{
		Node<T>* p = List;
		do
		{
			cout << p->Value << endl;
			p = p->NextNode;
		} while (p != nullptr);
	}

	//����ֵ
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

	//������������ֵ
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

	//ɾ���������Ľڵ�
	void Pop_Back()
	{
		Node<T>* p = GetNodePointer(GetLength() - 2);
		free(p->NextNode);
		p->NextNode = nullptr;
	}

	//ɾ�������ض�������Ӧ�ڵ�
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

	//ɾ������
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