#include <iostream>
#include <string>
#include "container/adapter/jr_stack.h"
#include "memory/jr_allocator.h"

int main() {
    jr_std::allocator<int> alloc;
    jr_std::stack<int> alice;
    int m1[] = { 1,2,3};
    for(int i = 0; i<3; i++) {
        alice.push(m1[i]);
    }
    jr_std::stack<int> m(std::move(alice), alloc);
    std::cout << m.size();

    return 0;
}
