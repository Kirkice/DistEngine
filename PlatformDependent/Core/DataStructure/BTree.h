#pragma once

template<class K, int M = 3>
struct BTreeNode
{
	K _key[M];	//�ؼ��� (��Ч�ؼ��ָ���ΪM-1)
	BTreeNode<K, M>* _sub[M + 1];	//����������ָ������
	size_t _size;		//�ڵ��йؼ��ֵĸ���
	BTreeNode<K, M>* _parent;	//ָ�򸸽ڵ��ָ��

	BTreeNode()
		:_size(0)
		, _parent(NULL)
	{
		for (size_t i = 0; i < M + 1; i++)
		{
			_sub[i] = NULL;
		}
	}
};

template<class K, class V>
struct Pair
{
	K _first;
	V _second;

	Pair(const K& k = K(), const V& v = V())
		:_first(k)
		, _second(v)
	{}
};

template<class K, int M = 3>
class BTree
{
public:
	BTree()
		:_root(NULL)
	{}
	//�ݹ����key
	Pair<BTreeNode<K, M>*, int> Find(const K& key)
	{
		BTreeNode<K, M>* parent = NULL;
		BTreeNode<K, M>* cur = _root;
		//�����ؼ�������_key[]�����key����С�� ++i �������������
		// 1.���key=_key[i]��ֹͣ���������ظýṹ��ڵ�
		// 2.���key��������ֹͣ��������ʱ������_sub[i]ָ��Ĺؼ���������������ݶ��ǽ���_key[i-1]��_key[i]֮������ݣ�����Ҫ�ҵ�key�����������
		// 3.�������ѭ����δ�ҵ�������cur=NULL������cur�ĸ��ڵ㣻��ʱ�����ǲ���key���Ͳ��뵽parentָ��Ĺؼ���������
		while (cur != NULL)
		{
			size_t i = 0;
			while (i < cur->_size && cur->_key[i] < key)
				++i;
			if (cur->_key[i] == key)
				return Pair<BTreeNode<K, M>*, int>(cur, i);
			// key<_key[i] ��������key[i]�±���ͬ������
			parent = cur;
			cur = cur->_sub[i];
		}
		return Pair<BTreeNode<K, M>*, int>(parent, -1);
	}
	//��������
	bool Insert(K& key)
	{
		// 1.B-��Ϊ��
		if (NULL == _root)
		{
			_root = new BTreeNode<K, M>;
			_root->_key[0] = key;
			++_root->_size;
			return true;
		}

		Pair<BTreeNode<K, M>*, int> ret = Find(key);
		// 2.�������Ѿ�����
		if (ret._second != -1)
			return false;

		// 3.�������ݵ��ؼ�������
		BTreeNode<K, M>* cur = ret._first;
		BTreeNode<K, M>* sub = NULL;
		while (1)
		{
			int i = 0;
			for (i = cur->_size - 1; i >= 0; )
			{ // �Ѵ�������Ų����Ӧ����ҲҪ����Ų��
				if (cur->_key[i] > key)
				{
					cur->_key[i + 1] = cur->_key[i];
					cur->_sub[i + 2] = cur->_sub[i + 1];
					i--;
				}
				else
				{
					break;
				}
			}
			cur->_key[i + 1] = key;
			cur->_sub[i + 2] = sub;
			if (sub != NULL)
				cur->_sub[i + 2]->_parent = cur;
			cur->_size++;

			//�ؼ�������δ��������ɹ�
			if (cur->_size < M)
				return true;

			//�ؼ���������������Ҫ���з���
			int mid = M / 2;
			//����
			BTreeNode<K, M>* tmp = new BTreeNode<K, M>;
			int index = 0;
			size_t k;
			for (k = mid + 1; k < cur->_size; k++)
			{
				tmp->_key[index] = cur->_key[k];
				if (cur->_sub[k] != NULL)
				{
					tmp->_sub[index] = cur->_sub[k];
					cur->_sub[k] = NULL;
					tmp->_sub[index]->_parent = tmp;
				}
				tmp->_size++;
				cur->_size--;
				index++;
			}
			if (cur->_sub[k] != NULL)
			{
				tmp->_sub[index] = cur->_sub[k];
				cur->_sub[k] = NULL;
				tmp->_sub[index]->_parent = tmp;
			}
			//���ڵ�Ϊ��ʱ������
			if (cur->_parent == NULL)
			{
				_root = new BTreeNode<K, M>;
				_root->_key[0] = cur->_key[mid];
				cur->_size--;
				_root->_sub[0] = cur;
				_root->_sub[1] = tmp;
				_root->_size++;

				//����
				tmp->_parent = _root;
				cur->_parent = _root;
				return true;
			}
			//���ڵ㲻Ϊ��ʱ������
			key = cur->_key[mid];
			cur->_size--;
			cur = cur->_parent;
			sub = tmp;
		}
	}

	void InOrder()
	{
		_InOrder(_root);
	}
protected:
	void _InOrder(BTreeNode<K, M>* root)
	{
		if (root == NULL)
			return;
		size_t i = 0;
		for (; i < root->_size; i++)
		{
			_InOrder(root->_sub[i]);
			cout << root->_key[i] << " ";
		}
		_InOrder(root->_sub[i]);
	}

protected:
	BTreeNode<K, M>* _root;
};