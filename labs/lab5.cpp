#include <iostream>
#include <string>
using namespace std;


struct Station {
    string name;
    Station* prev;
    Station* next;
    Station(const string& n) : name(n), prev(NULL), next(NULL) {}
};

class TrainRoute {
public:
    TrainRoute(int n);
    void print() const; // provided for convenience
    int distance(const string& from, const string& to) const;
    void add(const string& name, const string& prevName, const string& nextName);
    void remove(const string& name);

private:
    Station* head;
    Station* find(const string& name) const; // provided for convienience
};


TrainRoute::TrainRoute(int n) 
{
    head = NULL;
    string name[100], prevName[100], nextName[100];
    Station* nodes[100];
    for (int i = 0; i < n; i++) {
        cin >> name[i] >> prevName[i] >> nextName[i];
        nodes[i] = new Station(name[i]);
    }
    for (int i = 0; i < n ; i++) {
        for (int j = 0; j < n; j++) {
            if (prevName[i] == name[j]) {
                nodes[i]->prev = nodes[j];
            }
            if (nextName[i] == name[j]) {
                nodes[i]->next = nodes[j];
            }
        }
    }
    for (int i = 0; i < n; i++) {
        if (nodes[i]->prev == NULL) {
            head = nodes[i];
            break;
        }
    }
}

Station* TrainRoute::find(const string& name) const {
    Station* curr = head;
    while (curr != NULL) {
        if (curr->name == name) return curr;
        curr = curr->next;
    }
    return NULL;
}

void TrainRoute::print() const { // do not modify
    Station* curr = head;
    while (curr != NULL) {
        cout << curr->name;
        if (curr->next != NULL) cout << " <-> ";
        curr = curr->next;
    }
    cout << endl;
}

int TrainRoute::distance(const string& from, const string& to) const {
    Station* start = find(from);
    Station* end = find(to);
    if (!start || !end) return -1;
    int forward = 0;
    Station* curr = start;
    while (curr != NULL && curr != end) {
        curr = curr->next;
        forward++;
    }
    if (curr == end) return forward;
    int backward = 0;
    curr = start;
    while (curr != NULL && curr != end) {
        curr = curr->prev;
        backward++;
    }
    if (curr == end) return backward;
    
    return -1;
}

void TrainRoute::add(const string& name, const string& prevName, const string& nextName) {
    Station* prevStation = find(prevName);
    Station* nextStation = find(nextName);
    Station* newStation = new Station(name);

    newStation->prev = prevStation;
    newStation->next = nextStation;

    if (prevStation != NULL) {
        prevStation->next = newStation;
    }
    if (nextStation != NULL) {
        nextStation->prev = newStation; 
    }
}

void TrainRoute::remove(const string& name) {
    Station* target = find(name);
    if (!target) return;
    if (target->prev != NULL) {
        target->prev->next = target->next;
    }
    if (target->next != NULL) {
        target->next->prev = target->prev;
    }
    if (target == head) {
        head = target->next;
    }
    delete target;
    }

int main() { // do not modify
    int n;
    cin >> n;
    TrainRoute route(n);

    int q;
    cin >> q;
    for (int i = 0; i < q; i++) {
        char cmd;
        cin >> cmd;
        if (cmd == 'P') {
            route.print();
        } else if (cmd == 'D') {
            string a, b;
            cin >> a >> b;
            cout << route.distance(a, b) << endl;
        } else if (cmd == 'A') {
            string name, prev, next;
            cin >> name >> prev >> next;
            route.add(name, prev, next);
        } else if (cmd == 'R') {
            string name;
            cin >> name;
            route.remove(name);
        }
    }
}