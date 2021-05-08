#include <iostream>
#include <string>
#include "container/sequence/jr_forward_list.h"
#include "container/sequence/jr_vector.h"

std::ostream& operator<<(std::ostream& ostr, const jr_std::forward_list<int>& list)
{
    for (auto &i : list) {
        ostr << " " << i;
    }
    return ostr;
}

int main() {
    jr_std::forward_list<int> list1;
    jr_std::forward_list<int> list2;
    int m1[] = { 5,9,0,1,3};
    int m2[] = { 8,7,2,6,4 };
    for(int i = 4; i>=0; i--) {
        list1.push_front(m1[i]);
        list2.push_front(m2[i]);
    }
    list1.sort();
    list2.sort();
    std::cout << "list1:  " << list1 << "\n";
    std::cout << "list2:  " << list2 << "\n";
    list1.merge(list2);
    std::cout << "merged: " << list1 << "\n";


    return 0;
}
