#pragma once

//	AVL���Ľڵ���
template<class K, class V>
struct AVLTreeNode
{
	K _key;
	V _value;
	int  _bf;			//ƽ������ -1,0,1		(ÿ���ڵ��ƽ�����ӵ����������ĸ߶ȼ�ȥ�������ĸ߶�) 
	AVLTreeNode<K, V>* _parent;	//ָ�򸸽ڵ��ָ��
	AVLTreeNode<K, V>* _left;			//ָ�����ӵ�ָ��
	AVLTreeNode<K, V>* _right;		//ָ���Һ��ӵ�ָ��

	AVLTreeNode(const K& key = K(), const V& value = V())
		:_key(key)
		, _value(value)
		, _bf(0)
		, _parent(NULL)
		, _left(NULL)
		, _right(NULL)
	{}
};

template<class K, class V>
class AVLTree
{
public:
	AVLTree()
		:_root(NULL)
	{}
	bool Insert(const K& key, const V& value);  //��������
	//�����ӡ
	void  InOrder()
	{
		_InOrder(_root);
		cout << endl;
	}
	//�ж�AVLTree���Ƿ�ƽ��
	bool IsBalance()
	{
		return _IsBalance(_root);
	}
	//�������ĸ߶�
	int Height()
	{
		return _Height(_root);
	}
protected:
	void _RotateR(AVLTreeNode<K, V>*& parent);
	void _RotateL(AVLTreeNode<K, V>*& parent);
	void _RotateLR(AVLTreeNode<K, V>*& parent);
	void _RotateRL(AVLTreeNode<K, V>*& parent);
	void _InOrder(AVLTreeNode<K, V>* root);
	bool _IsBalance(AVLTreeNode<K, V>* root);
	int _Height(AVLTreeNode<K, V>* root);
protected:
	AVLTreeNode<K, V>* _root; //���ڵ�
};

