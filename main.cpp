#include <iostream>
#include <string>
#include <algorithm>
#include <utility>
#include "container/associate/jr_set.h"
#include "iterator/jr_iterator.h"

int main() {
    jr_std::multiset<int> n;
    n.insert(1);
    n.insert(2);
    n.insert(3);
    n.insert(3);
    n.insert(3);
    n.insert(4);
    n.insert(4);
    n.erase(4);
    for(auto it = n.begin(); it != n.end(); ++it)
        std::cout << *it << std::endl;

    return 0;
}
