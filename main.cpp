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
    jr_std::deque<int> d1(15, -1), d2(4, -2);
    jr_std::deque<int>::const_iterator it = d1.cbegin();
    it += 3;
    d1.insert(it, d2.begin(), d2.end());
    print(d1);
    return 0;
}
