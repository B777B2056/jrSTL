#include <iostream>
#include <string>
#include <algorithm>
#include <utility>
#include "container/associate/jr_map.h"
#include "iterator/jr_iterator.h"

int main() {
    jr_std::multimap<int, char> n;
    n.insert(jr_std::pair<const int,char>(1, 'a'));
    n.insert(jr_std::pair<const int,char>(1, 'b'));
    n.insert(jr_std::pair<const int,char>(1, 'c'));
    n.insert(jr_std::pair<const int,char>(2, 'd'));
    n.insert(jr_std::pair<const int,char>(3, 'e'));
    n.insert(jr_std::pair<const int,char>(4, 'f'));
    n.erase(1);
    for(auto it = n.begin(); it != n.end(); ++it)
        std::cout << it->first << " " << it->second << std::endl;

    return 0;
}
