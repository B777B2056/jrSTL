#include <gtest/gtest.h>
#include <random>
#include <deque>
#include <iostream>
#include "../container/sequence/jr_deque.h"

#define MAX_SIZE 2000

void get_random_size_var(size_t max_size,
                         size_t& size,
                         int& var,
                         size_t min_size = 0);

// 赋值构造函数与元素访问测试
TEST(testCase,deque_assign_ctor_test){
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    // 赋值构造函数测试
    jr_std::deque<int> des(cnt, var);
    ASSERT_EQ(cnt, des.size());
    // 索引遍历测试
    for(size_t i = 0; i < des.size(); i++)
        EXPECT_EQ(var, des[i]);
    // at成员函数遍历测试
    for(size_t i = 0; i < des.size(); i++)
        EXPECT_EQ(var, des.at(i));
    // 迭代器遍历测试
    auto dit = des.begin();
    for(; dit != des.end(); ++dit)
        EXPECT_EQ(var, *dit);
    // 头尾访问测试
    EXPECT_EQ(var, des.front());
    EXPECT_EQ(var, des.back());
}

// 迭代器范围构造函数测试
TEST(testCase,deque_iterator_ctor_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jr_std::deque<int> src(cnt, var);
    // 迭代器范围构造函数
    jr_std::deque<int> des(src.begin(), src.end());
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < src.size(); i++)
        EXPECT_EQ(src[i], des[i]);
}

// 拷贝构造函数测试
TEST(testCase,deque_copy_ctor_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jr_std::deque<int> src(cnt, var);
    // 拷贝构造函数
    jr_std::deque<int> des(src);
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < src.size(); i++)
        EXPECT_EQ(src[i], des[i]);
}

// 移动构造函数测试
TEST(testCase,deque_move_ctor_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jr_std::deque<int> src(cnt, var);
    // 移动构造函数
    jr_std::deque<int> des(std::move(src));
    ASSERT_EQ(src.empty(), true);
}

// 初始化列表构造函数测试
TEST(testCase,deque_initializer_list_ctor_test) {
    std::deque<int> src{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    // 迭代器范围构造函数
    jr_std::deque<int> des{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < src.size(); i++)
        EXPECT_EQ(src[i], des[i]);
}

// 拷贝运算符测试
TEST(testCase, deque_operator_copy_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jr_std::deque<int> src(cnt, var);
    jr_std::deque<int> des{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    des = src;
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < src.size(); i++)
        EXPECT_EQ(src[i], des[i]);
}

// 移动运算符测试
TEST(testCase, deque_operator_move_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jr_std::deque<int> src(cnt, var);
    jr_std::deque<int> des{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    des = std::move(src);
    EXPECT_EQ(src.empty(), true);
    ASSERT_EQ(cnt, des.size());
    for(size_t i = 0; i < des.size(); i++)
        EXPECT_EQ(var, des[i]);
}

// 赋值成员函数测试
TEST(testCase, deque_assign_mem_fn_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    std::deque<int> src(10, -1);
    jr_std::deque<int> des(15, -4);
    // 重复赋值测试
    src.assign(cnt, var);
    des.assign(cnt, var);
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
    // 初始化列表赋值测试
    src.assign({32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2});
    des.assign({32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2});
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
    // 迭代器赋值测试
    jr_std::deque<int> tmp(6, -100);
    des.assign(tmp.begin(), tmp.end());
    ASSERT_EQ(tmp.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(tmp[i], des[i]);
}

// clear测试
TEST(testCase, deque_clear_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jr_std::deque<int> des(cnt, var);
    des.clear();
    EXPECT_EQ(des.size(), 0);
}

// resize测试
TEST(testCase, deque_resize_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jr_std::deque<int> des(cnt, var);
    std::deque<int> tmp(cnt, var);
    // 新size小于旧size
    des.resize(cnt / 2);
    ASSERT_EQ(des.size(), cnt / 2);
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(tmp[i], des[i]);
    // 新size大于旧size
    des.resize(cnt, var);
    ASSERT_EQ(des.size(), cnt);
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(tmp[i], des[i]);
    // 边界测试：新size等于旧size
    des.resize(cnt, var - 1);
    ASSERT_EQ(des.size(), cnt);
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(tmp[i], des[i]);
}

// push_front测试
TEST(testCase, deque_push_front_test) {
    std::deque<int> src;
    jr_std::deque<int> des;
    // 拷贝push
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        src.push_front(n);
        des.push_front(n);
    }
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
    // 移动push
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        src.push_front(std::move(n));
        des.push_front(std::move(n));
    }
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
}

// push_back测试
TEST(testCase, deque_push_back_test) {
    std::deque<int> src;
    jr_std::deque<int> des;
    // 拷贝push
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        src.push_back(n);
        des.push_back(n);
    }
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
    // 移动push
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        src.push_back(std::move(n));
        des.push_back(std::move(n));
    }
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
}

