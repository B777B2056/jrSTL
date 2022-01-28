#include <gtest/gtest.h>
#include <random>
#include <iostream>
#include <array>
#include "../container/sequence/jr_array.h"

#define MAX_SIZE 2000

void get_random_size_var(size_t max_size, size_t& size, int& var, size_t min_size = 0);

// 赋值构造函数与元素访问测试
TEST(testCase,array_initlist_test){
    // 赋值构造函数测试
    std::array<int, 16> src{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    jrSTL::array<int, 16> des{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    ASSERT_EQ(src.size(), des.size());
    // 索引遍历测试
    for(size_t i = 0; i < src.size(); i++)
        EXPECT_EQ(src[i], des[i]);
    // at成员函数遍历测试
    for(size_t i = 0; i < src.size(); i++)
        EXPECT_EQ(src.at(i), des.at(i));
    // 迭代器遍历测试
    auto sit = src.begin();
    auto dit = des.begin();
    for(; sit != src.end(); ++sit)
        EXPECT_EQ(*sit, *dit++);
    // 头尾访问测试
    EXPECT_EQ(src.front(), des.front());
    EXPECT_EQ(src.back(), des.back());
}

// fill测试
TEST(testCase,array_fill_test){
    // 赋值构造函数测试
    std::array<int, 16> src{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    jrSTL::array<int, 16> des{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    src.fill(-100);
    des.fill(-100);
    ASSERT_EQ(src.size(), des.size());
    // 索引遍历测试
    for(size_t i = 0; i < src.size(); i++)
        EXPECT_EQ(src[i], des[i]);
}

// swap测试
TEST(testCase, array_swap_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jrSTL::array<int,16> src{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    jrSTL::array<int,16> des{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    jrSTL::array<int,16> tmp = src;
    src.swap(des);
    ASSERT_EQ(tmp.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(tmp[i], des[i]);
    des.swap(src);
    ASSERT_EQ(tmp.size(), src.size());
    for(size_t i = 0; i < src.size(); ++i)
        EXPECT_EQ(tmp[i], src[i]);
}

// 逆序访问测试
TEST(testCase, array_reverse_test) {
    std::array<int, 16> src = {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    jrSTL::array<int, 16> des = {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    auto it = src.rbegin();
    auto rit = des.rbegin();
    while(rit != des.rend()) {
        EXPECT_EQ(*it, *rit);
        ++rit;
        ++it;
    }
}

