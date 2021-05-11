#include <iostream>
#include <string>
#include "container/utils/jr_tree.h"

int main() {
    jr_std::_Balance_BST<int> avl;
    for(int i = 1; i<=10; i++)
        avl.insert(i);
    std::cout << "Min: " << avl.get_min() << ","
              << "Max: " << avl.get_max() << std::endl;
    avl.print_tree();
    std::cout << std::endl;
    avl.erase(10);
    std::cout << "Min: " << avl.get_min() << ","
              << "Max: " << avl.get_max() << std::endl;
    avl.print_tree();
    std::cout << std::endl;
    avl.erase(9);
    std::cout << "Min: " << avl.get_min() << ","
              << "Max: " << avl.get_max() << std::endl;
    avl.print_tree();
    return 0;
}