// pop_front测试
TEST(testCase, deque_pop_front_test) {
    std::deque<int> src{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    jr_std::deque<int> des{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    src.pop_front();
    des.pop_front();
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);

}

// pop_back测试
TEST(testCase, deque_pop_back_test) {
    std::deque<int> src{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    jr_std::deque<int> des{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    src.pop_back();
    des.pop_back();
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);

}

// 原位构造push_front测试
TEST(testCase, deque_emplace_front_test) {
    std::deque<int> src;
    jr_std::deque<int> des;
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        src.emplace_front(n);
        des.emplace_front(n);
    }
    EXPECT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
}

// 原位构造push_back测试
TEST(testCase, deque_emplace_back_test) {
    std::deque<int> src;
    jr_std::deque<int> des;
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        src.emplace_back(n);
        des.emplace_back(n);
    }
    EXPECT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
}

//// insert测试
TEST(testCase, deque_insert_test) {
    std::deque<int> src;
    jr_std::deque<int> des;
    // 拷贝insert
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        EXPECT_EQ(*(src.insert(src.begin(), n)),
                  *(des.insert(des.cbegin(), n)));
    }
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
    // 移动insert
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        EXPECT_EQ(*(src.insert(src.begin(), std::move(n))),
                  *(des.insert(des.cbegin(), std::move(n))));
    }
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
    // 赋值insert
    EXPECT_EQ(*(src.insert(src.begin(), 10, -901)),
              *(des.insert(des.cbegin(), 10, -901)));
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
    // 初始化列表insert
    EXPECT_EQ(*(src.insert(src.begin(), {1,2,3,4,5})),
              *(des.insert(des.cbegin(), {1,2,3,4,5})));
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
    // 迭代器insert
    jr_std::deque<int> tmp({1,2,3,4,5});
    std::deque<int> tmp0({1,2,3,4,5});
    EXPECT_EQ(*(src.insert(src.begin(), tmp0.begin(), tmp0.end())),
              *(des.insert(des.cbegin(), tmp.begin(), tmp.end())));
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
}

// emplace测试
TEST(testCase, deque_emplace_test) {
    std::deque<int> src;
    jr_std::deque<int> des;
    // 拷贝emplace
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        EXPECT_EQ(*(src.emplace(src.begin(), n)),
                  *(des.emplace(des.cbegin(), n)));
    }
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
    // 移动emplace
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        EXPECT_EQ(*(src.emplace(src.begin(), std::move(n))),
                  *(des.emplace(des.cbegin(), std::move(n))));
    }
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
}

// erase测试
TEST(testCase, deque_erase_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var, 20);
    jr_std::deque<int> des(cnt, var);
    std::deque<int> src(cnt, var);
    // 单位置erase
    EXPECT_EQ(*(src.erase(src.begin() + 2)),
              *(des.erase(des.cbegin() + 2)));
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
    // 范围erase
    EXPECT_EQ(*(src.erase(src.begin() + 2, src.begin() + 6)),
              *(des.erase(des.cbegin() + 2, des.cbegin() + 6)));
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
    // 边界测试
    EXPECT_EQ(*(src.erase(src.begin(), src.begin() + 6)),
              *(des.erase(des.cbegin(), des.cbegin() + 6)));
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
    src.erase(src.begin() + 2, src.end());
    des.erase(des.cbegin() + 2, des.cend());
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
    // 全区间删除
    src.erase(src.begin(), src.end());
    des.erase(des.cbegin(), des.cend());
    EXPECT_EQ(src.size(), des.size());
}

// swap测试
TEST(testCase, deque_swap_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jr_std::deque<int> src(cnt, var - 1);
    jr_std::deque<int> des(cnt, var + 1);
    jr_std::deque<int> tmp = src;
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
TEST(testCase, deque_reverse_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    std::deque<int> src;
    jr_std::deque<int> des;
    for(int i = 0; i < static_cast<int>(cnt); i++) {
        src.push_back(i);
        des.push_back(i);
    }
    auto it = src.rbegin();
    auto rit = des.rbegin();
    while(rit != des.rend()) {
        EXPECT_EQ(*it, *rit);
        ++rit;
        ++it;
    }
}

// 迭代器-=运算
TEST(testCase, deque_iterator_sub_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var, 20);
    std::deque<int> src(cnt, var);
    jr_std::deque<int> des(cnt, var);
    auto it = src.begin();
    auto rit = des.begin();
    it += 12;
    rit += 12;
    it -= 5;
    rit -= 5;
    EXPECT_EQ(*it, *rit);
}
