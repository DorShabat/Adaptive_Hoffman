#include "Node.h"
#include "AdaptiveHuffmanAlg.h"

Node* AdaptiveHuffmanAlg::findNodeByChar(char c) {
    return charNodeDict.count(c) ? charNodeDict[c] : nullptr;
}

Node* AdaptiveHuffmanAlg::findMaxNodeInBlock(Node* node) {
    Node* maxNode = node; // Start with the current node
    int maxIndex = node->index; // Start with the current node's index
    // Iterate from the highest index down to the current node's index
    for (int i = maxIndexNumber; i >= node->index; --i) {
        if (indexNodeDict.count(i)) {
            // Check if the node has the same value and a higher index
            if (indexNodeDict[i]->weight == node->weight && indexNodeDict[i]->index > maxIndex) {
                maxNode = indexNodeDict[i];
                maxIndex = indexNodeDict[i]->index;
                break;
            }
        }
    }
    return maxNode;
}

void AdaptiveHuffmanAlg::updateValueOfNode(Node* node) {
    node->weight += 1;
    updateTreeValues(node);
}

void AdaptiveHuffmanAlg::charCodeDictCreate() {
    int m = 26;
    int e = std::floor(std::log2(m));
    int r = m - std::pow(2, e);

    for (int k = 1; k <= m; ++k) {
        if (k >= 1 && k <= 2 * r) {
            charCodeDict['a' + k - 1] = std::bitset<5>(k - 1).to_string().substr(5 - e - 1);
        }
        else {
            charCodeDict['a' + k - 1] = std::bitset<5>(k - r - 1).to_string().substr(5 - e);
        }
    }
}

Node* AdaptiveHuffmanAlg::findNode(int index) {
    return indexNodeDict.count(index) ? indexNodeDict[index] : nullptr;
}

Node* AdaptiveHuffmanAlg::findNytNode() {
    return findNode(nytIndex);
}

int AdaptiveHuffmanAlg::allocateNewIndex() {
    return --currentIndex;
}

void AdaptiveHuffmanAlg::swapNodes(Node* node1, Node* node2) {
    Node* parent1 = node1->parent;
    Node* parent2 = node2->parent;
    //same father
    if (parent1 == parent2) {
        if (parent1->left == node1) {
            parent1->left = node2;
            parent1->right = node1;
        }
        else {
            parent1->left = node1;
            parent1->right = node2;
        }
    }
    //different father
    else {
        if (parent1) {
            if (parent1->left == node1) parent1->left = node2;
            else parent1->right = node2;
        }
        if (parent2) {
            if (parent2->left == node2) parent2->left = node1;
            else parent2->right = node1;
        }
    }

    node1->parent = parent2;
    node2->parent = parent1;

    std::swap(node1->index, node2->index);

    indexNodeDict[node1->index] = node1;
    indexNodeDict[node2->index] = node2;
}

int getDepthOfDeepestDescendant(Node* node) {
    if (node == nullptr) {
        return 0;
    }

    int leftDepth = getDepthOfDeepestDescendant(node->left);
    int rightDepth = getDepthOfDeepestDescendant(node->right);

    return 1 + std::max(leftDepth, rightDepth);
}

void AdaptiveHuffmanAlg::updateTreeValues(Node* node) {
    while (node) {
        if (node->left == NULL && node->right == NULL) { node = node->parent; continue; }

        Node* maxNode = findMaxNodeInBlock(node);
        if (maxNode != node && maxNode->parent != node && node->parent != maxNode) {
            if (save_it == 0 && getDepthOfDeepestDescendant(maxNode) != getDepthOfDeepestDescendant(node)) {
                exportTreeToDot("results/tree" + std::to_string(index_img) + ".dot", maxNode->index, node->index);
                step++;
            }
            swapNodes(node, maxNode);
        }
        int oldValue = node->weight;
        node->weight = (node->left ? node->left->weight : 0) + (node->right ? node->right->weight : 0);

        node = node->parent;
    }
}

std::string AdaptiveHuffmanAlg::getCodeForNode(Node* node) {
    std::string code;
    while (node->parent) {
        code = (node->parent->left == node ? "0" : "1") + code;
        node = node->parent;
    }
    return code;
}

std::string AdaptiveHuffmanAlg::encodeSymbol(char letter) {
    //letter in tree
    if (charNodeDict.count(letter)) {
        Node* node = charNodeDict[letter];
        std::string code = getCodeForNode(node);
        updateValueOfNode(node);
        return code;
    }
    //create node for letter
    else {
        std::string nytCode = getCodeForNode(findNytNode());
        std::string charCode = charCodeDict[letter];
        std::string code = nytCode + charCode;

        Node* nytNode = findNytNode();
        Node* newNode = new Node(0, allocateNewIndex(), letter);
        Node* newNytNode = new Node(0, allocateNewIndex());

        nytNode->right = newNode;
        newNode->parent = nytNode;
        nytNode->left = newNytNode;
        newNytNode->parent = nytNode;

        nytIndex = newNytNode->index;
        indexNodeDict[newNode->index] = newNode;
        indexNodeDict[newNytNode->index] = newNytNode;
        charNodeDict[letter] = newNode;

        updateValueOfNode(newNode);
        if (save_it == 0) {
            exportTreeToDot("results/tree" + std::to_string(index_img) + ".dot", -1, -1);
            step++;
        }
        return code;
    }
}

