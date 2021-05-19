#include <iostream>
#include <functional>
#include "functional/jr_functional.h"

void f(int& n1, int& n2, const int& n3)
{
    std::cout << "In function: " << n1 << ' ' << n2 << ' ' << n3 << '\n';
    ++n1; // 增加存储于函数对象的 n1 副本
    ++n2; // 增加 main() 的 n2
    // ++n3; // 编译错误
}

int main() {
    std::cout << "Standard placeholder _5 is for the argument number "
              << jr_std::is_placeholder<decltype(jr_std::placeholders::_5)>::value
              << '\n';
    return 0;
}
