#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

// Sentinel used by main() for wildcard in Find
static const int WILDCARD = -999999;

template <class DT>
struct NestedBST {
    // ----- Node fields -----
    DT value;                 // value at this dimension (for this node)
    bool hasValue = false;    // becomes true when first assigned
    int dimension = 0;        // which dimension this node represents

    NestedBST *left = nullptr;
    NestedBST *right = nullptr;
    NestedBST *innerTree = nullptr; // equality chain → next dimension

    // keys reachable through the equality chain below this node
    vector<int> keys; // required by spec

    // ----- Global (per-type) configuration -----
    static int NUM_DIMS;      // set from main()

    // ----- Ctors / Dtor -----
    NestedBST() = default;
    NestedBST(DT v, int dim) {
        value = v;
        hasValue = true;
        dimension = dim;
    }

    ~NestedBST() {
        delete left;   left = nullptr;
        delete right;  right = nullptr;
        delete innerTree; innerTree = nullptr;
    }

    // ----- Small helpers for maintaining ancestor key lists -----
    void removeKeyFromNode(int victimKey) {
        for (size_t i = 0; i < keys.size(); ++i) {
            if (keys[i] == victimKey) {
                keys.erase(keys.begin() + static_cast<long long>(i));
                break;
            }
        }
    }

    void addKeyToNodeIfMissing(int newKey) {
        for (int k : keys) {
            if (k == newKey) return;
        }
        keys.push_back(newKey);
    }

    // ----- Public API per spec -----
    void insert(int key, const vector<DT>& values) {
        vector<NestedBST<DT>*> path; // ancestors along equality chain (this dim and above)
        insertHelper(key, values, path);
    }

    void find(const vector<DT>& pattern) {
        bool printedAny = false;
        vector<DT> current;
        findHelper(pattern, 0, current, printedAny);
        if (!printedAny) cout << "EMPTY" << '\n';
    }

    void display(int indent = 0) {
        displayHelper(indent);
    }

private:
    NestedBST<DT>* ensureChildAtSameDim(NestedBST<DT>*& child, DT currentValue) {
        if (child == nullptr) {
            child = new NestedBST<DT>(currentValue, dimension); // same dimension for left/right
        }
        return child;
    }

    NestedBST<DT>* ensureInner(DT nextValue) {
        if (innerTree == nullptr) {
            innerTree = new NestedBST<DT>(nextValue, dimension + 1);
        }
        return innerTree;
    }

    void printTupleWithKey(int key, const vector<DT>& tuple) {
        cout << "key=" << key << " for (";
        for (int d = 0; d < static_cast<int>(tuple.size()); ++d) {
            if (d) cout << ",";
            cout << tuple[d];
        }
        cout << ")" << '\n';
    }

    void insertHelper(int key, const vector<DT>& values,
                      vector<NestedBST<DT>*>& path) {
        DT currentValue = values[dimension];

        if (!hasValue) {
            value = currentValue;
            hasValue = true;
        }

        if (currentValue < value) {
            ensureChildAtSameDim(left, currentValue)->insertHelper(key, values, path);
            return;
        } else if (currentValue > value) {
            ensureChildAtSameDim(right, currentValue)->insertHelper(key, values, path);
            return;
        } else {
            // Equality-chain path includes this node
            vector<NestedBST<DT>*> path2 = path;
            path2.push_back(this);

            if (dimension == NUM_DIMS - 1) {
                // We are at the last dimension (leaf)
                bool isNew = false, isUnchanged = false, isUpdate = false;
                int oldKey = -1;

                if (keys.empty()) {
                    isNew = true;
                    keys.push_back(key);
                } else if (keys[0] == key) {
                    isUnchanged = true;
                } else {
                    isUpdate = true;
                    oldKey = keys[0];
                    keys[0] = key;
                }

                // Update ancestor candidate-key lists
                if (isNew) {
                    for (auto *node : path2) node->addKeyToNodeIfMissing(key);
                } else if (isUpdate) {
                    for (auto *node : path2) node->removeKeyFromNode(oldKey);
                    for (auto *node : path2) node->addKeyToNodeIfMissing(key);
                }

                // Print required message
                cout << (isNew      ? "Inserted key="
                      : isUnchanged ? "Unchanged key="
                                    : "Updated key=")
                     << key << " for (";
                for (int d = 0; d < NUM_DIMS; ++d) {
                    if (d) cout << ",";
                    cout << values[d];
                }
                cout << ")" << '\n';
                return;
            }

            // Need to go deeper to next dimension
            addKeyToNodeIfMissing(key); // this equality node participates for this key
            ensureInner(values[dimension + 1])->insertHelper(key, values, path2);
        }
    }

