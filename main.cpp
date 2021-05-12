#include <iostream>
#include <string>
#include <chrono>
#include <functional>
#include <iomanip>
#include "container/associate/jr_set.h"
#include "iterator/jr_iterator.h"

int main() {

    jr_std::multiset<int> nums;
    for(int i = 1; i < 10; i++)
        nums.insert(i);
    nums.insert(1);
    nums.insert(1);
    nums.insert(9);
    auto it = nums.begin();
    for(; it != nums.end(); ++it)
        std::cout << *it << " " ;
    return 0;
}
