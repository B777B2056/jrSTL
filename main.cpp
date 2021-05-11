#include <iostream>
#include <string>
#include "container/associate/jr_set.h"

int main() {
    jr_std::set<int> avl;
    for(int i = -1000; i<1000; i++)
        avl.insert(i);
    auto it = avl.begin();
    while(it != avl.end()) {
        std::cout << *it << " ";
        ++it;
    }
    return 0;
}
