#include <iostream>
#include <string>
#include "container/sequence/jr_array.h"

template<class Os, class V> Os& operator<<(Os& os, const V& v) {
    os << "{";
    for (auto i : v) os << ' ' << i;
    return os << " } ";
}

int main() {
    jr_std::array<int, 3> a1{1, 2, 3}, a2{4, 5, 6};

       auto it1 = a1.begin();
       auto it2 = a2.begin();
       int& ref1 = a1[1];
       int& ref2 = a2[1];

       std::cout << a1 << a2 << *it1 << ' ' << *it2 << ' ' << ref1 << ' ' << ref2 << '\n';
       a1.swap(a2);
       std::cout << a1 << a2 << *it1 << ' ' << *it2 << ' ' << ref1 << ' ' << ref2 << '\n';

    return 0;
}
