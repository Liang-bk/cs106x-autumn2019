#include "pqueue-heap.h"
using namespace std;

HeapPQueue::HeapPQueue() {
    this->size = 10;
    this->arr = new string[this->size + 1];
}
HeapPQueue::~HeapPQueue() {
    this->logSize = 0;
    this->size = 0;
    delete[] this->arr;
}

void HeapPQueue::down(int i) {
    int l = 2 * i, r = 2 * i + 1;
    int nextI = i;
    if (l <= this->logSize) {
        if (this->arr[nextI].compare(this->arr[l]) > 0){
            nextI = l;
        }
    }
    if (r <= this->logSize) {
        if (this->arr[nextI].compare(this->arr[r]) > 0){
            nextI = r;
        }
    }
    if (nextI != i) {
        swap(this->arr[i], this->arr[nextI]);
        down(nextI);
    }
}
void HeapPQueue::up(int i) {
    while (i > 1) {
        int fa = i / 2;
        if (this->arr[fa].compare(this->arr[i]) > 0) {
            string temp = this->arr[fa];
            this->arr[fa] = this->arr[i];
            this->arr[i] = temp;
            i = fa;
        } else {
            break;
        }
    }
}
void HeapPQueue::escalateSize() {
    this->size = this->size * 2;
    string *temp = new string[this->size];
    for (int i = 0; i <= this->logSize; i++) {
        temp[i] = this->arr[i];
    }
    delete[] this->arr;
    this->arr = temp;
}
const string& HeapPQueue::peek() const {
    // placeholder so method compiles..
	// replace with your own implementation
    if (isEmpty()) {
        return kEmptyString;
    }
    return this->arr[1];
}

string HeapPQueue::extractMin() {
	// placeholder so method compiles..
	// replace with your own implementation
    string front = peek();
    if (this->logSize > 1) {
        string temp = this->arr[this->logSize];
        this->arr[this->logSize] = this->arr[1];
        this->arr[1] = temp;
        this->logSize -= 1;
        this->down(1);
    } else {
        this->logSize -= 1;
    }
    return front;
}

void HeapPQueue::enqueue(const string& elem) {
    // placeholder so method compiles..
	// replace with your own implementation
    if (this->logSize + 1 >= this->size){
        this->escalateSize();
    }
    this->arr[this->logSize + 1] = elem;
    this->logSize += 1;
    up(this->logSize);
}

HeapPQueue *HeapPQueue::merge(HeapPQueue * one, HeapPQueue * two) {
	// placeholder so method compiles..
	// replace with your own implementation
    HeapPQueue *heapP = new HeapPQueue();
    while (!one->isEmpty() && !two->isEmpty()) {
        string oneS = one->peek(), twoS = two->peek();
        if (oneS.compare(twoS) <= 0) {
            heapP->enqueue(oneS);
            one->extractMin();
        } else {
            heapP->enqueue(twoS);
            two->extractMin();
        }
    }
    while (!one->isEmpty()) {
        string oneS = one->peek();
        heapP->enqueue(oneS);
        one->extractMin();
    }
    while (!two->isEmpty()) {
        string twoS = two->peek();
        heapP->enqueue(twoS);
        two->extractMin();
    }
    return heapP;
}

