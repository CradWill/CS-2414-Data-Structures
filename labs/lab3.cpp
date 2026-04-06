#include <iostream>
using namespace std;

long long superPowerChain(int n) {
    if (n <= 1) return 1;
    return n * superPowerChain(n - 1);
}

long long alienBunnyBoom(int n) {
    if (n <= 2) return 1;
    return alienBunnyBoom(n - 1) + alienBunnyBoom(n - 2);
}

int main() {
    int numHeroes, numMonths;
    cin >> numHeroes >> numMonths;

    cout << "Total Power Of Heroes: " << superPowerChain(numHeroes) << endl;
    cout << "Total Number Of Bunny Pairs: " << alienBunnyBoom(numMonths) << endl;
    return 0;
}
