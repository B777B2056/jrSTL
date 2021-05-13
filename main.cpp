#include <iostream>
#include <string>
#include <algorithm>
#include <utility>
#include "container/associate/jr_set.h"
#include "iterator/jr_iterator.h"

int main() {
    jr_std::multiset<int> n;
    int m[] = {3,5,4,3,2,8,5,34,25};
    for(int i : m)
        n.insert(i);
    n.erase(5);
    for(auto it = n.begin(); it != n.end(); ++it)
        std::cout << *it << std::endl;

    return 0;
}
