#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <stack>
#include <vector>
#include <stdexcept>

using namespace std;

struct TreeNode {
    int value;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int val) : value(val), left(nullptr), right(nullptr) {}
};

class BinaryTree {
    TreeNode* root;

    void clear(TreeNode* node) {
        if (node) {
            clear(node->left);
            clear(node->right);
            delete node;
        }
    }

    void dfsRecursive(TreeNode* node, vector<int>& result) {
        if (!node) return;
        result.push_back(node->value);
        dfsRecursive(node->left, result);
        dfsRecursive(node->right, result);
    }

public:
    BinaryTree() : root(nullptr) {}
    ~BinaryTree() { clear(root); }

    void setRoot(TreeNode* newRoot) { root = newRoot; }
    TreeNode* getRoot() const { return root; }

    vector<int> dfs() {
        vector<int> result;
        dfsRecursive(root, result);
        return result;
    }
};

class AVLTree {
    struct AVLNode {
        int value;
        AVLNode* left;
        AVLNode* right;
        int height;
        AVLNode(int val) : value(val), left(nullptr), right(nullptr), height(1) {}
    };

    AVLNode* root;

    int height(AVLNode* node) { return node ? node->height : 0; }

    int balanceFactor(AVLNode* node) {
        return node ? height(node->left) - height(node->right) : 0;
    }

    void updateHeight(AVLNode* node) {
        if (node) node->height = 1 + max(height(node->left), height(node->right));
    }

    AVLNode* rotateRight(AVLNode* y) {
        AVLNode* x = y->left;
        y->left = x->right;
        x->right = y;
        updateHeight(y);
        updateHeight(x);
        return x;
    }

    AVLNode* rotateLeft(AVLNode* x) {
        AVLNode* y = x->right;
        x->right = y->left;
        y->left = x;
        updateHeight(x);
        updateHeight(y);
        return y;
    }

