#include <iostream>
#include <string>
#include <algorithm>
#include <utility>
#include "container/associate/jr_unordered_map.h"

struct IntHash{
    size_t operator()(const jr_std::pair<const int, char>& n) {
        return n.first % 11;
    }
};

struct KeyEqual{
    bool operator()(const jr_std::pair<const int, char>& n1, const jr_std::pair<const int, char>& n2) {
        return n1.first == n2.first;
    }
};

int main() {
    char b = 'a';
    int a[] = {47, 7, 29, 11, 16, 92, 7, 7, 7, 22, 8, 3, 50, 37, 89, 94, 21};
    jr_std::unordered_map<int, char, IntHash, KeyEqual> n(5);
    for(int i = 0; i < 14; i++)
        n.insert(jr_std::pair<const int, char>(a[i], b+i));
    jr_std::allocator<jr_std::pair<const int, char>> al;
    jr_std::unordered_map<int, char, IntHash, KeyEqual> nw(n, al);
    for(auto it = nw.begin(); it != nw.end(); ++it)
        std::cout << it->first << " " << it->second << std::endl;
    for(auto it = n.begin(); it != n.end(); ++it)
        std::cout << it->first << " " << it->second << std::endl;

    return 0;
}
