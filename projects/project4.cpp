#include <iostream>
#include <string>
using namespace std;

/* ============================================================
   Helper functions for row operations
   ============================================================ */

// Return a pointer to row r (mutable)
inline int* rowPtr(int* data, int r, int C) {
    return data + r * C;
}

// Return a pointer to row r (const)
inline const int* rowPtrConst(const int* data, int r, int C) {
    return data + r * C;
}

/* ----------- REQUIRED: Compare two rows A and B lexicographically ------
   Must:
     - Compare column 0, then column 1, … until difference found
     - Return -1, 0, +1 normally
     - Increment cmpCount ONCE per scalar comparison A[col] ? B[col]
   ---------------------------------------------------------------------- */
int compareRows(const int* A, const int* B, int C, long long &cmpCount) {
    for (int col = 0; col < C; ++col) {
        ++cmpCount;                 // 1 per scalar comparison
        if (A[col] < B[col]) return -1;
        if (A[col] > B[col]) return +1;
    }
    return 0;
}

/* ----------- REQUIRED: Swap rows i and j (physically copy C ints) ------
   Must:
     - Swap row i and row j element-by-element
     - Count ONE exchange per row-level swap
   ---------------------------------------------------------------------- */
void rowSwap(int* data, int i, int j, int C, long long &exchanges) {
    if (i == j) return;             // no real exchange
    int* rowI = rowPtr(data, i, C);
    int* rowJ = rowPtr(data, j, C);
    for (int c = 0; c < C; ++c) {
        int tmp   = rowI[c];
        rowI[c]   = rowJ[c];
        rowJ[c]   = tmp;
    }
    ++exchanges;                    // one row-level exchange
}

/* Copy row src → dst (C ints). No counters incremented. */
void copyRowInto(int* dst, const int* src, int C) {
    for (int c = 0; c < C; ++c) dst[c] = src[c];
}

/* ============================================================
   Quick Sort (Lomuto partition)
   ============================================================ */

// Lomuto partition using last row as pivot (row index hi)
int partition_lomuto(int* data, int lo, int hi, int C,
                     long long &cmpCount, long long &exchanges) {
    // Copy pivot row so it won't move while we swap rows
    int* pivot = new int[C];
    copyRowInto(pivot, rowPtrConst(data, hi, C), C);

    int i = lo - 1;
    for (int j = lo; j <= hi - 1; ++j) {
        int* rowJ = rowPtr(data, j, C);
        int cmp = compareRows(rowJ, pivot, C, cmpCount);
        if (cmp <= 0) {             // rowJ <= pivot
            ++i;
            rowSwap(data, i, j, C, exchanges);
        }
    }
    rowSwap(data, i + 1, hi, C, exchanges);
    delete [] pivot;
    return i + 1;
}

void quicksort_rows_rec(int* data, int lo, int hi, int C,
                        long long &cmpCount, long long &exchanges) {
    if (lo < hi) {
        int p = partition_lomuto(data, lo, hi, C, cmpCount, exchanges);
        quicksort_rows_rec(data, lo, p - 1, C, cmpCount, exchanges);
        quicksort_rows_rec(data, p + 1, hi, C, cmpCount, exchanges);
    }
}

void quicksort_rows(int* data, int R, int C,
                    long long &cmpCount, long long &exchanges) {
    if (R <= 1) return;
    quicksort_rows_rec(data, 0, R - 1, C, cmpCount, exchanges);
}


/* ============================================================
   Heap Sort
   ============================================================ */

// Standard max-heapify (sift-down) on rows
void heapify(int* data, int n, int i, int C,
             long long &cmpCount, long long &exchanges) {
    while (true) {
        int largest = i;
        int left  = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < n) {
            const int* rowL = rowPtrConst(data, left, C);
            const int* rowLargest = rowPtrConst(data, largest, C);
            if (compareRows(rowL, rowLargest, C, cmpCount) > 0) {
                largest = left;
            }
        }
        if (right < n) {
            const int* rowR = rowPtrConst(data, right, C);
            const int* rowLargest = rowPtrConst(data, largest, C);
            if (compareRows(rowR, rowLargest, C, cmpCount) > 0) {
                largest = right;
            }
        }

        if (largest != i) {
            rowSwap(data, i, largest, C, exchanges);
            i = largest;            // continue sifting down
        } else {
            break;
        }
    }
}

