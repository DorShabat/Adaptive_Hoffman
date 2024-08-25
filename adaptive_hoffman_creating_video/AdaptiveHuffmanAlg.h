#pragma once
#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <queue>
#include <iomanip>
#include <bitset>
#include <vector>
#include <string>
#include <fstream>
#include <queue>
#include <string>
#include <cstdlib> 
#include "Node.h"

class AdaptiveHuffmanAlg {
private:
    Node* root;
    std::unordered_map<int, Node*> indexNodeDict;
    std::unordered_map<char, Node*> charNodeDict;
    int nytIndex;
    int maxIndexNumber;
    int currentIndex;
    std::unordered_map<char, std::string> charCodeDict;
    int INERTION;
    int INERTIONCounter;
    int step = 0;
    save_state save_it;
    int final_step = -1;
    int index_img = 0;

public:

    AdaptiveHuffmanAlg(int rootValue, int rootIndex, int insertionInput, save_state saving)
        : root(new Node(rootValue, rootIndex)), nytIndex(rootIndex), maxIndexNumber(rootIndex), currentIndex(rootIndex), INERTION(insertionInput), INERTIONCounter(1), save_it(saving) {
        indexNodeDict[rootIndex] = root;
    }
    int get_max_index_img() {
        return index_img;
    }
    Node* findNodeByChar(char c);
    Node* findMaxNodeInBlock(Node* node);
    void updateValueOfNode(Node* node);
    void charCodeDictCreate();
    Node* findNode(int index);
    Node* findNytNode();
    int allocateNewIndex();
    void swapNodes(Node* node1, Node* node2);
    void updateTreeValues(Node* node);
    std::string getCodeForNode(Node* node);
    std::string encodeSymbol(char letter);
    std::string encodeText(const std::string& text);
    void printTreeHelper(Node* node, int is_left, int depth);
    void printTree();
    std::vector<Node*> getLeafNodes();
    void divideLeafValuesAndUpdateTree();
    int updateInternalNodeValues(Node* node);
    void exportTreeToDot(const std::string& filename, int highlightIndex1, int highlightIndex2);
};
