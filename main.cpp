#include <iostream>
#include <string>
#include <algorithm>
#include <utility>
#include "container/sequence/jr_list.h"

int main() {
    jr_std::list<int> n;
    for(int i = 1; i <= 10; i++)
        n.push_back(i);
    for(auto it = n.begin(); it != n.end(); ++it)
        std::cout << *it << " ";
    std::cout << std::endl;
    for(auto rit = n.rbegin(); rit != n.rend(); ++rit)
        std::cout << *rit << " ";

    return 0;
}
