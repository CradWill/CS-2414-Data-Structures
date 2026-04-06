#include <iostream>
#include <string>
using namespace std;

class Text2Compress {
protected:
    // Limits aligned with grader scale
    static const short int MAX_SEQUENCE = 200000;
    static const int MAX_RULES    = 512;
    static const int MATRIX_DIM   = 640;   // 128 base + up to 512 merges
    static const int ASCII_MAX    = 127;
    static const short int DECODE_BUF   = 300000;

    // Working buffer for encode() output
    int _seq[MAX_SEQUENCE];
    int _length;

    // Pristine original text (as ASCII codes, with CR LF per line)
    int _origSeq[MAX_SEQUENCE];
    int _origLen;

    // Model state
    int _freq[MATRIX_DIM][MATRIX_DIM]; // pair frequencies
    int _rules[MAX_RULES][3];          // (a, b) -> z
    int _ruleCount;
    int _maxSymbol;                    // highest symbol id assigned so far

    // Replace pair (a,b) with z inside arbitrary buffer
    static void replacePairInBuffer(const int* inBuf, int inLen,
                                    int a, int b, int z,
                                    int* outBuf, int& outLen) {
        outLen = 0;
        for (int i = 0; i < inLen; ) {
            if (i + 1 < inLen && inBuf[i] == a && inBuf[i + 1] == b) {
                if (outLen < MAX_SEQUENCE) outBuf[outLen++] = z;
                i += 2;
            } else {
                if (outLen < MAX_SEQUENCE) outBuf[outLen++] = inBuf[i];
                i += 1;
            }
        }
    }

    // Replace on this->_seq
    void replacePair(int a, int b, int z) {
        int out[MAX_SEQUENCE], newLen = 0;
        replacePairInBuffer(_seq, _length, a, b, z, out, newLen);
        _length = newLen;
        for (int i = 0; i < _length; ++i) _seq[i] = out[i];
    }

    // Compute pair frequencies from provided buffer (bounded by 'limit')
    void computeFrequenciesFrom(const int* buf, int len, int limit) {
        for (int i = 0; i < limit; ++i)
            for (int j = 0; j < limit; ++j)
                _freq[i][j] = 0;

        for (int i = 0; i + 1 < len; ++i) {
            int a = buf[i], b = buf[i + 1];
            if (a >= 0 && a < limit && b >= 0 && b < limit)
                _freq[a][b] += 1;
        }
    }

public:
    Text2Compress()
        : _length(0), _origLen(0), _ruleCount(0), _maxSymbol(ASCII_MAX) {
        for (int i = 0; i < MATRIX_DIM; ++i)
            for (int j = 0; j < MATRIX_DIM; ++j)
                _freq[i][j] = 0;
    }

    // Step 1: read 'lines' of text, map to ASCII codes, append CR(13) LF(10)
    void initialize(int /*k*/, int lines) {
        _length = 0;
        string s;
        for (int i = 0; i < lines; ++i) {
            getline(cin, s);
            for (size_t j = 0; j < s.size(); ++j) {
                unsigned char ch = static_cast<unsigned char>(s[j]);
                int v = (ch <= 127) ? int(ch) : 63; // map non-ASCII to '?'
                if (_length < MAX_SEQUENCE) _seq[_length++] = v;
            }
            if (_length < MAX_SEQUENCE) _seq[_length++] = 13; // CR
            if (_length < MAX_SEQUENCE) _seq[_length++] = 10; // LF
        }
        // Preserve pristine original
        _origLen = _length;
        for (int i = 0; i < _origLen; ++i) _origSeq[i] = _seq[i];
    }

