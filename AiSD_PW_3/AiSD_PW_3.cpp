#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <queue>
#include <vector>
#include <algorithm>
#include <sstream>
#include <stdexcept>

using namespace std;

struct BinaryTreeNode {
    int value;
    BinaryTreeNode* left;
    BinaryTreeNode* right;

    BinaryTreeNode(int val) : value(val), left(nullptr), right(nullptr) {}
};

struct AVLTreeNode {
    int value;
    AVLTreeNode* left;
    AVLTreeNode* right;
    int height;

    AVLTreeNode(int val) : value(val), left(nullptr), right(nullptr), height(1) {}
};

class BinaryTree {
private:
    BinaryTreeNode* root;

    void clear(BinaryTreeNode* node) {
        if (node) {
            clear(node->left);
            clear(node->right);
            delete node;
        }
    }

    BinaryTreeNode* parseBracketString(const string& s, size_t& pos) {
        while (pos < s.length() && s[pos] == ' ') pos++;

        if (pos >= s.length()) {
            throw invalid_argument("Unexpected end of string");
        }

        if (s[pos] == '(') {
            pos++;

            while (pos < s.length() && s[pos] == ' ') pos++;

            if (pos >= s.length()) {
                throw invalid_argument("Unexpected end of string after '('");
            }

            int value = 0;
            bool negative = false;

            if (s[pos] == '-') {
                negative = true;
                pos++;
            }

            if (!isdigit(s[pos])) {
                throw invalid_argument("Expected number");
            }

            while (pos < s.length() && isdigit(s[pos])) {
                value = value * 10 + (s[pos] - '0');
                pos++;
            }

            if (negative) {
                value = -value;
            }

            BinaryTreeNode* node = new BinaryTreeNode(value);

            while (pos < s.length() && s[pos] == ' ') pos++;

            if (pos < s.length() && s[pos] == '(') {
                node->left = parseBracketString(s, pos);
            }

            while (pos < s.length() && s[pos] == ' ') pos++;

            if (pos < s.length() && s[pos] == '(') {
                node->right = parseBracketString(s, pos);
            }

            while (pos < s.length() && s[pos] == ' ') pos++;

            if (pos >= s.length() || s[pos] != ')') {
                throw invalid_argument("Expected closing bracket");
            }

            pos++;
            return node;
        }
        else {
            throw invalid_argument("Expected opening bracket");
        }
    }

    void depthFirstTraversalRecursive(BinaryTreeNode* node, vector<int>& result) {
        if (node) {
            result.push_back(node->value);
            depthFirstTraversalRecursive(node->left, result);
            depthFirstTraversalRecursive(node->right, result);
        }
    }

public:
    BinaryTree() : root(nullptr) {}

    ~BinaryTree() {
        clear(root);
    }

    bool buildFromBracketString(const string& bracketString) {
        try {
            if (bracketString.empty()) {
                throw invalid_argument("Empty string");
            }

            for (char c : bracketString) {
                if (!isdigit(c) && c != '(' && c != ')' && c != ' ' && c != '-') {
                    throw invalid_argument("Invalid characters in string");
                }
            }

            stack<char> brackets;
            for (char c : bracketString) {
                if (c == '(') {
                    brackets.push(c);
                }
                else if (c == ')') {
                    if (brackets.empty()) {
                        throw invalid_argument("Unbalanced brackets");
                    }
                    brackets.pop();
                }
            }

            if (!brackets.empty()) {
                throw invalid_argument("Unbalanced brackets");
            }

            size_t pos = 0;
            root = parseBracketString(bracketString, pos);

            while (pos < bracketString.length() && bracketString[pos] == ' ') pos++;
            if (pos < bracketString.length()) {
                throw invalid_argument("Extra characters at end of string");
            }

            return true;

        }
        catch (const exception& e) {
            cout << "Error building tree: " << e.what() << endl;
            clear(root);
            root = nullptr;
            return false;
        }
    }

    vector<int> depthFirstTraversal() {
        vector<int> result;
        depthFirstTraversalRecursive(root, result);
        return result;
    }

    BinaryTreeNode* getRoot() const { return root; }
};

class AVLTree {
private:
    AVLTreeNode* root;

    int getHeight(AVLTreeNode* node) {
        return node ? node->height : 0;
    }

    int getBalance(AVLTreeNode* node) {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }

    void updateHeight(AVLTreeNode* node) {
        if (node) {
            node->height = 1 + max(getHeight(node->left), getHeight(node->right));
        }
    }

