#define _CRT_SECURE_NO_WARNINGS 1

#include<iostream>
#include<stack>
using namespace std;

template<class K, class V>
struct BSTNode
{
	K _key;
	V _value;
	BSTNode<K, V>* _left;
	BSTNode<K, V>* _right;

	friend ostream& operator<<<K, V>(ostream&, BSTNode<K, V>*&);
	/*template<class K, class V>
	friend ostream& operator<<(ostream&, BSTNode<K,V>*&);*/

	BSTNode(const K& key, const V& value = 1)
		:_key(key), _value(value), _left(NULL), _right(NULL)
	{}
};

template<class K, class V>
class BSTree
{
public:
	BSTree()
		:_root(NULL)
	{}
	//�����½ڵ㣨�ǵݹ飩
	bool Insert(const K& key, const V& value);
	//�����½ڵ㣨�ݹ飩
	bool Insert_R(const K& key, const V& value)
	{
		return _Insert_R(_root, key, value);
	}
	//�ǵݹ����
	BSTNode<K, V>* Find(const K& key);
	//�ݹ����
	BSTNode<K, V>* Find_R(const K& key)
	{
		return _Find_R(_root, key);
	}
	//�ǵݹ�ɾ��
	bool Remove(const K& key);
	//�ݹ�ɾ��
	bool Remove_R(const K& key)
	{
		return _Remove_R(_root, key);
	}
	//��ӡ
	void NonR_InOrderPrintTree();

protected:
	bool _Insert_R(BSTNode<K, V>*& root, const K& key, const V& value);
	BSTNode<K, V>* _Find_R(BSTNode<K, V>*& root, const K& key);
	bool _Remove_R(BSTNode<K, V>*& root, const K& key);
protected:
	BSTNode<K, V>* _root;
};

