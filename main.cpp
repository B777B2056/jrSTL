#include <gtest/gtest.h>
#include <iostream>
#include "container/sequence/jr_list.h"

//int main() {
//    jr_std::list<int> des, tmp(6, -100);
//    std::cout << tmp.size();
//    des.assign(tmp.begin(), tmp.end());
//    return 0;
//}

int main(int argc,char **argv){
  testing::InitGoogleTest(&argc,argv);
  return RUN_ALL_TESTS();
}
