#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

class MinMaxHeap {
private:
    vector<int> heap;

    // index helpers
    int parent(int i) const {
        return (i - 1) / 2;
    }

    int left(int i) const {
        return 2 * i + 1;
    }

    int right(int i) const {
        return 2 * i + 2;
    }

    void swapNodes(int a, int b) {
        int temp = heap[a];
        heap[a] = heap[b];
        heap[b] = temp;
    }

    // true if node i is on a min level, false if on a max level
    bool isMinLevel(int i) const {
        int level = 0;
        while (i > 0) {
            i = parent(i);
            level++;
        }
        // level 0 (root) is min, then they alternate
        return (level % 2 == 0);
    }

    // bubble up on min levels using grandparent comparisons
    void bubbleUpMin(int index) {
        // need a grandparent, so index >= 3
        while (index >= 3) {
            int grandparent = parent(parent(index));
            if (heap[index] < heap[grandparent]) {
                swapNodes(index, grandparent);
                index = grandparent;
            } else {
                break;
            }
        }
    }

    // bubble up on max levels using grandparent comparisons
    void bubbleUpMax(int index) {
        while (index >= 3) {
            int grandparent = parent(parent(index));
            if (heap[index] > heap[grandparent]) {
                swapNodes(index, grandparent);
                index = grandparent;
            } else {
                break;
            }
        }
    }

    // main bubbleUp function described in the handout
    void bubbleUp(int index) {
        if (index == 0) return;   // new root

        int p = parent(index);

        if (isMinLevel(index)) {
            // node is on a min level
            if (heap[index] > heap[p]) {
                // too big for min level, should move towards max level
                swapNodes(index, p);
                bubbleUpMax(p);
            } else {
                bubbleUpMin(index);
            }
        } else {
            // node is on a max level
            if (heap[index] < heap[p]) {
                // too small for max level
                swapNodes(index, p);
                bubbleUpMin(p);
            } else {
                bubbleUpMax(index);
            }
        }
    }

public:
    // insert described in the handout
    void insert(int value) {
        heap.push_back(value);                      // Step 1: append to vector
        bubbleUp(static_cast<int>(heap.size()) - 1); // Step 2: bubbleUp on last index
    }

    // prints the heap level by level
    void display() const {
        cout << "Min-Max Heap (level by level):" << endl;

        int n = static_cast<int>(heap.size());
        int levelCount = 1; // how many nodes should be on this level
        int printed = 0;    // how many printed on current level

        for (int i = 0; i < n; ++i) {
            cout << heap[i] << " ";
            printed++;

            if (printed == levelCount) {
                cout << endl;
                levelCount *= 2;
                printed = 0;
            }
        }

        if (printed != 0) {
            cout << endl;
        }
    }
};

int main() {
    MinMaxHeap h;

    int x;
    while (cin >> x) {
        h.insert(x);
    }

    h.display();

    return 0;
}
