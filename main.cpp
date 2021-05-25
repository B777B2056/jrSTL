#include <iostream>
#include <string>
#include <random>
#include <functional>
#include "algorithm/jr_algorithm.h"

int main() {
    std::array<int, 10> s = {5, 7, 4, 2, 8, 6, 1, 9, 0, 3};

        // 用默认的 operator< 排序
        jr_std::sort(s.begin(), s.end());
        for (auto a : s) {
            std::cout << a << " ";
        }
        std::cout << '\n';

        // 用标准库比较函数对象排序
        jr_std::sort(s.begin(), s.end(), std::greater<int>());
        for (auto a : s) {
            std::cout << a << " ";
        }
        std::cout << '\n';

        // 用自定义函数对象排序
        struct {
            bool operator()(int a, int b) const
            {
                return a < b;
            }
        } customLess;
        jr_std::sort(s.begin(), s.end(), customLess);
        for (auto a : s) {
            std::cout << a << " ";
        }
        std::cout << '\n';

        // 用 lambda 表达式排序
        jr_std::sort(s.begin(), s.end(), [](int a, int b) {
            return b < a;
        });
        for (auto a : s) {
            std::cout << a << " ";
        }
        std::cout << '\n';
    return 0;
}
