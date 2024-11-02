// This is the CPP file you will edit and turn in.
// Also remove these comments here and add your own, along with
// comments on every function and on complex code sections.
// TODO: remove this comment header

#include "encoding.h"
using namespace std;
// TODO: include any other headers you need

Map<int, int> buildFrequencyTable(istream& input) {
    // TODO: implement this function
    Map<int, int> freqTable;   // this is just a placeholder so it will compile
    int x = 0;
    while ((x = input.get()) != -1) {
        freqTable[x] += 1;
    }
    freqTable[PSEUDO_EOF] = 1;
    return freqTable;          // this is just a placeholder so it will compile
}

HuffmanNode* buildEncodingTree(const Map<int, int>& freqTable) {
    // TODO: implement this function
    PriorityQueue<HuffmanNode*> pq;
    for (const int &key : freqTable) {
        // it can't use char c = key in here.
        int value = freqTable[key];
        HuffmanNode* node = new HuffmanNode(key, value);
        pq.enqueue(node, value);
    }
    HuffmanNode* root = nullptr;
    while (true) {
        HuffmanNode* node1 = nullptr, *node2 = nullptr;
        if (!pq.isEmpty()) node1 = pq.dequeue();
        if (!pq.isEmpty()) node2 = pq.dequeue();
        if (node1 == nullptr || node2 == nullptr) break;

        HuffmanNode* newNode = new HuffmanNode(NOT_A_CHAR, node1->count + node2->count, node1, node2);
        root = newNode;
        pq.enqueue(newNode, newNode->count);
    }
    return root;   // this is just a placeholder so it will compile
}

Map<int, string> buildEncodingMap(HuffmanNode* encodingTree) {
    // TODO: implement this function
    Map<int, string> encodingMap;   // this is just a placeholder so it will compile
    using PHS = pair<HuffmanNode*, string>;
    Queue<PHS> q;
    if (encodingTree != nullptr) {
        q.enqueue({encodingTree, ""});
        while (!q.isEmpty()) {
            PHS front = q.dequeue();
            HuffmanNode* node = front.first;
            string code = front.second;
            if (node->isLeaf()) {
                encodingMap[node->character] = code;
            } else {
                q.enqueue({node->zero, code + "0"});
                q.enqueue({node->one, code + "1"});
            }
        }
    }
    return encodingMap;             // this is just a placeholder so it will compile
}

void encodeData(istream& input, const Map<int, string>& encodingMap, obitstream& output) {
    // TODO: implement this function
    int x = 0;
    while ((x = input.get()) != -1) {
        string code = encodingMap[x];
        for (const char &c : code) {
            if (c == '0') output.writeBit(0);
            else output.writeBit(1);
        }
    }
    string code = encodingMap[PSEUDO_EOF];
    for (const char &c : code) {
        if (c == '0') output.writeBit(0);
        else output.writeBit(1);
    }
}

void decodeData(ibitstream& input, HuffmanNode* encodingTree, ostream& output) {
    // TODO: implement this function
    int bit = 0;
    HuffmanNode* t = encodingTree;
    while((bit = input.readBit()) != EOF) {
        if (bit == 0) t = t->zero;
        else t = t->one;
        if (t->isLeaf()) {
            output.put(t->character);
            t = encodingTree;
        }
    }
}

void compress(istream& input, obitstream& output) {
    // TODO: implement this function
    Map<int, int> freqTable = buildFrequencyTable(input);
    HuffmanNode* root = buildEncodingTree(freqTable);
    Map<int, string> encodingMap = buildEncodingMap(root);
    output << freqTable;
    rewindStream(input);
    encodeData(input, encodingMap, output);
    freeTree(root);
}

void decompress(ibitstream& input, ostream& output) {
    // TODO: implement this function
    Map<int, int> freqTable;
    input >> freqTable;
    HuffmanNode* root = buildEncodingTree(freqTable);
    decodeData(input, root, output);
    freeTree(root);
}

void freeTree(HuffmanNode* node) {
    // TODO: implement this function
    if (node == nullptr) return;
    freeTree(node->zero);
    freeTree(node->one);
    delete node;
}
