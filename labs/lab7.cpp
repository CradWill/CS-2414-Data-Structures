#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

// node class definition given do not modify
class Node{
private:
    char info;
    Node* left;
    Node* right;
public:
    Node (char val, Node* l = NULL, Node* r = NULL){
        info = val;
        left = l;
        right = r;
    }
    Node* getLeft() const { return left; }
    Node* getRight() const { return right; }
    char   getInfo() const { return info; }

    void setRight(Node* r) { this->right = r; }
    void setLeft(Node* l)  { this->left  = l; }
};

class BinaryTree{
private:
    Node* root;

    // Build from level-order string s, using array indices: left=2*i+1, right=2*i+2
    Node* build(const string& s, int i){
        if (i >= (int)s.size() || s[i] == 'N') return nullptr;
        Node* n = new Node(s[i]);
        n->setLeft (build(s, 2*i + 1));
        n->setRight(build(s, 2*i + 2));
        return n;
    }

    void destroy(Node* n){
        if (!n) return;
        destroy(n->getLeft());
        destroy(n->getRight());
        delete n;
    }

    int height(Node* n){
        if (!n) return 0;
        return 1 + max(height(n->getLeft()), height(n->getRight()));
    }

    void post(Node* n, vector<char>& out){
        if (!n) return;
        post(n->getLeft(), out);
        post(n->getRight(), out);
        out.push_back(n->getInfo());
    }

public:
    BinaryTree(){
        string input;
        if (!(cin >> input)) { root = nullptr; return; }
        root = build(input, 0);
    }

    ~BinaryTree(){ destroy(root); }

    // DFS search by value (not used in build, but implemented if needed)
    Node* search (char a, Node* current){
        if (!current) return nullptr;
        if (current->getInfo() == a) return current;
        if (Node* L = search(a, current->getLeft())) return L;
        return search(a, current->getRight());
    }

    int height(){
        return height(root);
    }

    void postfixExpression(Node* node){
        vector<char> out;
        post(node, out);
        for (size_t i = 0; i < out.size(); ++i){
            if (i) cout << ' ';
            cout << out[i];
        }
        cout << '\n';
    }

    Node* getRoot(){ return root; }
};

int main(){
    BinaryTree tree;
    cout << "Height of tree: " << tree.height() << endl;
    tree.postfixExpression(tree.getRoot());
    return 0;
}
