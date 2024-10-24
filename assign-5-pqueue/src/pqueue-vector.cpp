#include "pqueue-vector.h"
using namespace std;

VectorPQueue::VectorPQueue() {}
VectorPQueue::~VectorPQueue() {}

const string& VectorPQueue::peek() const {
    // placeholder so method compiles..
	// replace with your own implementation
    if (isEmpty()) {
        return kEmptyString;
    }
    int index = 0;
    for (int i = 1; i < pq.size(); i++) {
        if (pq[i].compare(pq[index]) < 0) {
            index = i;
        }
    }
    const string &ans = pq[index];
    return ans;
}

string VectorPQueue::extractMin() {
	// placeholder so method compiles..
	// replace with your own implementation
    const string &tmp = peek();
    string front = tmp;
    for (int i = 0; i < pq.size(); i++) {
        if (pq[i].compare(front) == 0) {
            pq.remove(i);
            logSize -= 1;
            break;
        }
    }
    return front;
}

void VectorPQueue::enqueue(const string& elem) {
	// placeholder so method compiles..
	// replace with your own implementation
    pq.add(elem);
    logSize += 1;
}

VectorPQueue *VectorPQueue::merge(VectorPQueue * one, VectorPQueue * two) {
	// placeholder so method compiles..
	// replace with your own implementation
    VectorPQueue *mergePqueue = new VectorPQueue();
    while (!one->isEmpty()) {
        mergePqueue->enqueue(one->extractMin());
    }
    while (!two->isEmpty()) {
        mergePqueue->enqueue(two->extractMin());
    }
    return mergePqueue;
}
