#pragma once
#include "pqueue.h"
#include <string>
#include "vector.h"
class BinomialHeapPQueue : public PQueue {
public:
	BinomialHeapPQueue();
	~BinomialHeapPQueue();
	
	static BinomialHeapPQueue *merge(BinomialHeapPQueue *one, BinomialHeapPQueue *two);
	
	void enqueue(const std::string& elem);
    std::string extractMin();
    const std::string& peek() const;
private:
    // provide data methods and helper methods to
    // help realize the binomial heap-backed PQueue
    struct node
    {
        std::string elem;
        Vector<node*> children;
        node() {}
        node(const std::string &elem): elem(elem) {
            children = {};
        }
        ~node() {}
    };
    Vector<node*> nodes;
    void deleteNodes(node* &t);
    void copyNodes(node* &dst, const node* src);
};