    void findHelper(const vector<DT>& pattern, int d,
                    vector<DT>& prefix, bool &printedAny) {
        if (!hasValue) return; // empty tree

        bool isWild = (pattern[d] == WILDCARD);
        DT target = pattern[d];

        // Left branch
        if (isWild || target < value) {
            if (left) left->findHelper(pattern, d, prefix, printedAny);
        }

        // Equality chain
        if (isWild || target == value) {
            prefix.push_back(value);
            if (dimension == NUM_DIMS - 1) {
                if (!keys.empty()) {
                    printTupleWithKey(keys[0], prefix);
                    printedAny = true;
                }
            } else if (innerTree) {
                innerTree->findHelper(pattern, d + 1, prefix, printedAny);
            }
            prefix.pop_back();
        }

        // Right branch
        if (isWild || target > value) {
            if (right) right->findHelper(pattern, d, prefix, printedAny);
        }
    }

    void displayHelper(int indent) {
        if (!hasValue) return;

        if (left) left->displayHelper(indent);

        for (int i = 0; i < indent; ++i) cout << ' ';
        cout << "[dim " << dimension << "] value=" << value
             << "  (candidates=" << static_cast<int>(keys.size()) << ")" << '\n';

        if (innerTree) {
            for (int i = 0; i < indent; ++i) cout << ' ';
            cout << "  -> dim " << (dimension + 1) << '\n';
            if (dimension == NUM_DIMS - 2) {
                innerTree->displayLeaves(indent + 2);
            } else {
                innerTree->displayHelper(indent + 2);
            }
        }

        if (right) right->displayHelper(indent);
    }

    void displayLeaves(int indent) {
        if (!hasValue) return;
        if (left) left->displayLeaves(indent);
        for (int i = 0; i < indent; ++i) cout << ' ';
        cout << "[dim " << (NUM_DIMS - 1) << "] value=" << value << "  key=";
        if (!keys.empty()) cout << keys[0];
        cout << '\n';
        if (right) right->displayLeaves(indent);
    }
};

template<class DT>
int NestedBST<DT>::NUM_DIMS = 0;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int numDimensions;
    if (!(cin >> numDimensions)) return 0;
    NestedBST<int>::NUM_DIMS = numDimensions;

    auto *root = new NestedBST<int>();
    root->dimension = 0;

    int numCommands;
    cin >> numCommands;

    char command;
    for (int i = 0; i < numCommands; i++) {
        cin >> command;
        switch (command) {
            case 'I': {
                int key;
                cin >> key;
                vector<int> values(numDimensions);
                for (int d = 0; d < numDimensions; d++) cin >> values[d];
                root->insert(key, values);
                break;
            }
            case 'F': {
                vector<int> pattern(numDimensions);
                string token;
                for (int d = 0; d < numDimensions; d++) {
                    cin >> token;
                    if (token == "*") pattern[d] = WILDCARD;
                    else pattern[d] = stoi(token);
                }
                root->find(pattern);
                break;
            }
            case 'D': {
                cout << "NestedBST Structure:" << '\n';
                root->display();
                cout << '\n';
                break;
            }
            default:
                cout << "Unknown command: " << command << '\n';
                break;
        }
    }

    delete root;
    return 0;
}