template<class K, class V>
bool BSTree<K, V>::Insert(const K& key, const V& value)
{
	if (_root == NULL)
	{
		_root = new BSTNode<K, V>(key);
		return true;
	}
	BSTNode<K, V>* cur = _root;
	BSTNode<K, V>* parent = _root;
	while (cur)
	{
		if (cur->_key < key)
		{
			parent = cur;
			cur = cur->_right;
		}
		else if (cur->_key > key)
		{
			parent = cur;
			cur = cur->_left;
		}
		else
			return false;
	}
	if (key > parent->_key)
		parent->_right = new BSTNode<K, V>(key);
	else
		parent->_left = new BSTNode<K, V>(key);
	return true;
}
template<class K, class V>
bool BSTree<K, V>::_Insert_R(BSTNode<K, V>*& root, const K& key, const V& value)
{
	if (root == NULL)
	{
		root = new BSTNode<K, V>(key);
		return true;
	}
	else if (root->_key < key)
	{
		return _Insert_R(root->_right, key, value);
	}
	else if (root->_key > key)
	{
		return _Insert_R(root->_left, key, value);
	}
	else
	{
		return false;
	}
}
template<class K, class V>
BSTNode<K, V>* BSTree<K, V>::Find(const K& key)
{
	if (_root == NULL)
		return NULL;
	BSTNode<K, V>* cur = _root;
	while (cur)
	{
		if (cur->_key == key)
			return cur;
		else if (cur->_key > key)
			cur = cur->_left;
		else
			cur = cur->_right;
	}
	return NULL;
}
template<class K, class V>
BSTNode<K, V>* BSTree<K, V>::_Find_R(BSTNode<K, V>*& root, const K& key)
{
	if (_root == NULL)
		return NULL;
	BSTNode<K, V>* cur = root;

	if (root->_key == key)
		return root;
	else if (root->_key > key)
		return _Find_R(root->_left, key);
	else
		return _Find_R(root->_right, key);
}
template<class K, class V>
bool BSTree<K, V>::Remove(const K& key)
{
	// 1.���� ���ڵ�Ϊ��
	// 2.ֻ�и��ڵ�
	// 3.�ýڵ�������
		// a.�ýڵ��������� (������������û��)
		// b.�ýڵ���������
		// c.�ýڵ�����������
	if (_root == NULL)
		return false;
	if (_root->_left == NULL && _root->_right == NULL)
	{
		if (_root->_key == key)
		{
			delete _root;
			_root = NULL;
			return true;
		}
		else
			return false;
	}
	BSTNode<K, V>* cur = _root;
	BSTNode<K, V>* parent = NULL;
	BSTNode<K, V>* del = NULL;
	while (cur)
	{
		if (cur->_key < key)
		{
			parent = cur;
			cur = cur->_right;
		}
		else if (cur->_key > key)
		{
			parent = cur;
			cur = cur->_left;
		}
		else
		{ // �ҵ��ýڵ㣬�������ɾ��
			del = cur;
			// 1.Ҫɾ���Ľڵ���������
			if (cur->_left == NULL)
			{
				// a.Ҫɾ���Ľڵ�û��������������������
				if (cur->_right != NULL)
				{   //    \   <
					if (parent->_left == cur)parent->_left = cur->_right;
					else if (parent->_right == cur)parent->_right = cur->_right;
				}
				else
				{// b.Ҫɾ���Ľڵ����������Ҳ��������
					if (parent->_left == cur)parent->_left = NULL;
					else if (parent->_right = cur)parent->_right = NULL;
				}
			}
			// 2.Ҫɾ���Ľڵ�������Ϊ�գ�����������
			else if (cur->_right == NULL)
			{ //        /   >
				if (parent->_left = cur)parent->_left = cur->_left;
				else if (parent->_right = cur)parent->_right = cur->_left;
			}
			// 3.Ҫɾ���Ľڵ��������������������
			else if (cur->_left && cur->_right)
			{
				BSTNode<K, V>* firstleft = cur->_right;
				// ���������е���С�ڵ�,�ҵ����Ҫɾ���Ľڵ㽻��
				while (firstleft->_left)
				{
					parent = firstleft;
					firstleft = firstleft->_left;
				}
				del = firstleft;
				swap(cur->_key, firstleft->_key);
				swap(cur->_value, firstleft->_value);
				// �������������

				// 1.Ҫɾ���ڵ�����������������ǿգ���С�ڵ㾭��ѭ��Ѱ��Ѱ��
				if (parent->_left == firstleft)
				{
					parent->_left = firstleft->_right;
				}
				// 2.Ҫɾ���ڵ�������������ӽڵ㣬����С�ڵ���Ҫɾ���ڵ���Һ���
				else if (cur->_right == firstleft)
				{
					cur->_right = firstleft->_right;
				}
			}
			delete del;
			return true;
		}
	}
	return false;
}
template<class K, class V>
bool BSTree<K, V>::_Remove_R(BSTNode<K, V>*& root, const K& key)
{
	//δ�ҵ�
	if (root == NULL)
		return false;
	//Ҷ�ӽڵ�
	if (root->_left == NULL && root->_right == NULL)
	{
		if (root->_key == key)
		{
			delete root;
			root = NULL;
			return true;
		}
		else
			return false;
	}
	//��ʼѰ��
	if (root->_key > key)
	{
		return _Remove_R(root->_left, key);
	}
	else if (root->_key < key)
	{
		return _Remove_R(root->_right, key);
	}
	else
	{ //�ҵ��ڵ�
		if (root->_left == NULL)
		{//������Ϊ�գ������������ӵ����ڵ�
			root = root->_right;
		}
		else if (root->_right == NULL)
		{//������Ϊ�գ������������ӵ����ڵ�
			root = root->_left;
		}
		else
		{//������������ΪNULL
			BSTNode<K, V>* firstleft = root->_right;
			BSTNode<K, V>* parent = root;
			//��������������С�ڵ�firstleft���ҵ��󽫸ýڵ�������Ҫɾ���ڵ����Խ���
			while (firstleft->_left)
			{
				parent = firstleft;
				firstleft = firstleft->_left;
			}
			swap(firstleft->_key, root->_key);
			swap(firstleft->_value, root->_value);
			//���������
			if (parent->_left == firstleft)
			{//firstleftΪҪɾ���ڵ���������еĽڵ�
				parent->_left = firstleft->_right;
			}
			else if (root->_right == firstleft)
			{//��С�ڵ�ΪҪɾ���ڵ��������ĸ��ڵ�
				root->_right = firstleft->_right;
			}
			delete firstleft;
			return true;
		}
	}
}
template<class K, class V>
void BSTree<K, V>::NonR_InOrderPrintTree()
{
	if (_root == NULL)
		return;
	stack<BSTNode<K, V>*> sc;
	BSTNode<K, V>* cur = _root;
	while (cur || !sc.empty())
	{
		while (cur)
		{
			sc.push(cur);
			cur = cur->_left;
		}
		if (!sc.empty())
		{
			cur = sc.top();
			cout << cur->_key << " ";
			sc.pop();
			cur = cur->_right;
		}
	}
	cout << endl;
}
template<class K, class V>
ostream& operator<<(ostream& out, BSTNode<K, V>*& bst)
{
	if (bst)
	{
		out << "BSTNode->key: " << bst->_key;
	}
	return out;
}