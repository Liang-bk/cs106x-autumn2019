#include "pqueue-linked-list.h"
using namespace std;

LinkedListPQueue::LinkedListPQueue() {
    this->head = new node();
    this->rear = new node();
    this->head->next = this->rear;
    this->rear->prev = this->head;
}
LinkedListPQueue::~LinkedListPQueue() {
    node *p = this->head->next;
    while (p != this->rear) {
        node *temp = p;
        p = p->next;
        delete temp;
    }
    delete this->head;
    delete this->rear;
    this->logSize = 0;
}

const string& LinkedListPQueue::peek() const {
    // placeholder so method compiles..
	// replace with your own implementation
    if (this->isEmpty()) {
        return kEmptyString;
    }
    node *front = this->head->next;
    return front->val;
}

string LinkedListPQueue::extractMin() {
	// placeholder so method compiles..
	// replace with your own implementation
    string frontVal = this->peek();
    if (!this->isEmpty()) {
        node* front = this->head->next;
        this->head->next = front->next;
        front->next->prev = this->head;
        delete front;
        this->logSize -= 1;
    }
    return frontVal;
}

void LinkedListPQueue::enqueue(const string& elem) {
	// placeholder so method compiles..
	// replace with your own implementation
    node* temp = new node();
    temp->val = elem;
    node* it = this->head->next;
    while (it != this->rear) {
        if (it->val.compare(elem) > 0){
            break;
        }
        it = it->next;
    }
    temp->prev = it->prev;
    it->prev->next = temp;
    temp->next = it;
    it->prev = temp;
    this->logSize += 1;
}

LinkedListPQueue *LinkedListPQueue::merge(LinkedListPQueue* one, LinkedListPQueue* two) {
	// placeholder so method compiles..
	// replace with your own implementation
    LinkedListPQueue *pq = new LinkedListPQueue();
    while (!one->isEmpty() && !two->isEmpty()) {
        string oneS = one->peek(), twoS = two->peek();
        if (oneS.compare(twoS) <= 0) {
            pq->enqueue(oneS);
            one->extractMin();
        } else {
            pq->enqueue(twoS);
            two->extractMin();
        }
    }
    while (!one->isEmpty()) pq->enqueue(one->extractMin());
    while (!two->isEmpty()) pq->enqueue(two->extractMin());
    return pq;
}

