#include "pqueue-binomial-heap.h"
using namespace std;

BinomialHeapPQueue::BinomialHeapPQueue() {
    this->nodes = {nullptr};
}
void BinomialHeapPQueue::deleteNodes(node* &t) {
    if (!t->children.isEmpty()) {
        for (int i = 0; i < t->children.size(); i++) {
            deleteNodes(t->children[i]);
        }
    }
    delete t;
    return;
}
BinomialHeapPQueue::~BinomialHeapPQueue() {
    for (int i = 0; i < this->nodes.size(); i++) {
        if (this->nodes[i] != nullptr) {
            this->deleteNodes(this->nodes[i]);
            this->nodes[i] = nullptr;
        }
    }
}

const string& BinomialHeapPQueue::peek() const {
    // placeholder so method compiles..
	// replace with your own implementation
    if (this->isEmpty()) {
        return kEmptyString;
    }
    int index = -1;
    for (int i = 0; i < this->nodes.size(); i++) {
        if (this->nodes[i] != nullptr) {
            if (index == -1 || this->nodes[index]->elem.compare(this->nodes[i]->elem) > 0) {
                index = i;
            }
        }
    }
    return this->nodes[index]->elem;
}

string BinomialHeapPQueue::extractMin() {
	// placeholder so method compiles..
	// replace with your own implementation
    string front = peek();
    if (!this->isEmpty()) {
        int index = -1;
        for (int i = 0; i < this->nodes.size(); i++) {
            if (this->nodes[i] != nullptr && this->nodes[i]->elem.compare(front) == 0) {
                index = i;
                break;
            }
        }
        Vector<node*> toAdd = this->nodes[index]->children;
        delete this->nodes[index];
        this->nodes[index] = nullptr;
        node *t = nullptr;
        for (int i = 0; i < toAdd.size() && i < this->nodes.size(); i++) {
            index = i;
            if (nodes[i] == nullptr && t == nullptr) {
                nodes[i] = toAdd[i];
            } else if (nodes[i] == nullptr && t != nullptr) {
                if (t->elem.compare(toAdd[i]->elem) > 0) {
                    toAdd[i]->children.add(t);
                    t = toAdd[i];
                } else {
                    t->children.add(toAdd[i]);
                }
            } else if (nodes[i] != nullptr && t == nullptr) {
                if (nodes[i]->elem.compare(toAdd[i]->elem) > 0) {
                    toAdd[i]->children.add(nodes[i]);
                    t = toAdd[i];
                } else {
                    nodes[i]->children.add(toAdd[i]);
                    t = nodes[i];
                }
                nodes[i] = nullptr;
            } else {
                node *temp = nullptr;
                if (nodes[i]->elem.compare(toAdd[i]->elem) > 0) {
                    toAdd[i]->children.add(nodes[i]);
                    temp = toAdd[i];
                } else {
                    nodes[i]->children.add(toAdd[i]);
                    temp = nodes[i];
                }
                nodes[i] = t;
                t = temp;
            }
        }
        ++index;
        while (index < nodes.size() && t != nullptr) {
            if (nodes[index] == nullptr) {
                nodes[index] = t;
                t = nullptr;
            } else {
                if (nodes[index]->elem.compare(t->elem) > 0) {
                    t->children.add(nodes[index]);
                } else {
                    nodes[index]->children.add(t);
                    t = nodes[index];
                }
                nodes[index] = nullptr;
            }
            ++index;
        }
    }
    this->logSize -= 1;
    return front;
}

void BinomialHeapPQueue::enqueue(const string& elem) {
    // placeholder so method compiles.
	// replace with your own implementation
    bool stop = false;
    node* t = new node(elem);
    for (int i = 0; i < this->nodes.size(); i++) {
        if (nodes[i] == nullptr) {
            nodes[i] = t;
            stop = true;
            break;
        } else {
            if (t->elem.compare(nodes[i]->elem) > 0) {
                nodes[i]->children.add(t);
                t = nodes[i];
            } else {
                t->children.add(nodes[i]);
            }
            nodes[i] = nullptr;
        }
    }
    if (!stop) {
        this->nodes.add(t);
    }
    this->logSize += 1;
}
void BinomialHeapPQueue::copyNodes(node *&dst, const node *src) {
    if (src != nullptr) {
        dst->elem = src->elem;
        for (int i = 0; i < src->children.size(); i++) {
            node* temp = new node();
            copyNodes(temp, src->children[i]);
            dst->children.add(temp);
        }
    }
}
BinomialHeapPQueue *BinomialHeapPQueue::merge(BinomialHeapPQueue* one, BinomialHeapPQueue* two) {
	// placeholder so method compiles..
	// replace with your own implementation
    // There is a problem, I have to create new nodes for pq, not use node* from one and two,
    // so at least, I have to copy the elements from one and two, which will at least cost O(n + m),
    // because I assume there are n + m elements in both of them. And then I can merge them in O(logn)
    // and there will have 8 situations,
    // we consider node1, node2, t:
    /*                 0      0  0
     *                 0      0  1
     *                 0      1  0
     *                 0      1  1
     *                 1      0  0
     *                 ...
     * But we can add node1 and node2 first, and if node1 + node2 > 1, then we set new node = t
     * and let t = node1 + node2, else node1 + node2 < 1, so we add t now, after adding t,if the
     * reuslt > 1, let new node = nullptr, t = node1 + node2 + t, else set new node = t and
     * t = nullptr. So we could add next and next.
     * The implementation could be noising, so I choose to implement it later if I have time.
     * No matter how we simplify the add process, We still need to creat the copy of one and two,
     * which is definitely not be fun(I really don't want to copy a treeeeeeeeee,,,).
    */
    BinomialHeapPQueue *pq = new BinomialHeapPQueue();
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
