#include <iostream>
#include <string>
#include "container/sequence/jr_vector.h"
#include "container/sequence/jr_list.h"
#include "container/adapter/jr_stack.h"
#include "container/adapter/jr_queue.h"

template <class T>
void print(const jr_std::deque<T>& d) {
    for(auto it = d.begin(); it != d.end(); it++) {
        std::cout << *it << " ";
    }
    std::cout << d.size() << std::endl;
}

int main() {
    jr_std::stack<int, jr_std::vector<int> > s;
    for(int i=0; i<10; i++)
        s.push(i);
    while(!s.empty()) {
        std::cout << s.top() << " ";
        s.pop();
    }
    return 0;
}
