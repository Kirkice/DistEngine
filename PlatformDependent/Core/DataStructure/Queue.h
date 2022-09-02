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
	//�ж϶����Ƿ�Ϊ��
	bool isempty() {
		if (num == 0) return true;
		else return false;
	}
	//��ȡ������Ԫ�صĸ���
	int size() {
		return num;
	}
	//������
	void enqueue(T t) {
		//�����ǰ����Ϊ��
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
	//������
	T dequeue() {
		if (isempty()) {
			return NULL;
		}
		//��head������һ�����ɾ��
		Node* oldNode = head->next;
		T items = oldNode->item;
		head->next = oldNode->next;
		delete oldNode;
		num--;
		//���ɾ���������һ��Ԫ�أ���lastָ���ÿ�
		if (isempty()) {
			last = nullptr;
		}
		return items;
	}
};
