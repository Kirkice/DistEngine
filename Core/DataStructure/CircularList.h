#include <iostream>
using namespace std;

template<typename Type> class CircularList;

template<typename Type> class ListNode {
private:
	friend class CircularList<Type>;
	ListNode() :m_pnext(NULL) {}
	ListNode(const Type item, ListNode<Type>* next = NULL) :m_data(item), m_pnext(next) {}
	~ListNode() {
		m_pnext = NULL;
	}

private:
	Type m_data;
	ListNode* m_pnext;
};


template<typename Type> class CircularList {
public:
	CircularList() :head(new ListNode<Type>()) {
		head->m_pnext = head;
	}
	~CircularList() {
		MakeEmpty();
		delete head;
	}
public:
	void MakeEmpty();	//clear the list
	int Length();		//get the length
	ListNode<Type>* Find(Type value, int n);	//find the nth data which is equal to value
	ListNode<Type>* Find(int n);			//find the nth data
	bool Insert(Type item, int n = 0);			//insert the data into the nth data of the list
	Type Remove(int n = 0);					//delete the nth data
	bool RemoveAll(Type item);				//delete all the datas which are equal to value
	Type Get(int n);	//get the nth data
	void Print();		//print the list

private:
	ListNode<Type>* head;

};

template<typename Type> void CircularList<Type>::MakeEmpty() {
	ListNode<Type>* pdel, * pmove = head;
	while (pmove->m_pnext != head) {
		pdel = pmove->m_pnext;
		pmove->m_pnext = pdel->m_pnext;
		delete pdel;
	}
}

template<typename Type> int CircularList<Type>::Length() {
	ListNode<Type>* pmove = head;
	int count = 0;
	while (pmove->m_pnext != head) {
		pmove = pmove->m_pnext;
		count++;
	}
	return count;
}

template<typename Type> ListNode<Type>* CircularList<Type>::Find(int n) {
	if (n < 0) {
		cout << "The n is out of boundary" << endl;
		return NULL;
	}
	ListNode<Type>* pmove = head->m_pnext;
	for (int i = 0; i < n && pmove != head; i++) {
		pmove = pmove->m_pnext;
	}
	if (pmove == head) {
		cout << "The n is out of boundary" << endl;
		return NULL;
	}
	return pmove;
}

template<typename Type> ListNode<Type>* CircularList<Type>::Find(Type value, int n) {
	if (n < 1) {
		cout << "The n is illegal" << endl;
		return NULL;
	}
	ListNode<Type>* pmove = head;
	int count = 0;
	while (count != n) {
		pmove = pmove->m_pnext;
		if (pmove->m_data == value) {
			count++;
		}
		if (pmove == head) {
			cout << "can't find the element" << endl;
			return NULL;
		}
	}
	return pmove;
}

template<typename Type> bool CircularList<Type>::Insert(Type item, int n) {
	if (n < 0) {
		cout << "The n is out of boundary" << endl;
		return 0;
	}
	ListNode<Type>* pmove = head;
	ListNode<Type>* pnode = new ListNode<Type>(item);
	if (pnode == NULL) {
		cout << "Application error!" << endl;
		exit(1);
	}
	for (int i = 0; i < n; i++) {
		pmove = pmove->m_pnext;
		if (pmove == head) {
			cout << "The n is out of boundary" << endl;
			return 0;
		}
	}

	pnode->m_pnext = pmove->m_pnext;
	pmove->m_pnext = pnode;
	return 1;
}

template<typename Type> bool CircularList<Type>::RemoveAll(Type item) {
	ListNode<Type>* pmove = head;
	ListNode<Type>* pdel = head->m_pnext;
	while (pdel != head) {
		if (pdel->m_data == item) {
			pmove->m_pnext = pdel->m_pnext;
			delete pdel;
			pdel = pmove->m_pnext;
			continue;
		}
		pmove = pmove->m_pnext;
		pdel = pdel->m_pnext;
	}
	return 1;
}

template<typename Type> Type CircularList<Type>::Remove(int n) {
	if (n < 0) {
		cout << "can't find the element" << endl;
		exit(1);
	}
	ListNode<Type>* pmove = head, * pdel;
	for (int i = 0; i < n && pmove->m_pnext != head; i++) {
		pmove = pmove->m_pnext;
	}
	if (pmove->m_pnext == head) {
		cout << "can't find the element" << endl;
		exit(1);
	}
	pdel = pmove->m_pnext;
	pmove->m_pnext = pdel->m_pnext;
	Type temp = pdel->m_data;
	delete pdel;
	return temp;
}

template<typename Type> Type CircularList<Type>::Get(int n) {
	if (n < 0) {
		cout << "The n is out of boundary" << endl;
		exit(1);
	}
	ListNode<Type>* pmove = head->m_pnext;
	for (int i = 0; i < n; i++) {
		pmove = pmove->m_pnext;
		if (pmove == head) {
			cout << "The n is out of boundary" << endl;
			exit(1);
		}
	}
	return pmove->m_data;
}

template<typename Type> void CircularList<Type>::Print() {
	ListNode<Type>* pmove = head->m_pnext;
	cout << "head";
	while (pmove != head) {
		cout << "--->" << pmove->m_data;
		pmove = pmove->m_pnext;
	}
	cout << "--->over" << endl << endl << endl;
}