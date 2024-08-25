#pragma once

// Node structure for the Huffman Tree
struct Node {
    int weight;
    int index;
    char character;
    Node* left;
    Node* right;
    Node* parent;

    Node(int val, int idx, char ch = '\0') : weight(val), index(idx), character(ch), left(nullptr), right(nullptr), parent(nullptr) {}
};
