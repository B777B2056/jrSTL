#include <iostream>
#include <string>
#include <algorithm>
#include <utility>
#include "container/associate/jr_map.h"
#include "iterator/jr_iterator.h"

int main() {
    jr_std::map<int, char> alice;
    jr_std::map<int, char> bob;
    jr_std::map<int, char> eve;

    alice[1] = 'a';
    alice[2] = 'b';
    alice[3] = 'c';
    bob[7] = 'Z';
    bob[8] = 'Y';
    bob[9] = 'X';
    bob[10] = 'W';
    eve[1] = 'a';
    eve[2] = 'b';
    eve[3] = 'c';

    std::cout << std::boolalpha;

    // 比较不相等的容器
    std::cout << "alice == bob returns " << (alice == bob) << '\n';
    std::cout << "alice != bob returns " << (alice != bob) << '\n';
    std::cout << "alice <  bob returns " << (alice < bob) << '\n';
    std::cout << "alice <= bob returns " << (alice <= bob) << '\n';
    std::cout << "alice >  bob returns " << (alice > bob) << '\n';
    std::cout << "alice >= bob returns " << (alice >= bob) << '\n';

    std::cout << '\n';

    // 比较相等的容器
    std::cout << "alice == eve returns " << (alice == eve) << '\n';
    std::cout << "alice != eve returns " << (alice != eve) << '\n';
    std::cout << "alice <  eve returns " << (alice < eve) << '\n';
    std::cout << "alice <= eve returns " << (alice <= eve) << '\n';
    std::cout << "alice >  eve returns " << (alice > eve) << '\n';
    std::cout << "alice >= eve returns " << (alice >= eve) << '\n';

    return 0;
}