    AVLNode* balance(AVLNode* node) {
        updateHeight(node);
        int bf = balanceFactor(node);

        if (bf > 1) {
            if (balanceFactor(node->left) < 0) node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        if (bf < -1) {
            if (balanceFactor(node->right) > 0) node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
        return node;
    }

    AVLNode* insertRecursive(AVLNode* node, int value) {
        if (!node) return new AVLNode(value);

        if (value < node->value) node->left = insertRecursive(node->left, value);
        else if (value > node->value) node->right = insertRecursive(node->right, value);
        else return node;

        return balance(node);
    }

    AVLNode* findMin(AVLNode* node) {
        while (node && node->left) node = node->left;
        return node;
    }

    AVLNode* deleteRecursive(AVLNode* node, int value) {
        if (!node) return nullptr;

        if (value < node->value) {
            node->left = deleteRecursive(node->left, value);
        }
        else if (value > node->value) {
            node->right = deleteRecursive(node->right, value);
        }
        else {
            if (!node->left || !node->right) {
                AVLNode* temp = node->left ? node->left : node->right;
                if (!temp) {
                    delete node;
                    return nullptr;
                }
                *node = *temp;
                delete temp;
            }
            else {
                AVLNode* temp = findMin(node->right);
                node->value = temp->value;
                node->right = deleteRecursive(node->right, temp->value);
            }
        }
        return balance(node);
    }

    bool searchRecursive(AVLNode* node, int value) {
        if (!node) return false;
        if (node->value == value) return true;
        return value < node->value ? searchRecursive(node->left, value) : searchRecursive(node->right, value);
    }

    void clearRecursive(AVLNode* node) {
        if (node) {
            clearRecursive(node->left);
            clearRecursive(node->right);
            delete node;
        }
    }

public:
    AVLTree() : root(nullptr) {}
    ~AVLTree() { clearRecursive(root); }

    void insert(int value) { root = insertRecursive(root, value); }
    void remove(int value) { root = deleteRecursive(root, value); }
    bool search(int value) { return searchRecursive(root, value); }

    vector<int> bfs() {
        vector<int> result;
        if (!root) return result;

        queue<AVLNode*> q;
        q.push(root);

        while (!q.empty()) {
            AVLNode* current = q.front();
            q.pop();
            result.push_back(current->value);
            if (current->left) q.push(current->left);
            if (current->right) q.push(current->right);
        }
        return result;
    }

    vector<int> preorder() {
        vector<int> result;
        if (!root) return result;

        stack<AVLNode*> st;
        st.push(root);

        while (!st.empty()) {
            AVLNode* current = st.top();
            st.pop();
            result.push_back(current->value);
            if (current->right) st.push(current->right);
            if (current->left) st.push(current->left);
        }
        return result;
    }

    vector<int> inorder() {
        vector<int> result;
        stack<AVLNode*> st;
        AVLNode* current = root;

        while (current || !st.empty()) {
            while (current) {
                st.push(current);
                current = current->left;
            }
            current = st.top();
            st.pop();
            result.push_back(current->value);
            current = current->right;
        }
        return result;
    }

    vector<int> postorder() {
        vector<int> result;
        if (!root) return result;

        stack<AVLNode*> st1, st2;
        st1.push(root);

        while (!st1.empty()) {
            AVLNode* current = st1.top();
            st1.pop();
            st2.push(current);
            if (current->left) st1.push(current->left);
            if (current->right) st1.push(current->right);
        }

        while (!st2.empty()) {
            result.push_back(st2.top()->value);
            st2.pop();
        }
        return result;
    }
};

class TreeParser {
    string input;
    int pos;

    void skipSpaces() {
        while (pos < input.size() && isspace(input[pos])) pos++;
    }

    int readNumber() {
        skipSpaces();
        if (pos >= input.size() || !isdigit(input[pos])) {
            throw runtime_error("Ожидалось число");
        }

        int number = 0;
        while (pos < input.size() && isdigit(input[pos])) {
            number = number * 10 + (input[pos] - '0');
            pos++;
        }
        return number;
    }

    TreeNode* parseSubtree() {
        skipSpaces();
        if (pos >= input.size() || input[pos] != '(') throw runtime_error("Ожидалась '('");
        pos++;

        skipSpaces();
        if (pos < input.size() && input[pos] == ')') {
            pos++;
            return nullptr;
        }

        int value = readNumber();
        TreeNode* node = new TreeNode(value);

        skipSpaces();
        int childCount = 0;

        while (pos < input.size() && input[pos] != ')') {
            if (childCount >= 2) {
                delete node;
                throw runtime_error("У узла больше 2 потомков - не бинарное дерево");
            }

            TreeNode* child = parseSubtree();
            if (childCount == 0) node->left = child;
            else node->right = child;
            childCount++;
            skipSpaces();
        }

        if (pos >= input.size() || input[pos] != ')') {
            delete node;
            throw runtime_error("Ожидалась ')'");
        }
        pos++;

        return node;
    }

public:
    BinaryTree* parse(const string& str) {
        input = str;
        pos = 0;

        skipSpaces();
        if (input.empty()) throw runtime_error("Пустая строка");

        BinaryTree* tree = new BinaryTree();
        TreeNode* root = parseSubtree();

        skipSpaces();
        if (pos < input.size()) {
            delete root;
            delete tree;
            throw runtime_error("Лишние символы в конце");
        }

        tree->setRoot(root);
        return tree;
    }
};

bool validateInput(const string& str) {
    int balance = 0;
    bool expectNumber = true;

    for (size_t i = 0; i < str.size(); i++) {
        char c = str[i];
        if (isspace(c)) continue;

        if (c == '(') {
            balance++;
            expectNumber = true;
        }
        else if (c == ')') {
            balance--;
            if (balance < 0) return false;
            expectNumber = false;
        }
        else if (isdigit(c)) {
            if (!expectNumber) return false;
            while (i < str.size() && isdigit(str[i])) i++;
            i--;
            expectNumber = false;
        }
        else {
            return false;
        }
    }
    return balance == 0;
}

int main() {
    setlocale(LC_ALL, "Russian");
    cout << "=== ЛАБОРАТОРНАЯ РАБОТА: ДЕРЕВЬЯ ===" << endl;

    string input;
    ifstream file("tree.txt");

    if (file.is_open()) {
        getline(file, input);
        file.close();
        cout << "Из файла: " << input << endl;
    }
    else {
        input = "(8 (9 (5)) (1))";
        cout << "Файл не найден, используем пример: " << input << endl;
    }

    if (!validateInput(input)) {
        cout << "ОШИБКА: Неправильный формат скобочной записи!" << endl;
        return 1;
    }

    TreeParser parser;
    BinaryTree* binaryTree = nullptr;

    try {
        binaryTree = parser.parse(input);
        cout << "✓ Обычное дерево создано!" << endl;

        vector<int> dfsResult = binaryTree->dfs();
        cout << "Обход в глубину: ";
        for (int val : dfsResult) cout << val << " ";
        cout << endl;

    }
    catch (const exception& e) {
        cout << "Ошибка парсинга: " << e.what() << endl;
        return 1;
    }

    AVLTree avlTree;
    vector<int> elements = binaryTree->dfs();
    for (int val : elements) avlTree.insert(val);

    cout << "\n✓ АВЛ дерево создано!" << endl;

    cout << "\nДемонстрация поиска:" << endl;
    for (int val : elements) {
        cout << "Поиск " << val << ": " << (avlTree.search(val) ? "найден" : "не найден") << endl;
    }

    cout << "\nОБХОД В ШИРИНУ: ";
    vector<int> bfs = avlTree.bfs();
    for (int val : bfs) cout << val << " ";

    cout << "\nПРЯМОЙ ОБХОД: ";
    vector<int> pre = avlTree.preorder();
    for (int val : pre) cout << val << " ";

    cout << "\nЦЕНТРИРОВАННЫЙ: ";
    vector<int> in = avlTree.inorder();
    for (int val : in) cout << val << " ";

    cout << "\nОБРАТНЫЙ: ";
    vector<int> post = avlTree.postorder();
    for (int val : post) cout << val << " ";
    cout << endl;

    if (!elements.empty()) {
        int toRemove = elements[0];
        cout << "\nДемонстрация удаления: удаляем " << toRemove << endl;
        avlTree.remove(toRemove);
        cout << "После удаления - обход в ширину: ";
        vector<int> afterRemove = avlTree.bfs();
        for (int val : afterRemove) cout << val << " ";
        cout << endl;
    }

    delete binaryTree;
    return 0;
}