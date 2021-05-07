#include <iostream>
#include <string>
#include "container/sequence/jr_vector.h"
#include "container/sequence/jr_deque.h"
#include "container/adapter/jr_priority_queue.h"

using my_pair_t = std::pair<size_t,bool>;

using my_container_t = jr_std::vector<my_pair_t>;

template<typename T> void print_queue(T& q) {
    while(!q.empty()) {
        std::cout << q.top() << " ";
        q.pop();
    }
    std::cout << '\n';
}

int main() {
    auto my_comp =
            [](const my_pair_t& e1, const my_pair_t& e2)
            { return e1.first > e2.first; };
    jr_std::priority_queue<my_pair_t,
                        my_container_t,
                        decltype(my_comp)> queue(my_comp);
    queue.push(std::make_pair(5, true));
    queue.push(std::make_pair(3, false));
    queue.push(std::make_pair(7, true));
    std::cout << std::boolalpha;
    while(!queue.empty())
    {
        const auto& p = queue.top();
        std::cout << p.first << " " << p.second << "\n";
        queue.pop();
    }

    return 0;
}
