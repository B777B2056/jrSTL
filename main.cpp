#include <iostream>
#include <random>
#include <functional>
#include <algorithm>
#include "container/sequence/jr_vector.h"
#include "algorithm/jr_algorithm.h"

template<class Iter>
void merge_sort(Iter first, Iter last)
{
    if (last - first > 1) {
        Iter middle = first + (last - first) / 2;
        merge_sort(first, middle);
        merge_sort(middle, last);
        std::cout << "Middle = " << *middle;
        std::cout << ", Before merge: ";
        for(Iter i = first; i != last; i++) {
            std::cout << *i << " ";
        }
        std::cout << std::endl;
        jr_std::inplace_merge(first, middle, last);
        std::cout << "After merge: ";
        for(Iter i = first; i != last; i++) {
            std::cout << *i << " ";
        }
        std::cout << std::endl;
    }
}


int main() {
    jr_std::vector<int> v;
    int m[] = {8, 2, -2, 0, 11, 11, 1, 7, 3};
    for(int n : m)
        v.push_back(n);
    merge_sort(v.begin(), v.end());
    for(auto n : v) {
        std::cout << n << ' ';
    }
    std::cout << '\n';

    return 0;
}