    // Step 3: Train K merges. Each iteration:
    //  - re-encode from ORIGINAL by applying previously learned rules
    //  - count pairs on that re-encoded text
    //  - choose most frequent pair BUT ONLY among ASCII (<=127)
    //  - tie-break: smallest a, then b; never pick a zero-frequency pair
    void train(int K) {
        _ruleCount = 0;
        _maxSymbol = ASCII_MAX;

        for (int it = 0; it < K; ++it) {
            // Start from original each iteration
            int tmp[MAX_SEQUENCE];
            int tmpLen = _origLen;
            for (int i = 0; i < tmpLen; ++i) tmp[i] = _origSeq[i];

            // Apply all previously learned rules to tmp
            for (int r = 0; r < _ruleCount; ++r) {
                int a = _rules[r][0], b = _rules[r][1], z = _rules[r][2];
                int outBuf[MAX_SEQUENCE], outLen = 0;
                replacePairInBuffer(tmp, tmpLen, a, b, z, outBuf, outLen);
                tmpLen = outLen;
                for (int i = 0; i < tmpLen; ++i) tmp[i] = outBuf[i];
            }

            // Count pair frequencies on the re-encoded text
            int limit = _maxSymbol + 1;
            if (limit > MATRIX_DIM) break;
            computeFrequenciesFrom(tmp, tmpLen, limit);

            // Select best pair ONLY among ASCII (<=127)
            int bestA = -1, bestB = -1;
            int bestCnt = 0; // must be >0 to accept
            for (int a = 0; a <= ASCII_MAX; ++a) {
                for (int b = 0; b <= ASCII_MAX; ++b) {
                    int c = _freq[a][b];
                    if (c > bestCnt ||
                        (c == bestCnt && c > 0 &&
                         (bestA == -1 || a < bestA || (a == bestA && b < bestB)))) {
                        bestCnt = c; bestA = a; bestB = b;
                    }
                }
            }
            if (bestCnt <= 0) break;

            int z = _maxSymbol + 1;
            if (z >= MATRIX_DIM || _ruleCount >= MAX_RULES) break;

            _rules[_ruleCount][0] = bestA;
            _rules[_ruleCount][1] = bestB;
            _rules[_ruleCount][2] = z;
            _ruleCount++;
            _maxSymbol = z;
        }

        // Reset working sequence to original for the real encode() print
        _length = _origLen;
        for (int i = 0; i < _origLen; ++i) _seq[i] = _origSeq[i];
    }

    // Step 4: print learned rules (caller prints the header)
    void displayRules() {
        for (int i = 0; i < _ruleCount; ++i)
            cout << _rules[i][0] << ' ' << _rules[i][1] << ' ' << _rules[i][2] << '\n';
    }

    // Step 5: apply rules IN ORDER to pristine text and print token ids
    void displaySequence() {
        for (int r = 0; r < _ruleCount; ++r) {
            int a = _rules[r][0], b = _rules[r][1], z = _rules[r][2];
            replacePair(a, b, z);
        }
        for (int i = 0; i < _length; ++i) {
            if (i) cout << ' ';
            cout << _seq[i];
        }
        cout << '\n';
    }

    // Steps 6–7: read triplets + sequence, reverse apply, print text
    void decode() {
        int T;
        if (!(cin >> T)) { cout << '\n'; return; }

        int ra[MAX_RULES], rb[MAX_RULES], rz[MAX_RULES];
        for (int i = 0; i < T; ++i) cin >> ra[i] >> rb[i] >> rz[i];

        short int enc[DECODE_BUF], L = 0, tok;
        while (cin >> tok) {
            if (L < DECODE_BUF) enc[L++] = tok;
        }

        // Reverse merges: last to first
        for (int i = T - 1; i >= 0; --i) {
            int a = ra[i], b = rb[i], z = rz[i];
            short int out[DECODE_BUF], nL = 0;
            for (int j = 0; j < L; ++j) {
                int x = enc[j];
                if (x == z) {
                    if (nL < DECODE_BUF) out[nL++] = a;
                    if (nL < DECODE_BUF) out[nL++] = b;
                } else {
                    if (nL < DECODE_BUF) out[nL++] = x;
                }
            }
            L = nL;
            for (int j = 0; j < L; ++j) enc[j] = out[j];
        }

        for (int i = 0; i < L; ++i) {
            int x = enc[i];
            char ch = (x >= 0 && x <= 127) ? static_cast<char>(x) : '?';
            cout << ch;
        }
        cout << '\n';
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int k, numLines;
    if (!(cin >> k >> numLines)) return 0;

    // Consume any pending CR/LF before the first text line
    while (cin.peek() == '\r' || cin.peek() == '\n') cin.get();

    Text2Compress comp;

    // Step 1/2
    comp.initialize(k, numLines);

    // Step 3
    comp.train(k);

    // Step 4
    cout << "Rules learned from Compression:" << '\n';
    comp.displayRules();

    // Step 5
    cout << "Compressed sequence:" << '\n';
    comp.displaySequence();

    // Step 6/7
    cout << "Decompressed Text:" << '\n';
    comp.decode();

    return 0;
}
