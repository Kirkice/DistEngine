#include<iostream>
#include<string>
using namespace std;
template<typename T, int V>
class Stack {
	T* data;
	int top;
public:
	Stack();
	~Stack();
	void Push(T e);
	int Top();
	void Pop();
};
template<typename T, int V>
Stack<T, V>::Stack() {
	data = new T[V];
	top = -1;
}
template<typename T, int V>
Stack<T, V>::~Stack() {
	delete[]data;
}
template<typename T, int V>
void Stack<T, V>::Push(T e) {
	if (top == V - 1)cout << "Full" << endl;
	else {
		top++;
		data[top] = e;
	}
}
template<typename T, int V>
int Stack<T, V>::Top() {
	if (top == -1)
	{
		string str;
		str = "Empty";
		throw str;
	}
	else {
		int e;
		e = data[top];
		return e;
	}
}
template<typename T, int V>
void Stack<T, V>::Pop() {
	if (top != -1)
	{
		int e;
		e = data[top];
		top--;
	}
}