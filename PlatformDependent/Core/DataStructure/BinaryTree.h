#ifndef BINARY_TREE_H
#define BINARY_TREE_H

#include <array>
#include <memory>
#include <type_traits>

template <typename T> class _binary_tree_node;
template <typename NodeType> class _binary_tree_baseIterator;
template <typename T> class _binary_tree_iterator;
template <typename T> class _binary_tree_const_iterator;
template <typename T> class binary_tree;

namespace
{
	constexpr int ALLOC_SIZE = 16;
} // namespace

template <typename T> class _binary_tree_node
{
	template <typename> friend class binary_tree;

public:
	using value_type = T;
	using reference = T&;
	using const_reference = const T&;
	using pointer = T*;
	using const_pointer = const T*;

private:
	value_type _data;
	std::array<_binary_tree_node*, 3> _linked_nodes;

public:
	_binary_tree_node() = default;
	~_binary_tree_node() = default;
	_binary_tree_node(const _binary_tree_node& other);
	_binary_tree_node(_binary_tree_node&& other);
	_binary_tree_node& operator=(const _binary_tree_node& other);
	_binary_tree_node& operator=(_binary_tree_node&& other);

	reference data();
	const_reference data() const;
	std::array<_binary_tree_node*, 3>& linked_nodes();
	const std::array<_binary_tree_node*, 3>& linked_nodes() const;
};

template <typename NodeType> class _binary_tree_baseIterator
{
	template <typename> friend class _binary_tree_iterator;
	template <typename> friend class _binary_tree_const_iterator;
	template <typename> friend class binary_tree;

public:
	// using node_type = typename std::remove_const<NodeType>::type;
	using node_type = NodeType;
	using node_pointer = node_type*;
	using value_type = typename std::conditional<std::is_const<NodeType>::value, const typename NodeType::value_type,
		typename node_type::value_type>::type;
	using reference = value_type&;

	using pointer = value_type*;

protected:
	node_pointer _node;

public:
	~_binary_tree_baseIterator() = default;
	_binary_tree_baseIterator();
	_binary_tree_baseIterator(node_pointer _pnode);
	_binary_tree_baseIterator& operator=(node_pointer _pnode);
	_binary_tree_baseIterator(const _binary_tree_baseIterator& other);
	_binary_tree_baseIterator& operator=(const _binary_tree_baseIterator& other);
	bool operator==(_binary_tree_baseIterator other) const;

	_binary_tree_baseIterator parent() const;
	_binary_tree_baseIterator left_son() const;
	_binary_tree_baseIterator right_son() const;
	_binary_tree_baseIterator set_left_son(_binary_tree_baseIterator lson) const;
	_binary_tree_baseIterator set_right_son(_binary_tree_baseIterator rson) const;

	operator bool() const;
	reference operator*() const;
	pointer operator->() const;
};

template <typename T> class _binary_tree_iterator : public _binary_tree_baseIterator<_binary_tree_node<T>>
{
public:
	using _baseIt = _binary_tree_baseIterator<_binary_tree_node<T>>;
	using node_type = typename _baseIt::node_type;
	using node_pointer = typename _baseIt::node_pointer;
	using _baseIt::_baseIt;
	using value_type = T;
	using reference = T&;
	using const_reference = const T&;
	using pointer = T*;
	using const_pointer = const T*;

	_binary_tree_iterator(_baseIt _base);
};

template <typename T> class _binary_tree_const_iterator : public _binary_tree_baseIterator<const _binary_tree_node<T>>
{
public:
	using _baseIt = _binary_tree_baseIterator<const _binary_tree_node<T>>;
	using _NonConstIt = _binary_tree_iterator<T>;
	using node_type = typename _baseIt::node_type;
	using node_pointer = typename _baseIt::node_pointer;
	using _baseIt::_baseIt;
	using value_type = T;
	using reference = T&;
	using const_reference = const T&;
	using pointer = T*;
	using const_pointer = const T*;

public:
	_binary_tree_const_iterator(_baseIt _base);
	_binary_tree_const_iterator(_NonConstIt _nonConstIt);
	_binary_tree_const_iterator& operator=(_NonConstIt _nonConstIt);

	_binary_tree_const_iterator set_left_son(_binary_tree_const_iterator lson) const = delete;
	_binary_tree_const_iterator set_right_son(_binary_tree_const_iterator rson) const = delete;
};

