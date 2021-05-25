#include <iostream>
#include <string>
#include <random>
#include "container/sequence/jr_vector.h"
#include "algorithm/jr_algorithm.h"

int main() {
    jr_std::vector<int> numbers {22, 7, 93, 45, 19, 56, 88, 12, 8, 7, 15, 10};
    size_t count {5}; // Index of nth element
    jr_std::nth_element(jr_std::begin(numbers), jr_std::begin(numbers) + count, jr_std::end(numbers));
    for(int n : numbers)
        std::cout << n << " ";
    std::cout << std::endl;

    jr_std::vector<int> v{5, 6, 4, 3, 2, 6, 7, 9, 3};

        jr_std::nth_element(v.begin(), v.begin() + v.size()/2, v.end());
        std::cout << "The median is " << v[v.size()/2] << '\n';

        jr_std::nth_element(v.begin(), v.begin()+1, v.end(), std::greater<int>());
        std::cout << "The second largest element is " << v[1] << '\n';
        for(int n : v)
            std::cout << n << " ";
        std::cout << std::endl;
    return 0;
}