    AVLTreeNode* rotateRight(AVLTreeNode* y) {
        AVLTreeNode* x = y->left;
        AVLTreeNode* T2 = x->right;

        x->right = y;
        y->left = T2;

        updateHeight(y);
        updateHeight(x);

        return x;
    }

    AVLTreeNode* rotateLeft(AVLTreeNode* x) {
        AVLTreeNode* y = x->right;
        AVLTreeNode* T2 = y->left;

        y->left = x;
        x->right = T2;

        updateHeight(x);
        updateHeight(y);

        return y;
    }

    AVLTreeNode* insert(AVLTreeNode* node, int value) {
        if (!node) {
            return new AVLTreeNode(value);
        }

        if (value < node->value) {
            node->left = insert(node->left, value);
        }
        else if (value > node->value) {
            node->right = insert(node->right, value);
        }
        else {
            return node;
        }

        updateHeight(node);

        int balance = getBalance(node);

        if (balance > 1 && value < node->left->value) {
            return rotateRight(node);
        }

        if (balance < -1 && value > node->right->value) {
            return rotateLeft(node);
        }

        if (balance > 1 && value > node->left->value) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        if (balance < -1 && value < node->right->value) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    AVLTreeNode* minValueNode(AVLTreeNode* node) {
        AVLTreeNode* current = node;
        while (current && current->left) {
            current = current->left;
        }
        return current;
    }

    AVLTreeNode* deleteNode(AVLTreeNode* node, int value) {
        if (!node) {
            return node;
        }

        if (value < node->value) {
            node->left = deleteNode(node->left, value);
        }
        else if (value > node->value) {
            node->right = deleteNode(node->right, value);
        }
        else {
            if (!node->left || !node->right) {
                AVLTreeNode* temp = node->left ? node->left : node->right;

                if (!temp) {
                    temp = node;
                    node = nullptr;
                }
                else {
                    *node = *temp;
                }

                delete temp;
            }
            else {
                AVLTreeNode* temp = minValueNode(node->right);
                node->value = temp->value;
                node->right = deleteNode(node->right, temp->value);
            }
        }

        if (!node) {
            return node;
        }

        updateHeight(node);

        int balance = getBalance(node);

        if (balance > 1 && getBalance(node->left) >= 0) {
            return rotateRight(node);
        }

        if (balance > 1 && getBalance(node->left) < 0) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        if (balance < -1 && getBalance(node->right) <= 0) {
            return rotateLeft(node);
        }

        if (balance < -1 && getBalance(node->right) > 0) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    bool search(AVLTreeNode* node, int value) {
        if (!node) {
            return false;
        }

        if (value == node->value) {
            return true;
        }
        else if (value < node->value) {
            return search(node->left, value);
        }
        else {
            return search(node->right, value);
        }
    }

    void clear(AVLTreeNode* node) {
        if (node) {
            clear(node->left);
            clear(node->right);
            delete node;
        }
    }

public:
    AVLTree() : root(nullptr) {}

    ~AVLTree() {
        clear(root);
    }

    void insert(int value) {
        root = insert(root, value);
    }

    void remove(int value) {
        root = deleteNode(root, value);
    }

    bool search(int value) {
        return search(root, value);
    }

    void buildFromBinaryTree(BinaryTree& binaryTree) {
        vector<int> values = binaryTree.depthFirstTraversal();
        for (int value : values) {
            insert(value);
        }
    }

    vector<int> breadthFirstTraversal() {
        vector<int> result;
        if (!root) return result;

        queue<AVLTreeNode*> q;
        q.push(root);

        while (!q.empty()) {
            AVLTreeNode* node = q.front();
            q.pop();
            result.push_back(node->value);

            if (node->left) q.push(node->left);
            if (node->right) q.push(node->right);
        }

        return result;
    }

    vector<int> depthFirstPreorder() {
        vector<int> result;
        if (!root) return result;

        stack<AVLTreeNode*> s;
        s.push(root);

        while (!s.empty()) {
            AVLTreeNode* node = s.top();
            s.pop();
            result.push_back(node->value);

            if (node->right) s.push(node->right);
            if (node->left) s.push(node->left);
        }

        return result;
    }

    vector<int> depthFirstInorder() {
        vector<int> result;
        stack<AVLTreeNode*> s;
        AVLTreeNode* current = root;

        while (current || !s.empty()) {
            while (current) {
                s.push(current);
                current = current->left;
            }

            current = s.top();
            s.pop();
            result.push_back(current->value);

            current = current->right;
        }

        return result;
    }

    vector<int> depthFirstPostorder() {
        vector<int> result;
        if (!root) return result;

        stack<AVLTreeNode*> s1, s2;
        s1.push(root);

        while (!s1.empty()) {
            AVLTreeNode* node = s1.top();
            s1.pop();
            s2.push(node);

            if (node->left) s1.push(node->left);
            if (node->right) s1.push(node->right);
        }

        while (!s2.empty()) {
            result.push_back(s2.top()->value);
            s2.pop();
        }

        return result;
    }
};

string readFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open file " << filename << endl;
        return "";
    }