template<class K, class V>
bool AVLTree<K, V>::Insert(const K& key, const V& value)
{
	//1.����
	if (_root == NULL)
	{
		_root = new AVLTreeNode<K, V>(key, value);
		return true;
	}

	//2.AVL����ΪNULL
	AVLTreeNode<K, V>* parent = NULL;
	AVLTreeNode<K, V>* cur = _root;
	//�ҵ����ݲ���λ��
	while (cur)
	{
		if (cur->_key < key)
		{
			parent = cur;
			cur = cur->_right;
		}
		else	if (cur->_key > key)
		{
			parent = cur;
			cur = cur->_left;
		}
		else
		{
			return false;
		}
	}
	//��������
	cur = new AVLTreeNode<K, V>(key, value);
	cur->_parent = parent;
	if (parent->_key > key)
		parent->_left = cur;
	else
		parent->_right = cur;

	while (parent)
	{
		//����ƽ������
		if (cur == parent->_left)
			parent->_bf--;
		else if (cur == parent->_right)
			parent->_bf++;

		//����ƽ�������Ƿ�Ϸ�
		if (parent->_bf == 0)
			break;
		else if (parent->_bf == -1 || parent->_bf == 1)
		{	// �������� �����游�ڵ��ƽ�����Ӳ�����Ϸ���
			cur = parent;
			parent = cur->_parent;
		}
		else   //	2 -2 ƽ�����Ӳ��Ϸ� ��Ҫ������ת ���͸߶�
		{
			if (parent->_bf == 2)
			{
				if (cur->_bf == 1)
					_RotateL(parent);
				else
					_RotateRL(parent);
			}
			else if (parent->_bf == -2)
			{
				if (cur->_bf == -1)
					_RotateR(parent);
				else
					_RotateLR(parent);
			}
			break;
		}
	}
}
///����
template<class K, class V>
void AVLTree<K, V>::_RotateR(AVLTreeNode<K, V>*& parent)
{
	AVLTreeNode<K, V>* subL = parent->_left;
	AVLTreeNode<K, V>* subLR = subL->_right;
	AVLTreeNode<K, V>* ppNode = parent->_parent;		//������Ƚڵ�
	//1.����parent���� ��parent��subLR��������
	parent->_left = subLR;
	if (subLR) subLR->_parent = parent;
	//2.����subL���� ��subL��parent��������
	subL->_right = parent;
	parent->_parent = subL;
	//3.�����Ƚڵ���sunL��������
	if (ppNode == NULL)
	{	//�������ΪNULL��˵����ǰsubL�ڵ�Ϊ���ڵ�
		subL->_parent = NULL;
		_root = subL;
	}
	else
	{
		subL->_parent = ppNode;
		if (ppNode->_left == parent)
			ppNode->_left = subL;
		else if (ppNode->_right == parent)
			ppNode->_right = subL;
	}
	//4.����ƽ������
	parent->_bf = 0;
	subL->_bf = 0;
	//5.����subLΪ��ǰ���ڵ�
	parent = subL;
}
//����
template<class K, class V>
void AVLTree<K, V>::_RotateL(AVLTreeNode<K, V>*& parent)
{
	AVLTreeNode<K, V>* subR = parent->_right;
	AVLTreeNode<K, V>* subRL = subR->_left;
	AVLTreeNode<K, V>* ppNode = parent->_parent;		//������Ƚڵ�

	//1.����parent���� ����parent��subRL
	parent->_right = subRL;
	if (subRL) subRL->_parent = parent;
	//2.����subR���� ����parent��subR
	subR->_left = parent;
	parent->_parent = subR;
	//3.�������Ƚڵ��subR�ڵ�
	subR->_parent = ppNode;
	if (ppNode == NULL)
	{//������Ƚڵ�ΪNULL��˵��Ŀǰ�ĸ��ڵ�ΪsubR
		_root = subR;
	}
	else
	{	//�����Ƚڵ��subR�ڵ���������
		if (parent == ppNode->_left)
			ppNode->_left = subR;
		else
			ppNode->_right = subR;
	}
	//4.����ƽ������
	parent->_bf = 0;
	subR->_bf = 0;
	//5.����subRΪ��ǰ���ڵ�
	parent = subR;
}
//����˫��
template<class K, class V>
void AVLTree<K, V>::_RotateLR(AVLTreeNode<K, V>*& parent)
{
	AVLTreeNode<K, V>* pNode = parent;
	AVLTreeNode<K, V>* subL = parent->_left;
	AVLTreeNode<K, V>* subLR = subL->_right;
	int bf = subLR->_bf;

	_RotateL(parent->_left);
	_RotateR(parent);

	if (bf == 1)
	{
		pNode->_bf = 0;
		subL->_bf = -1;
	}
	else if (bf == -1)
	{
		pNode->_bf = 1;
		subL->_bf = 0;
	}
	else
	{
		pNode->_bf = 0;
		subL->_bf = 0;
	}

}
//����˫��
template<class K, class V>
void AVLTree<K, V>::_RotateRL(AVLTreeNode<K, V>*& parent)
{
	AVLTreeNode<K, V>* pNode = parent;
	AVLTreeNode<K, V>* subR = parent->_right;
	AVLTreeNode<K, V>* subRL = subR->_left;
	int bf = subRL->_bf;

	_RotateR(parent->_right);
	_RotateL(parent);

	if (bf == 1)
	{
		pNode->_bf = 0;
		subR->_bf = -1;
	}
	else if (bf == -1)
	{
		pNode->_bf = 1;
		subR->_bf = 0;
	}
	else
	{
		pNode->_bf = 0;
		subR->_bf = 0;
	}
}
//�����ӡ
template<class K, class V>
void AVLTree<K, V>::_InOrder(AVLTreeNode<K, V>* root)
{
	if (root == NULL)
		return;
	_InOrder(root->_left);
	cout << root->_key << " ";
	_InOrder(root->_right);
}
//��AVL���ĸ߶�
template<class K, class V>
int AVLTree<K, V>::_Height(AVLTreeNode<K, V>* root)
{
	if (root == NULL)
		return 0;

	int high = 0;
	int left = _Height(root->_left);
	int right = _Height(root->_right);

	if (left > right)
		high = left;
	else
		high = right;

	if (root != _root)return 1 + high;
	else
		return high;
}
//����AVL���Ƿ�ʧ��
template<class K, class V>
bool AVLTree<K, V>::_IsBalance(AVLTreeNode<K, V>* root)
{
	if (root == NULL)
		return true;
	int bf = _Height(root->_right) - _Height(root->_left);
	if (root->_bf != bf)
	{
		cout << root->_key << endl;
		return false;
	}
	//bf�ľ���ֵС��2������������������ƽ������rootΪ���������Ŵ���ƽ��״̬
	return abs(bf) < 2 && _IsBalance(root->_left) && _IsBalance(root->_right);
}
