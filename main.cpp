#include <iostream>
#include <string>
#include <iomanip>
#include "container/sequence/jr_deque.h"

template <class T>
void print(const jr_std::deque<T>& d) {
    for(auto it = d.begin(); it != d.end(); it++) {
        std::cout << *it << " ";
    }
    std::cout << d.size() << std::endl;
}

int main() {

    return 0;
}
