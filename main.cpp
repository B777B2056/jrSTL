#include <iostream>
#include <string>
#include <algorithm>
#include <utility>
#include "container/associate/jr_unordered_set.h"
#include "iterator/jr_iterator.h"

int main() {
    jr_std::unordered_set<int> n;
    for(int i = 1; i <= 10; i++)
        n.insert(i);
    for(auto it = n.begin(); it != n.end(); ) {
        if((*it) % 2 == 1) {
            it = n.erase(it);
        }else{
            ++it;
        }
    }
    auto it0 = n.begin();
    n.insert(it0, -1);
    for(auto it = n.begin(); it != n.end(); ++it)
        std::cout << *it << " ";

    return 0;
}
