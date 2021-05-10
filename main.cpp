#include <iostream>
#include <string>
#include <set>
#include "container/sequence/jr_array.h"
#include "container/sequence/jr_list.h"
#include "functional/jr_functional.h"

std::ostream& operator<<(std::ostream& ostr, const jr_std::list<int>& list)
{
    for (auto &i : list) {
        ostr << " " << i;
    }
    return ostr;
}

int main() {

    jr_std::list<int> list1;
        jr_std::list<int> list2;
        jr_std::array<int, 7> a1 = { 5,9,0,1,3,20,-1097 };
        jr_std::array<int, 5> a2 = { 8,7,2,6,4 };
        for(auto v : a1)
            list1.push_back(v);
        for(auto v : a2)
            list2.push_back(v);
        list1.sort();
        list2.sort();
        std::cout << "list1:  " << list1 << "\n";
        std::cout << "list2:  " << list2 << "\n";
        list1.merge(list2);
        std::cout << "merged: " << list1 << "\n";
    return 0;
}