std::string AdaptiveHuffmanAlg::encodeText(const std::string& text) {
    charCodeDictCreate();
    std::ostringstream encodedText;
    for (char letter : text) {
        encodedText << encodeSymbol(letter);
        if (INERTION == INERTIONCounter) {
            divideLeafValuesAndUpdateTree(); 
            INERTIONCounter = 0;
        }
        INERTIONCounter++;

    }
    if (save_it != 2) {
        exportTreeToDot("results/tree" + std::to_string(index_img) + ".dot", -1, -1);
        step++;
        final_step = step;
    }
    return encodedText.str();
}

void AdaptiveHuffmanAlg::printTreeHelper(Node* node, int is_left, int depth) {
    if (node == nullptr) return;
    std::string direction = "R";
    if (is_left) { direction = "L"; }
    if (is_left == 2) { direction = "Root"; }
    // Print the current node with its depth indicated by leading spaces
    for (int i = 0; i < depth; ++i) {
        std::cout << "|   "; // Use '|   ' to represent the depth level
    }
    std::cout << "|-- "; // Use '|-- ' before the node value
    std::cout << "[" << direction << "__" << node->weight << "] (" << node->index;
    if (node->character != '\0') {
        std::cout << ", '" << node->character << "'";
    }
    std::cout << ")";
    if (node->parent != nullptr) {
        std::cout << "parent index: " << node->parent->index << "\n";
    }
    else {
        std::cout << "\n";
    }

    // Recursively print the left and right children with increased depth
    if (node->left || node->right) {
        printTreeHelper(node->left, 1, depth + 1);
        printTreeHelper(node->right, 0, depth + 1);
    }
}
void AdaptiveHuffmanAlg::printTree() {
    printTreeHelper(root, 2, 0);
}

std::vector<Node*> AdaptiveHuffmanAlg::getLeafNodes() {
    std::vector<Node*> leaf_nodes;
    std::vector<Node*> nodes_to_check = { this->root };
    while (!nodes_to_check.empty()) {
        Node* node = nodes_to_check.back();
        nodes_to_check.pop_back();
        if (node->left == nullptr && node->right == nullptr) {
            leaf_nodes.push_back(node);
        }
        else {
            if (node->left != nullptr) nodes_to_check.push_back(node->left);
            if (node->right != nullptr) nodes_to_check.push_back(node->right);
        }
    }
    return leaf_nodes;
}

void AdaptiveHuffmanAlg::divideLeafValuesAndUpdateTree() {
    std::vector<Node*> leaf_nodes = getLeafNodes();
    for (Node* leaf : leaf_nodes) {
        leaf->weight = (leaf->weight + 1) / 2; // Divide by 2 and round up
    }
    updateInternalNodeValues(this->root);
}

int AdaptiveHuffmanAlg::updateInternalNodeValues(Node* node) {
    if (node->left == nullptr && node->right == nullptr) {
        return node->weight;
    }

    int left_value = node->left ? updateInternalNodeValues(node->left) : 0;
    int right_value = node->right ? updateInternalNodeValues(node->right) : 0;

    node->weight = left_value + right_value;
    return node->weight;
}
void AdaptiveHuffmanAlg::exportTreeToDot(const std::string& filename, int highlightIndex1, int highlightIndex2) {
    if (step != 0 && step != 1 && step != 2 && step != 3 && step != 4 && step != 5 && step != 6 && step % 21 != 0 && step != final_step) return; // Save only every 21th frame and the first 7 frames and the final frame
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Unable to open file " << filename << std::endl;
        return;
    }

    file << "digraph G {\n";
    std::queue<Node*> q;
    q.push(root);
    while (!q.empty()) {
        Node* node = q.front();
        q.pop();

        if (node->left) {
            file << "    \"" << node->index << "\" -> \"" << node->left->index << "\" [label=\"0\"];\n";
            q.push(node->left);
        }
        if (node->right) {
            file << "    \"" << node->index << "\" -> \"" << node->right->index << "\" [label=\"1\"];\n";
            q.push(node->right);
        }

        std::string shape = "ellipse";
        std::string style = "";
        std::string fillcolor = "";

        if (node == root) {
            shape = "doublecircle";
            style = "filled";
            fillcolor = "lightblue";
        }
        else if (!node->left && !node->right) {
            shape = "box";
        }

        if (node->index == highlightIndex1 || node->index == highlightIndex2) {
            style = "filled";
            fillcolor = "yellow";
        }
        else if (node->weight == 0) {
            style = "filled";
            fillcolor = "purple";
        }

        file << "    \"" << node->index << "\" [label=\"";
        if (node->weight == 0) {
            file << "(NYT)";
        }
        else {
            file << "(" << node->index << ")\\n" << node->weight;
            if (node->character != '\0') {
                file << "\\n" << node->character;
            }
        }
        file << "\", shape=\"" << shape << "\", style=\"" << style << "\", fillcolor=\"" << fillcolor << "\"];\n";
    }
    file << "}\n";
    file.close();
    index_img++;
}
