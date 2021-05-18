#include <iostream>
#include "container/sequence/jr_vector.h"

int main() {
//    jr_std::istream_iterator<int> in_iter(std::cin), eof;
//    jr_std::vector<int> v(in_iter, eof);
    jr_std::vector<int> v;
    auto bi = jr_std::inserter(v, v.begin());
    for(int i = 1; i<=10; i++)
        *bi++ = i;
    jr_std::ostream_iterator<int> out_iter(std::cout, " ");

    for (auto s : v)
        *out_iter++ = s;

    return 0;
}
