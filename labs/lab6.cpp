#include <iostream>
#include <string>
using namespace std;

class Stack {
public:
    Stack();
    ~Stack();
    bool push(char c);
    char pop();
    char peek() const;
    bool isEmpty() const;

private:
    static const int MAX_SIZE = 10000;
    char data[MAX_SIZE];
    int topIndex;
};

Stack::Stack() : topIndex(-1) {}
Stack::~Stack() {}

bool Stack::push(char c) {
    if (topIndex + 1 >= MAX_SIZE) return false; // overflow guard
    data[++topIndex] = c;
    return true;
}

char Stack::pop() {
    if (isEmpty()) return '\0'; // underflow sentinel
    return data[topIndex--];
}

char Stack::peek() const {
    if (isEmpty()) return '\0';
    return data[topIndex];
}

bool Stack::isEmpty() const {
    return topIndex == -1;
}

bool matches(char open, char close) {
    return (open == '(' && close == ')') ||
           (open == '[' && close == ']') ||
           (open == '{' && close == '}');
}

bool isValid(const std::string& s) {
    Stack st;
    for (char c : s) {
        if (c == '(' || c == '[' || c == '{') {
            if (!st.push(c)) return false; // overflow shouldn't happen with given MAX_SIZE
        } else if (c == ')' || c == ']' || c == '}') {
            char top = st.pop();
            if (top == '\0') return false;    // nothing to match
            if (!matches(top, c)) return false; // wrong closer
        } else {
            return false; // invalid character per spec
        }
    }
    return st.isEmpty(); // no unmatched openers
}

int main() {
    string s;
    cin >> s;  // reads a single continuous string without spaces

    if (isValid(s)) {
        cout << "true" << endl;
    } else {
        cout << "false" << endl;
    }
    return 0;
}
