#include <bits/stdc++.h>
using namespace std;

// ---------------- timeValueNode ----------------
struct timeValueNode {
    int value;
    int timeFrame;
    timeValueNode* next;
    timeValueNode(int v, int t) : value(v), timeFrame(t), next(nullptr) {}
};

static void freeList(timeValueNode* head) {
    while (head) { timeValueNode* n = head->next; delete head; head = n; }
}

static void displayList(timeValueNode* head) {
    timeValueNode* cur = head;
    bool first = true;
    while (cur) {
        cout << "(" << cur->value << ", " << cur->timeFrame << ")";
        cur = cur->next;
        if (cur) cout << " ";
    }
}

static void insertOrdered(timeValueNode*& head, int val, int tFrame) {
    if (!head) { head = new timeValueNode(val, tFrame); return; }
    if (tFrame < head->timeFrame) {
        timeValueNode* n = new timeValueNode(val, tFrame);
        n->next = head; head = n; return;
    }
    timeValueNode* prev = nullptr;
    timeValueNode* cur = head;
    while (cur && cur->timeFrame < tFrame) { prev = cur; cur = cur->next; }
    if (cur && cur->timeFrame == tFrame) {
        cur->value = val; // overwrite
        return;
    }
    timeValueNode* n = new timeValueNode(val, tFrame);
    if (!prev) { n->next = head; head = n; }
    else { prev->next = n; n->next = cur; }
}

static void removeByTime(timeValueNode*& head, int tFrame) {
    if (!head) return;
    if (head->timeFrame == tFrame) {
        timeValueNode* tmp = head; head = head->next; delete tmp; return;
    }
    timeValueNode* prev = head;
    timeValueNode* cur = head->next;
    while (cur) {
        if (cur->timeFrame == tFrame) {
            prev->next = cur->next; delete cur; return;
        }
        prev = cur; cur = cur->next;
    }
}

// value effective at tFrame: last node with timeFrame <= tFrame, else 0
static int findAt(timeValueNode* head, int tFrame) {
    int ans = 0;
    timeValueNode* cur = head;
    while (cur && cur->timeFrame <= tFrame) {
        ans = cur->value;
        cur = cur->next;
    }
    return ans;
}

// ---------------- sparseRow ----------------
struct sparseRow {
    int rowIndex = 0;
    int colIndex = 0;
    timeValueNode* head = nullptr;

    ~sparseRow() { freeList(head); }

    void insertTV(int val, int tFrame) { insertOrdered(head, val, tFrame); }
    void removeTime(int tFrame) { removeByTime(head, tFrame); }
    int  findAtT(int tFrame) const { return findAt(head, tFrame); }

    void display() const {
        cout << "Row: " << rowIndex << ", Col: " << colIndex << " -> ";
        displayList(head);
        cout << "\n";
    }

    // pointer-safe ownership swap
    void swap(sparseRow& other) noexcept {
        std::swap(rowIndex, other.rowIndex);
        std::swap(colIndex, other.colIndex);
        std::swap(head, other.head);
    }
};

// ---------------- TemporalSparseMatrix ----------------
class TemporalSparseMatrix {
public:
    int rows;
    int cols;
    int MAX_NONZERO;
    int currNZ;
    sparseRow* entries;

    TemporalSparseMatrix(int r, int c, int maxNZ)
        : rows(r), cols(c), MAX_NONZERO(maxNZ), currNZ(0) {
        entries = new sparseRow[MAX_NONZERO];
    }

    ~TemporalSparseMatrix() {
        delete[] entries; // each sparseRow destructor frees its list
    }

    int indexOf(int r, int c) const {
        for (int i = 0; i < currNZ; ++i) {
            if (entries[i].rowIndex == r && entries[i].colIndex == c) return i;
        }
        return -1;
    }

    void insert(int r, int c, int val, int t) {
        int idx = indexOf(r, c);
        if (idx == -1) {
            if (currNZ >= MAX_NONZERO) return; // ignore if full
            entries[currNZ].rowIndex = r;
            entries[currNZ].colIndex = c;
            entries[currNZ].head = nullptr;
            entries[currNZ].insertTV(val, t);
            currNZ++;
        } else {
            entries[idx].insertTV(val, t);
        }
        cout << "Inserted value " << val << " at (" << r << ", " << c << ") for timeframe " << t << "\n";
    }

    void remove(int r, int c, int t) {
        int idx = indexOf(r, c);
        if (idx != -1) {
            entries[idx].removeTime(t);
            if (entries[idx].head == nullptr) {
                for (int i = idx; i < currNZ - 1; ++i) {
                    entries[i].swap(entries[i + 1]); // stable shift via swap
                }
                entries[currNZ - 1] = sparseRow();
                currNZ--;
            }
        }
        cout << "Removed value at (" << r << ", " << c << ") for timeframe " << t << "\n";
    }

    int find(int r, int c, int t) const {
        int idx = indexOf(r, c);
        if (idx == -1) return 0;
        return entries[idx].findAtT(t);
    }

    int size() const { return currNZ; }

    void display() const {
        cout << "Temporal Sparse Matrix:\n";
        for (int i = 0; i < currNZ; ++i) {
            entries[i].display();
        }
    }
};

// ---------------- main ----------------
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int R, C, MAXNZ;
    if (!(cin >> R >> C >> MAXNZ)) return 0;

    int numOps; 
    if (!(cin >> numOps)) numOps = INT_MAX; // if missing, just run to EOF

    TemporalSparseMatrix tsm(R, C, MAXNZ);

    for (int op = 0; op < numOps && cin; ++op) {
        char cmd;
        if (!(cin >> cmd)) break;
        if (cmd == 'I') {
            int r, c, v, t; cin >> r >> c >> v >> t;
            tsm.insert(r, c, v, t);
        } else if (cmd == 'F') {
            int r, c, t; cin >> r >> c >> t;
            int val = tsm.find(r, c, t);
            cout << "Found value at (" << r << ", " << c << ") for timeframe " << t << ": " << val << "\n";
        } else if (cmd == 'R') {
            int r, c, t; cin >> r >> c >> t;
            tsm.remove(r, c, t);
        } else if (cmd == 'S') {
            cout << "Current number of non-zero entries: " << tsm.size() << "\n";
        } else if (cmd == 'D') {
            tsm.display();
        } else {
            // ignore unknown command token and remainder of line
            string rest; getline(cin, rest);
        }
    }

    return 0;
}
