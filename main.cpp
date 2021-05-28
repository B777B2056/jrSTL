#include <gtest/gtest.h>
#include <iostream>
#include "container/sequence/jr_deque.h"

//int main() {
//    jr_std::deque<int> d(8, 1);
////    for(int n : d) {
////        std::cout << n << " ";
////    }
//    std::cout << d.size();
//    return 0;
//}

int main(int argc,char **argv){
  testing::InitGoogleTest(&argc,argv);
  return RUN_ALL_TESTS();
}