template <typename T> class binary_tree
{
public:
	using node_type = _binary_tree_node<T>;
	using const_node_type = const node_type;
	using node_pointer = node_type*;
	using const_node_pointer = const node_type*;
	using value_type = T;
	using reference = T&;
	using const_reference = const T&;
	using pointer = T*;
	using const_pointer = const T*;
	using iterator = _binary_tree_iterator<T>;
	using const_iterator = _binary_tree_const_iterator<T>;

private:
	std::allocator<node_type> _alloc;
	node_pointer _root;

private:
	size_t erase_r(iterator it);

public:
	binary_tree();
	binary_tree(const_iterator otherIt);
	binary_tree(const binary_tree& other);
	binary_tree(binary_tree&& other);
	binary_tree& operator=(const binary_tree& other);
	~binary_tree();

	iterator root();
	const_iterator root() const;

	void add_left(iterator it, const T& val = value_type());
	void add_left(iterator it, T&& val);
	void add_right(iterator it, const T& val = value_type());
	void add_right(iterator it, T&& val);

	void copy(const_iterator inIt, iterator outIt);
	size_t erase(iterator it);
};

template <typename T>
_binary_tree_node<T>::_binary_tree_node(const _binary_tree_node& other)
	: _data(other._data), _linked_nodes(other._linked_nodes)
{
}

template <typename T>
_binary_tree_node<T>::_binary_tree_node(_binary_tree_node&& other)
	: _data(std::move(other._data)), _linked_nodes(std::move(other._linked_nodes))
{
}

template <typename T> _binary_tree_node<T>& _binary_tree_node<T>::operator=(const _binary_tree_node& other)
{
	_data = other._data;
	_linked_nodes = other._linked_nodes;
	return *this;
}

template <typename T> _binary_tree_node<T>& _binary_tree_node<T>::operator=(_binary_tree_node&& other)
{
	_data = std::move(other._data);
	_linked_nodes = std::move(other._linked_nodes);
	return *this;
}

template <typename T> T& _binary_tree_node<T>::data()
{
	return _data;
}

template <typename T> const T& _binary_tree_node<T>::data() const
{
	return _data;
}

template <typename T> std::array<_binary_tree_node<T>*, 3>& _binary_tree_node<T>::linked_nodes()
{
	return _linked_nodes;
}

template <typename T> const std::array<_binary_tree_node<T>*, 3>& _binary_tree_node<T>::linked_nodes() const
{
	return _linked_nodes;
}

template <typename NodeType>
_binary_tree_baseIterator<NodeType>::_binary_tree_baseIterator(node_pointer _pnode) : _node(_pnode)
{
}

template <typename NodeType>
_binary_tree_baseIterator<NodeType>& _binary_tree_baseIterator<NodeType>::operator=(node_pointer _pnode)
{
	_node = _pnode;
	return *this;
}

template <typename NodeType> _binary_tree_baseIterator<NodeType>::_binary_tree_baseIterator() : _node(nullptr)
{
}

template <typename NodeType>
_binary_tree_baseIterator<NodeType>::_binary_tree_baseIterator(const _binary_tree_baseIterator& other)
	: _node(other._node)
{
}

template <typename NodeType>
_binary_tree_baseIterator<NodeType>& _binary_tree_baseIterator<NodeType>::operator=(
	const _binary_tree_baseIterator& other)
{
	_node = other._node;
	return *this;
}

template <typename NodeType> bool _binary_tree_baseIterator<NodeType>::operator==(_binary_tree_baseIterator other) const
{
	return _node == other._node;
}

template <typename NodeType> _binary_tree_baseIterator<NodeType> _binary_tree_baseIterator<NodeType>::parent() const
{
	return _node->linked_nodes()[0];
}

template <typename NodeType> _binary_tree_baseIterator<NodeType> _binary_tree_baseIterator<NodeType>::left_son() const
{
	return _node->linked_nodes()[1];
}

template <typename NodeType> _binary_tree_baseIterator<NodeType> _binary_tree_baseIterator<NodeType>::right_son() const
{
	return _node->linked_nodes()[2];
}

template <typename NodeType>
_binary_tree_baseIterator<NodeType> _binary_tree_baseIterator<NodeType>::set_left_son(
	_binary_tree_baseIterator lson) const
{
	static_assert(!std::is_const<NodeType>::value, "can not modify the data with const_iterator.");

	auto tmp = _node->linked_nodes()[1];
	_node->linked_nodes()[1] = lson._node;
	if (lson)
	{
		lson._node->linked_nodes()[0] = _node;
	}
	return tmp;
}

template <typename NodeType>
_binary_tree_baseIterator<NodeType> _binary_tree_baseIterator<NodeType>::set_right_son(
	_binary_tree_baseIterator rson) const
{
	static_assert(!std::is_const<NodeType>::value, "can not modify the data with const_iterator.");

	auto tmp = _node->linked_nodes()[2];
	_node->linked_nodes()[2] = rson._node;
	if (rson)
	{
		rson._node->linked_nodes()[0] = _node;
	}
	return tmp;
}