void heapsort_rows(int* data, int R, int C,
                   long long &cmpCount, long long &exchanges) {
    int n = R;
    if (n <= 1) return;

    // Build max heap
    for (int i = n / 2 - 1; i >= 0; --i) {
        heapify(data, n, i, C, cmpCount, exchanges);
    }

    // Extract max one by one
    for (int i = n - 1; i > 0; --i) {
        rowSwap(data, 0, i, C, exchanges);
        heapify(data, i, 0, C, cmpCount, exchanges);
    }
}


/* ============================================================
   LexSort (Stable insertion passes from rightmost → leftmost column)
   ============================================================ */

void lexsort_lexpass(int* data, int R, int C,
                     long long &cmpCount, long long &exchanges) {
    if (R <= 1 || C <= 0) return;

    int* temp = new int[C];

    // From rightmost column to leftmost
    for (int col = C - 1; col >= 0; --col) {
        // Stable insertion sort by column `col`
        for (int i = 1; i < R; ++i) {
            // Save row i
            copyRowInto(temp, rowPtrConst(data, i, C), C);
            int j = i - 1;

            // Move larger rows one step to the right (stable)
            while (j >= 0) {
                const int* rowJ = rowPtrConst(data, j, C);
                ++cmpCount;                         // compare Table[j][col] vs TEMP[col]
                if (rowJ[col] <= temp[col]) {       // ascending stable condition
                    break;
                }
                // shift row j -> row j+1 (counts as 1 exchange)
                copyRowInto(rowPtr(data, j + 1, C), rowJ, C);
                ++exchanges;
                --j;
            }
            // final placement of TEMP -> row j+1 (counts as 1 exchange)
            copyRowInto(rowPtr(data, j + 1, C), temp, C);
            ++exchanges;
        }
    }

    delete [] temp;
}


/* ============================================================
   Generic dispatcher (DO NOT MODIFY)
   ============================================================ */

enum Alg { QUICK, HEAP, LEX };

void generic_table_sort(int* data, int R, int C, Alg alg,
                        long long &comparisons, long long &exchanges) {
    comparisons = 0;
    exchanges = 0;
    if (R <= 1) return;

    if (alg == QUICK) {
        quicksort_rows(data, R, C, comparisons, exchanges);
    } else if (alg == HEAP) {
        heapsort_rows(data, R, C, comparisons, exchanges);
    } else if (alg == LEX) {
        lexsort_lexpass(data, R, C, comparisons, exchanges);
    }
}


/* ============================================================
   Output block printing (DO NOT MODIFY)
   ============================================================ */

void print_table_block(int t, int R, int C, const string &algName,
                       int* data, long long comps, long long exch) {
    cout << "Table " << t << " (R=" << R << ", C=" << C << ") \u2014 "
         << algName << "\n";
    cout << "Comparisons=" << comps << " Exchanges=" << exch << "\n";

    for (int r = 0; r < R; ++r) {
        const int* row = rowPtrConst(data, r, C);
        for (int c = 0; c < C; ++c) {
            if (c) cout << ' ';
            cout << row[c];
        }
        cout << "\n";
    }
}


/* ============================================================
   Main driver (DO NOT MODIFY)
   Reads T tables, prints Quick / Heap / Lex in correct order
   ============================================================ */

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    if (!(cin >> T)) return 0;

    for (int t = 1; t <= T; ++t) {
        int R, C;
        cin >> R >> C;

        int* A = (R > 0 && C > 0) ? new int[R*C] : nullptr;
        for (int i = 0; i < R*C; ++i) cin >> A[i];

        int* W = (R > 0 && C > 0) ? new int[R*C] : nullptr;

        // ---------------- QUICK ----------------
        for (int i = 0; i < R*C; ++i) W[i] = A[i];
        long long compQ = 0, exchQ = 0;
        generic_table_sort(W, R, C, QUICK, compQ, exchQ);
        print_table_block(t, R, C, "Quick", W, compQ, exchQ);
        cout << "\n";

        // ---------------- HEAP -----------------
        for (int i = 0; i < R*C; ++i) W[i] = A[i];
        long long compH = 0, exchH = 0;
        generic_table_sort(W, R, C, HEAP, compH, exchH);
        print_table_block(t, R, C, "Heap", W, compH, exchH);
        cout << "\n";

        // ---------------- LEX ------------------
        for (int i = 0; i < R*C; ++i) W[i] = A[i];
        long long compL = 0, exchL = 0;
        generic_table_sort(W, R, C, LEX, compL, exchL);
        print_table_block(t, R, C, "Lex", W, compL, exchL);

        delete[] A;
        delete[] W;

        if (t != T) cout << "\n";
    }

    return 0;
}
/* ============================================================
   End of project4.cpp
   ============================================================ */