    string content;
    string line;
    while (getline(file, line)) {
        content += line;
    }

    file.close();
    return content;
}

bool validateBinaryTreeStructure(BinaryTreeNode* node) {
    if (!node) return true;
    return validateBinaryTreeStructure(node->left) &&
        validateBinaryTreeStructure(node->right);
}

void printVector(const vector<int>& vec, const string& name) {
    cout << name << ": [";
    for (size_t i = 0; i < vec.size(); ++i) {
        cout << vec[i];
        if (i < vec.size() - 1) cout << ", ";
    }
    cout << "]" << endl;
}

void processTree(const string& bracketString) {
    cout << "\nProcessing bracket string: " << bracketString << endl;

    BinaryTree binaryTree;

    if (!binaryTree.buildFromBracketString(bracketString)) {
        cout << "Failed to build binary tree. Check input correctness." << endl;
        return;
    }

    if (!validateBinaryTreeStructure(binaryTree.getRoot())) {
        cout << "Error: Tree is not binary!" << endl;
        return;
    }

    cout << "✓ Binary tree built successfully" << endl;

    vector<int> dfsResult = binaryTree.depthFirstTraversal();
    printVector(dfsResult, "Binary tree depth-first traversal");

    AVLTree avlTree;
    avlTree.buildFromBinaryTree(binaryTree);
    cout << "✓ AVL tree built successfully" << endl;

    cout << "\n--- AVL Tree Traversals ---" << endl;

    vector<int> bfsResult = avlTree.breadthFirstTraversal();
    printVector(bfsResult, "Breadth-first traversal");

    vector<int> dfsPreorder = avlTree.depthFirstPreorder();
    printVector(dfsPreorder, "Preorder traversal");

    vector<int> dfsInorder = avlTree.depthFirstInorder();
    printVector(dfsInorder, "Inorder traversal");

    vector<int> dfsPostorder = avlTree.depthFirstPostorder();
    printVector(dfsPostorder, "Postorder traversal");

    cout << "\n--- Additional AVL Tree Operations ---" << endl;

    int searchValue;
    cout << "Enter value to search: ";
    cin >> searchValue;
    bool found = avlTree.search(searchValue);
    cout << "Value " << searchValue << (found ? " found" : " not found") << " in tree" << endl;

    int insertValue;
    cout << "Enter value to insert: ";
    cin >> insertValue;
    avlTree.insert(insertValue);
    cout << "Value " << insertValue << " inserted into tree" << endl;

    cout << "\n--- Traversals after insertion ---" << endl;
    printVector(avlTree.breadthFirstTraversal(), "Breadth-first traversal");
    printVector(avlTree.depthFirstPreorder(), "Preorder traversal");

    int deleteValue;
    cout << "Enter value to delete: ";
    cin >> deleteValue;
    avlTree.remove(deleteValue);
    cout << "Value " << deleteValue << " deleted from tree" << endl;

    cout << "\n--- Traversals after deletion ---" << endl;
    printVector(avlTree.breadthFirstTraversal(), "Breadth-first traversal");
    printVector(avlTree.depthFirstPreorder(), "Preorder traversal");
}

int main() {
    cout << "=== Laboratory Work: Trees ===" << endl;

    while (true) {
        cout << "\nMenu:" << endl;
        cout << "1. Enter tree manually" << endl;
        cout << "2. Load tree from file" << endl;
        cout << "3. Exit" << endl;

        string choice;
        cout << "Choose option: ";
        getline(cin, choice);

        if (choice == "1") {
            string bracketString;
            cout << "Enter bracket tree notation (e.g., (8 (9 (5)) (1)): ";
            getline(cin, bracketString);
            processTree(bracketString);
        }
        else if (choice == "2") {
            string filename;
            cout << "Enter filename: ";
            getline(cin, filename);
            string bracketString = readFromFile(filename);
            if (!bracketString.empty()) {
                processTree(bracketString);
            }
        }
        else if (choice == "3") {
            cout << "Exiting program." << endl;
            break;
        }
        else {
            cout << "Invalid choice. Please try again." << endl;
        }

        cin.clear();
    }

    return 0;
}
