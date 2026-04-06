#include <iostream>
using namespace std;

int main() {
    int numbers[5] = {1, 2, 3, 4, 5};
    int sum = 0;

    // Intent: compute the sum of the array
    for (int i = 0; i <= 5; i++) {  
        sum += numbers[i];
    }

    cout << "The sum is: " << sum << endl;
    return 0;
}