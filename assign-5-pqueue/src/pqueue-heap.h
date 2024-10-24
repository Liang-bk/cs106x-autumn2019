#pragma once
#include "pqueue.h"
#include <string>
#include "vector.h"
class HeapPQueue : public PQueue {
public:
	HeapPQueue();
	~HeapPQueue();
	
	static HeapPQueue *merge(HeapPQueue *one, HeapPQueue *two);
	
	void enqueue(const std::string& elem);
    std::string extractMin();
    const std::string& peek() const;
    
private:
    // provide data methods and helper methods to
    // help realize the binary heap-backed PQueue
    void up(int i);
    void down(int i);
    void escalateSize();
    std::string *arr;
    int size;
};
