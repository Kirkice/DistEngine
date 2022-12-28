#define _CRT_SECURE_NO_WARNINGS 1

#include<iostream>
#include<vector>
#include<assert.h>
using namespace std;

//利用容器适配器实现大小堆

template<class T>
struct Less
{
	bool operator()(const T& left, const T& parent)
	{
		return left < parent;
	}
};

template<class T>
struct Greater
{
	bool operator()(const T& left, const T& parent)
	{
		return left > parent;
	}
};

//template<class T>
template <class T, template<class> class Compare = Greater>  //默认实现大堆
class Heap
{
public:
	Heap()
	{}
	//构造函数
	Heap(const T* array, size_t size)
	{
		for (size_t i = 0; i < size; i++)
		{
			_array.push_back(array[i]);
		}
		//建堆
		for (int start = (_array.size() - 1 - 1) / 2; start >= 0; --start)
		{
			// 从下往上 并 从第一个有孩子的父节点开始 进行下调工作
			AdjustDown(start);
		}
	}
	//拷贝构造
	Heap(const Heap& hp)
	{
		//_array.reserve(hp._array.size());
		for (size_t i = 0; i < hp._array.size(); i++)
		{
			_array.push_back(hp._array[i]);
		}
	}
	//赋值运算符重载
	Heap& operator=(Heap hp)
	{
		if (this->_array != hp._array)
		{
			swap(hp._array, _array);
		}
		return *this;
	}
	//添加数据
	void Push(const T x)
	{
		// 尾插 然后 上调
		_array.push_back(x);
		AdjustUp(_array.size() - 1);
	}
	//删除堆顶
	void Pop()
	{
		assert(_array.size() > 0);
		swap(_array[0], _array[_array.size() - 1]);
		//_array[0] = _array[_array.size() - 1];
		_array.pop_back();
		AdjustDown(0);
	}
	//删除数据
	bool PopData(T data)
	{
		assert(_array.size() > 0);
		int index = GetIndex(data);
		if (index != -1)
		{
			swap(_array[index], _array[_array.size() - 1]); 
			_array.pop_back();
			AdjustDown(index);
			return true;
		}
		return false;
	}
	//访问根节点
	T& Top()
	{
		assert(_array.size() > 0);
		return _array[0];
	}
	//返回data在二叉堆中的索引
	int GetIndex(T data)
	{
		assert(_array.size() > 0);
		for (int i = 0; i < _array.size(); i++)
		{
			if (_array[i] == data)
			{
				return i;
			}
		}
		return -1;
	}
	//打印
	void Print()
	{
		if (_array.size() == 0)
			return;
		for (int i = 0; i < _array.size(); i++)
		{
			cout << _array[i] << " ";
		}
		cout << endl;
	}
protected:
	//上调
	void AdjustDown(int parent)    // 建小堆 将大数向下调（建大堆 将小数向下调）
	{
		size_t left = parent * 2 + 1;
		size_t right = left + 1;
		Compare<T> cp;           //如果是大堆传过来就是用大堆的逻辑，小堆就实现小堆的逻辑
		while (left < _array.size())
		{
			// 比较左右孩子，保证下标left为较小或较大的节点下标
			if (right < _array.size() && cp(_array[right], _array[left]))
			{
				left = right;
			}
			if (left < _array.size() && cp(_array[left], _array[parent]))
			{
				// 交换之后还需继续 将相对较大或相对较小的数循环向下调
				swap(_array[left], _array[parent]);
				parent = left;
				left = parent * 2 + 1;
				right = left + 1;
			}
			else
			{
				break;
			}
		}
	}
	//上调
	void AdjustUp(int child)
	{
		// 除了尾插的数x 其它数据已经按照堆排序排好了 所以只需将刚刚尾插的数x不断往上调 直到将它放到合适的位置  
		int parent = (child - 1) / 2;
		Compare<T> cp;  //如果是大堆传过来就是用大堆的逻辑，小堆就实现小堆的逻辑
		while (child > 0)
		{
			if (cp(_array[child], _array[parent]))
			{
				swap(_array[parent], _array[child]);
				child = parent;
				parent = (child - 1) / 2;
			}
			else
			{
				// 如果跳到这里，就说明次序已经排好，上面的数都小于x
				break;
			}
		}
	}

private:
	vector<T> _array;   //用vector存储堆中数据
};

template <class T, template<class> class Compare = Greater>
class PriorityQueue
{
public:
	void Push(const T x)
	{
		_heap.Push(x);
	}
	void Pop()
	{
		_heap.Pop();
	}
	T& Top()
	{
		return _heap.Top();
	}
protected:
	Heap<T, Compare> _heap;
};