template <typename NodeType> _binary_tree_baseIterator<NodeType>::operator bool() const
{
	return bool(_node);
}

template <typename NodeType>
typename _binary_tree_baseIterator<NodeType>::reference _binary_tree_baseIterator<NodeType>::operator*() const
{
	return _node->data();
}

template <typename NodeType>
typename _binary_tree_baseIterator<NodeType>::pointer _binary_tree_baseIterator<NodeType>::operator->() const
{
	return &(_node->data());
}

template <typename T> _binary_tree_iterator<T>::_binary_tree_iterator(_baseIt _base) : _baseIt(_base._node)
{
}

template <typename T> _binary_tree_const_iterator<T>::_binary_tree_const_iterator(_baseIt _base) : _baseIt(_base._node)
{
}

template <typename T>
_binary_tree_const_iterator<T>::_binary_tree_const_iterator(_NonConstIt _nonConstIt) : _baseIt(_nonConstIt._node)
{
}

template <typename T> _binary_tree_const_iterator<T>& _binary_tree_const_iterator<T>::operator=(_NonConstIt _nonConstIt)
{
	this->_node = _nonConstIt._node;
	return *this;
}

template <typename T> binary_tree<T>::binary_tree() : _root(_alloc.allocate(1))
{
	_root->_linked_nodes.fill(nullptr);
}

template <typename T> binary_tree<T>::binary_tree(const_iterator otherIt) : binary_tree()
{
	copy(otherIt, _root);
}

template <typename T> binary_tree<T>::binary_tree(const binary_tree& other) : binary_tree()
{
	copy(other._root, _root);
}

template <typename T> binary_tree<T>::binary_tree(binary_tree&& other) : _root(other._root)
{
}

template <typename T> binary_tree<T>& binary_tree<T>::operator=(const binary_tree& other)
{
	copy(other._root, _root);
	return *this;
}

template <typename T> binary_tree<T>::~binary_tree()
{
	erase(root());
}

template <typename T> typename binary_tree<T>::iterator binary_tree<T>::root()
{
	return iterator(_root);
}

template <typename T> typename binary_tree<T>::const_iterator binary_tree<T>::root() const
{
	return const_iterator(_root);
}

template <typename T> void binary_tree<T>::add_left(iterator it, const T& val)
{
	erase(it.set_left_son(_alloc.allocate(1)));
	it.left_son()._node->_data = val;
}

template <typename T> void binary_tree<T>::add_left(iterator it, T&& val)
{
	erase(it.set_left_son(_alloc.allocate(1)));
	it.left_son()._node->_data = std::move(val);
}

template <typename T> void binary_tree<T>::add_right(iterator it, const T& val)
{
	erase(it.set_right_son(_alloc.allocate(1)));
	it.right_son()._node->_data = val;
}

template <typename T> void binary_tree<T>::add_right(iterator it, T&& val)
{
	erase(it.set_right_son(_alloc.allocate(1)));
	it.right_son()._node->_data = std::move(val);
}

template <typename T> void binary_tree<T>::copy(const_iterator inIt, iterator outIt)
{
	*outIt = *inIt;
	if (inIt.left_son())
	{
		if (!outIt.left_son())
			add_left(outIt);
		copy(inIt.left_son(), outIt.left_son());
	}
	else if (outIt.left_son())
	{
		erase(outIt.left_son());
	}

	if (inIt.right_son())
	{
		if (!outIt.right_son())
			add_right(outIt);
		copy(inIt.right_son(), outIt.right_son());
	}
	else if (outIt.right_son())
	{
		erase(outIt.right_son());
	}
}

template <typename T> size_t binary_tree<T>::erase(iterator it)
{
	if (it)
	{
		if (it.parent())
		{
			if (it.parent().left_son() == it)
			{
				it.parent().set_left_son(nullptr);
			}
			else
			{
				it.parent().set_right_son(nullptr);
			}
		}
		return erase_r(it);
	}
	return 0;
}

template <typename T> size_t binary_tree<T>::erase_r(iterator it)
{
	size_t erase_cnt = 0;
	if (it)
	{
		erase_cnt++;
		erase_cnt += erase(it.left_son());
		erase_cnt += erase(it.right_son());
		_alloc.destroy(it._node);
		_alloc.deallocate(it._node, 1);
	}
	return erase_cnt;
}

#endif // BINARY_TREE_H
