#include <iostream>
#include <string>
#include "container/sequence/jr_deque.h"

template <class T>
void print(const jr_std::deque<T>& d) {
    for(auto it = d.begin(); it != d.end(); it++) {
        std::cout << *it << " ";
    }
    std::cout << d.size() << std::endl;
}

int main() {
    jr_std::deque<int> d1(15, -1);
    d1.clear();
    print(d1);
    return 0;
}
