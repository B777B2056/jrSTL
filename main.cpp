#include <iostream>
#include <string>
#include "container/sequence/jr_deque.h"

template <class T>
void print(const jr_std::deque<T>& d) {
    for(auto it = d.begin(); it != d.end(); it++) {
        std::cout << *it << " ";
    }
    std::cout << d.size() << std::endl;
}

struct A {
    std::string s;
    A(std::string str) : s(std::move(str))  { std::cout << " constructed\n"; }
    A(const A& o) : s(o.s) { std::cout << " copy constructed\n"; }
    A(A&& o) : s(std::move(o.s)) { std::cout << " move constructed\n"; }
    A& operator=(const A& other) {
        s = other.s;
        std::cout << " copy assigned\n";
        return *this;
    }
    A& operator=(A&& other) {
        s = std::move(other.s);
        std::cout << " move assigned\n";
        return *this;
    }
};

int main() {
    jr_std::deque<A> container;

        std::cout << "construct 2 times A:\n";
        A two { "two" };
        A three { "three" };

        std::cout << "emplace:\n";
        container.emplace(container.cend(), "one");

        std::cout << "emplace with A&:\n";
        container.emplace(container.cend(), two);

        std::cout << "emplace with A&&:\n";
        container.emplace(container.cend(), std::move(three));

        std::cout << "content:\n";
        for (const auto& obj : container)
            std::cout << ' ' << obj.s;
        std::cout << '\n';
//    int m[]={1,2,3,4,5,6,7,8,9,10};
//    jr_std::deque<int> d;
//    for(int i=0; i<10; i++) {
//        d.push_back(m[i]);
//        d.push_front(m[i]);
//    }
//    print(d);
    return 0;
}
