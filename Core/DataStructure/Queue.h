template <typename T>
class Queue
{
private:
	class Node
	{
	public:
		T item;
		Node* next;
		Node() {}
		Node(T item, Node* next) {
			this->item = item;
			this->next = next;
		}
		Node(Node& node) {
			this->item = node.item;
			this->next = node.next;
		}
	};
	Node* head;
	Node* last;
	int num;
public:
	Queue() {
		head = new Node(0, nullptr);
		last = nullptr;
		num = 0;
	}
	~Queue() {
		while (!isempty()) {
			dequeue();
		}
		delete head;
	}
	//判断队列是否为空
	bool isempty() {
		if (num == 0) return true;
		else return false;
	}
	//获取队列中元素的个数
	int size() {
		return num;
	}
	//进队列
	void enqueue(T t) {
		//如果当前队列为空
		if (last == nullptr) {
			last = new Node(t, nullptr);
			head->next = last;
		}
		else {
			Node* oldlast = last;
			last = new Node(t, nullptr);
			oldlast->next = last;
		}
		num++;
	}
	//出队列
	T dequeue() {
		if (isempty()) {
			return NULL;
		}
		//将head结点的下一个结点删除
		Node* oldNode = head->next;
		T items = oldNode->item;
		head->next = oldNode->next;
		delete oldNode;
		num--;
		//如果删除的是最后一个元素，则将last指针置空
		if (isempty()) {
			last = nullptr;
		}
		return items;
	}
};
