#ifndef __CIRCULAR_QUEUE_H__
#define __CIRCULAR_QUEUE_H__
#include <stddef.h>

template<typename T>
class CircularQueue
{
public:
	explicit CircularQueue(size_t maxsize)
		: maxsize_(maxsize + 1), head_(0), rear_(0)
	{
		array_ = new T[maxsize_];
	}

	CircularQueue(size_t maxsize, const T& val)
		: maxsize_(maxsize + 1), head_(0), rear_(0)
	{
		array_ = new T[maxsize_];
		for (size_t i = 0; i != maxsize; ++i)
		{
			array_[i] = val;
		}
		rear_ = maxsize;
	}

	CircularQueue(const CircularQueue& rhs)
		: maxsize_(rhs.maxsize_), head_(rhs.head_), rear_(rhs.rear_)
	{
		array_ = new T[maxsize_];
		for (int i = 0; i != maxsize_; ++i)
		{
			array_[i] = rhs.array_[i];
		}
	}

	~CircularQueue()
	{
		delete[] array_;
	}

	CircularQueue& operator=(const CircularQueue& rhs)
	{
		if (this == &rhs)
		{
			return *this;
		}
		delete[] array_;
		maxsize_ = rhs.maxsize_;
		head_ = rhs.head_;
		rear_ = rhs.rear_;
		array_ = new T[maxsize_];
		for (int i = 0; i != maxsize_; ++i)
		{
			array_[i] = rhs.array_[i];
		}
		return *this;
	}

	bool empty() const
	{
		return head_ == rear_;
	}

	size_t size() const
	{
		return (rear_ - head_ + maxsize_) % maxsize_;
	}

	T& front()
	{
		return array_[head_];
	}

	const T& front() const
	{
		return array_[head_];
	}

	void push(const T& val)
	{
		if ((rear_ + 1) % maxsize_ != head_)
		{
			array_[rear_] = val;
			rear_ = (rear_ + 1) % maxsize_;
		}
	}

	void pop()
	{
		if (head_ != rear_)
		{
			head_ = (head_ + 1) % maxsize_;
		}
	}

private:
	size_t  maxsize_;
	int     head_;
	int     rear_;
	T* array_;
};

#endif