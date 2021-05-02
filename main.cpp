#include <iostream>
#include <string>
#include <list>
#include "iterator/jr_iterator.h"
#include "container/sequence/jr_vector.h"
#include "container/sequence/jr_list.h"

void print_vec(const jr_std::vector<int>& vec)
{
    for (size_t i=0;i<vec.size();i++) {
         std::cout << ' ' << vec[i];
    }
    std::cout << '\n';
}

int main() {
//    jr_std::vector<int> vec(3,100);
//    auto it = vec.begin();
//    it = vec.insert(it, 200);
//    vec.insert(it,2,300);
//    print_vec(vec);
    const jr_std::list<int> m(100,3);
    jr_std::list<int>::const_iterator mit = m.begin();
    return 0;
}
