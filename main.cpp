#include <iostream>
#include <string>
#include <random>
#include <functional>
#include <numeric>
#include <algorithm>
#include "container/sequence/jr_vector.h"
#include "container/sequence/jr_forward_list.h"
#include "algorithm/jr_algorithm.h"

int main() {
    jr_std::vector<int> v;
    for(int n : {5,6,7,8,9,10, -10,-200,0,1,2,3,4,5,6,7,8,9,10})
        v.push_back(n);
        std::cout << "Original vector:\n    ";
        for (int elem : v)
            std::cout << elem << ' ';

        auto it = jr_std::stable_partition(v.begin(), v.end(), [](int i){return i % 2 == 0;});

        std::cout << "\nPartitioned vector:\n    ";
        std::copy(std::begin(v), it, jr_std::ostream_iterator<int>(std::cout, " "));
        std::cout << " * ";
        std::copy(it, std::end(v), jr_std::ostream_iterator<int>(std::cout, " "));

    return 0;
}
