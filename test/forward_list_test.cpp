#include <gtest/gtest.h>
#include <random>
#include <forward_list>
#include <iostream>
#include "../container/sequence/jr_forward_list.h"

#define MAX_SIZE 2000

void get_random_size_var(size_t max_size,
                         size_t& size,
                         int& var,
                         size_t min_size = 0);

// 赋值构造函数与元素访问测试
TEST(testCase,forward_list_assign_ctor_test){
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    // 赋值构造函数测试
    jrSTL::forward_list<int> des(cnt, var);
    // 迭代器遍历测试
    auto dit = des.begin();
    for(; dit != des.end(); ++dit)
        EXPECT_EQ(var, *dit);
    // 头尾访问测试
    EXPECT_EQ(var, des.front());
}

// 迭代器范围构造函数测试
TEST(testCase,forward_list_iterator_ctor_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jrSTL::forward_list<int> src(cnt, var);
    // 迭代器范围构造函数
    jrSTL::forward_list<int> des(src.begin(), src.end());
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// 拷贝构造函数测试
TEST(testCase,forward_list_copy_ctor_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jrSTL::forward_list<int> src(cnt, var);
    // 拷贝构造函数
    jrSTL::forward_list<int> des(src);
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// 移动构造函数测试
TEST(testCase,forward_list_move_ctor_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jrSTL::forward_list<int> src(cnt, var);
    // 移动构造函数
    jrSTL::forward_list<int> des(std::move(src));
    ASSERT_EQ(src.empty(), true);
}

// 初始化列表构造函数测试
TEST(testCase,forward_list_initializer_list_ctor_test) {
    std::forward_list<int> src{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    jrSTL::forward_list<int> des{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// 拷贝运算符测试
TEST(testCase, forward_list_operator_copy_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jrSTL::forward_list<int> src(cnt, var);
    jrSTL::forward_list<int> des{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    des = src;
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// 移动运算符测试
TEST(testCase, forward_list_operator_move_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jrSTL::forward_list<int> src(cnt, var);
    jrSTL::forward_list<int> des{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    des = std::move(src);
    ASSERT_EQ(src.empty(), true);
    auto dit = des.begin();
    for(; dit != des.end(); ++dit)
        EXPECT_EQ(var, *dit);
}

// 赋值成员函数测试
TEST(testCase, forward_list_assign_mem_fn_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    std::forward_list<int> src(10, -1);
    jrSTL::forward_list<int> des(15, -4);
    // 重复赋值测试
    src.assign(cnt, var);
    des.assign(cnt, var);
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    // 初始化列表赋值测试
    src.assign({32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2});
    des.assign({32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2});
    it = src.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    // 迭代器赋值测试
    jrSTL::forward_list<int> tmp(6, -100);
    des.assign(tmp.begin(), tmp.end());
    auto fit = tmp.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++fit)
        EXPECT_EQ(*fit, *dit);
}

// clear测试
TEST(testCase, forward_list_clear_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jrSTL::forward_list<int> des(cnt, var);
    des.clear();
    EXPECT_EQ(des.empty(), true);
}

// resize测试
TEST(testCase, forward_list_resize_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jrSTL::forward_list<int> des(cnt, var);
    std::forward_list<int> tmp(cnt, var);
    // 新size小于旧size
    des.resize(cnt / 2);
    ASSERT_EQ(jrSTL::distance(des.begin(), des.end()), cnt / 2);
    auto it = tmp.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    // 新size大于旧size
    des.resize(cnt, var);
    ASSERT_EQ(jrSTL::distance(des.begin(), des.end()), cnt);
    it = tmp.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    // 边界测试：新size等于旧size
    des.resize(cnt, var - 1);
    ASSERT_EQ(jrSTL::distance(des.begin(), des.end()), cnt);
    it = tmp.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// push_front测试
TEST(testCase, forward_list_push_front_test) {
    std::forward_list<int> src;
    jrSTL::forward_list<int> des;
    // 拷贝push
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        src.push_front(n);
        des.push_front(n);
    }
    ASSERT_EQ(std::distance(src.begin(), src.end()), jrSTL::distance(des.begin(), des.end()));
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    // 移动push
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        src.push_front(std::move(n));
        des.push_front(std::move(n));
    }
    ASSERT_EQ(std::distance(src.begin(), src.end()), jrSTL::distance(des.begin(), des.end()));
    it = src.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// pop_front测试
TEST(testCase, forward_list_pop_front_test) {
    std::forward_list<int> src{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    jrSTL::forward_list<int> des{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    src.pop_front();
    des.pop_front();
    ASSERT_EQ(std::distance(src.begin(), src.end()), jrSTL::distance(des.begin(), des.end()));
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// 原位构造push_front测试
TEST(testCase, forward_list_emplace_front_test) {
    std::forward_list<int> src;
    jrSTL::forward_list<int> des;
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        src.emplace_front(n);
        des.emplace_front(n);
    }
    EXPECT_EQ(std::distance(src.begin(), src.end()), jrSTL::distance(des.begin(), des.end()));
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// insert测试
TEST(testCase, forward_list_insert_test) {
    std::forward_list<int> src;
    jrSTL::forward_list<int> des;
    // 拷贝insert
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        EXPECT_EQ(*(src.insert_after(src.before_begin(), n)),
                  *(des.insert_after(des.cbefore_begin(), n)));
    }
    ASSERT_EQ(std::distance(src.begin(), src.end()), jrSTL::distance(des.begin(), des.end()));
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    // 移动insert
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        EXPECT_EQ(*(src.insert_after(src.cbefore_begin(), std::move(n))),
                  *(des.insert_after(des.cbefore_begin(), std::move(n))));
    }
    ASSERT_EQ(std::distance(src.begin(), src.end()), jrSTL::distance(des.begin(), des.end()));
    it = src.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    // 赋值insert
    EXPECT_EQ(*(src.insert_after(src.cbefore_begin(), 10, -901)),
              *(des.insert_after(des.cbefore_begin(), 10, -901)));
    ASSERT_EQ(std::distance(src.begin(), src.end()), jrSTL::distance(des.begin(), des.end()));
    it = src.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    // 初始化列表insert
    EXPECT_EQ(*(src.insert_after(src.cbefore_begin(), {1,2,3,4,5})),
              *(des.insert_after(des.cbefore_begin(), {1,2,3,4,5})));
    ASSERT_EQ(std::distance(src.begin(), src.end()), jrSTL::distance(des.begin(), des.end()));
    it = src.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    // 迭代器insert
    jrSTL::forward_list<int> tmp({1,2,3,4,5});
    std::forward_list<int> tmp0({1,2,3,4,5});
    EXPECT_EQ(*(src.insert_after(src.cbefore_begin(), tmp0.begin(), tmp0.end())),
              *(des.insert_after(des.cbefore_begin(), tmp.begin(), tmp.end())));
    ASSERT_EQ(std::distance(src.begin(), src.end()), jrSTL::distance(des.begin(), des.end()));
    it = src.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// emplace测试
TEST(testCase, forward_list_emplace_test) {
    std::forward_list<int> src;
    jrSTL::forward_list<int> des;
    // 拷贝emplace
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        EXPECT_EQ(*(src.emplace_after(src.before_begin(), n)),
                  *(des.emplace_after(des.cbefore_begin(), n)));
    }
    ASSERT_EQ(std::distance(src.begin(), src.end()), jrSTL::distance(des.begin(), des.end()));
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    // 移动emplace
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        EXPECT_EQ(*(src.emplace_after(src.before_begin(), std::move(n))),
                  *(des.emplace_after(des.cbefore_begin(), std::move(n))));
    }
    ASSERT_EQ(std::distance(src.begin(), src.end()), jrSTL::distance(des.begin(), des.end()));
    it = src.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// erase测试
TEST(testCase, forward_list_erase_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var, 20);
    jrSTL::forward_list<int> des(cnt, var);
    std::forward_list<int> src(cnt, var);
    // 单位置erase
    auto i = src.begin();
    auto j = des.cbegin();
    std::advance(i, 2);
    jrSTL::advance(j, 2);
    EXPECT_EQ(*(src.erase_after(i)),
              *(des.erase_after(j)));
    ASSERT_EQ(std::distance(src.begin(), src.end()), jrSTL::distance(des.begin(), des.end()));
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    // 范围erase
    i = src.begin();
    j = des.cbegin();
    std::advance(i, 2);
    jrSTL::advance(j, 2);
    auto i0 = src.begin();
    auto j0 = des.cbegin();
    std::advance(i0, 6);
    jrSTL::advance(j0, 6);
    EXPECT_EQ(*(src.erase_after(i, i0)),
              *(des.erase_after(j, j0)));
    ASSERT_EQ(std::distance(src.begin(), src.end()), jrSTL::distance(des.begin(), des.end()));
    it = src.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    // 边界测试
    i0 = src.begin();
    j0 = des.cbegin();
    std::advance(i0, 6);
    jrSTL::advance(j0, 6);
    EXPECT_EQ(*(src.erase_after(src.before_begin(), i0)),
              *(des.erase_after(des.cbefore_begin(), j0)));
    ASSERT_EQ(std::distance(src.begin(), src.end()), jrSTL::distance(des.begin(), des.end()));
    it = src.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    i = src.begin();
    j = des.cbegin();
    std::advance(i, 2);
    jrSTL::advance(j, 2);
    src.erase_after(i, src.end());
    des.erase_after(j, des.cend());
    ASSERT_EQ(std::distance(src.begin(), src.end()), jrSTL::distance(des.begin(), des.end()));
    it = src.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    // 全区间删除
    src.erase_after(src.begin(), src.end());
    des.erase_after(des.cbegin(), des.cend());
    EXPECT_EQ(std::distance(src.begin(), src.end()), jrSTL::distance(des.begin(), des.end()));
}

// swap测试
TEST(testCase, forward_list_swap_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jrSTL::forward_list<int> src(cnt, var - 1);
    jrSTL::forward_list<int> des(cnt, var + 1);
    jrSTL::forward_list<int> tmp = src;
    src.swap(des);
    ASSERT_EQ(jrSTL::distance(src.begin(), src.end()), jrSTL::distance(des.begin(), des.end()));
    auto it = tmp.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    des.swap(src);
    ASSERT_EQ(jrSTL::distance(src.begin(), src.end()), jrSTL::distance(des.begin(), des.end()));
    it = src.begin();
    dit = tmp.begin();
    for(; dit != tmp.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// sort测试
TEST(testCase, forward_list_sort_test) {
    std::forward_list<int> src{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    jrSTL::forward_list<int> des{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    src.sort();
    des.sort();
    ASSERT_EQ(std::distance(src.begin(), src.end()), jrSTL::distance(des.begin(), des.end()));
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// merge测试
TEST(testCase, forward_list_merge_test) {
    std::forward_list<int> src0{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    std::forward_list<int> src1{33,53421,4247,20,234,25,48,20,2350,18,2423,601,474,67,5589,2089};
    jrSTL::forward_list<int> des0{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    jrSTL::forward_list<int> des1{33,53421,4247,20,234,25,48,20,2350,18,2423,601,474,67,5589,2089};
    // 同长度
    src0.sort();
    src1.sort();
    des0.sort();
    des1.sort();
    src0.merge(src1);
    des0.merge(des1);
    ASSERT_EQ(std::distance(src0.before_begin(), src0.end()), jrSTL::distance(des0.before_begin(), des0.end()));
    ASSERT_EQ(std::distance(src1.before_begin(), src1.end()), jrSTL::distance(des1.before_begin(), des1.end()));
    auto it0 = src0.begin();
    auto dit0 = des0.begin();
    for(; dit0 != des0.end(); ++dit0, ++it0)
        EXPECT_EQ(*it0, *dit0);
    // 左list长度<右list长度
    src1.insert_after(src1.before_begin(), {86, 98, 342, 24, 2423, 8, 12});
    des1.insert_after(des1.cbefore_begin(), {86, 98, 342, 24, 2423, 8, 12});
    src0.sort();
    src1.sort();
    des0.sort();
    des1.sort();
    src0.merge(src1);
    des0.merge(des1);
    ASSERT_EQ(std::distance(src0.before_begin(), src0.end()), jrSTL::distance(des0.before_begin(), des0.end()));
    ASSERT_EQ(std::distance(src1.before_begin(), src1.end()), jrSTL::distance(des1.before_begin(), des1.end()));
    it0 = src0.begin();
    dit0 = des0.begin();
    for(; dit0 != des0.end(); ++dit0, ++it0)
        EXPECT_EQ(*it0, *dit0);
    // 左list长度>右list长度
    src0.insert_after(src0.before_begin(), {86, 98, 342, 24, 2423, 8, 12, 456, 2352, 65, 21312, 636,678, 112});
    des0.insert_after(des0.cbefore_begin(), {86, 98, 342, 24, 2423, 8, 12, 456, 2352, 65, 21312, 636,678, 112});
    src0.sort();
    src1.sort();
    des0.sort();
    des1.sort();
    src0.merge(src1);
    des0.merge(des1);
    ASSERT_EQ(std::distance(src0.before_begin(), src0.end()), jrSTL::distance(des0.before_begin(), des0.end()));
    ASSERT_EQ(std::distance(src1.before_begin(), src1.end()), jrSTL::distance(des1.before_begin(), des1.end()));
    it0 = src0.begin();
    dit0 = des0.begin();
    for(; dit0 != des0.end(); ++dit0, ++it0)
        EXPECT_EQ(*it0, *dit0);
}

// splice测试
TEST(testCase, forward_list_splice_test) {
    std::forward_list<int> src0{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    std::forward_list<int> src1{33,53421,4247,20,234,25,48,20,2350,18,2423,601,474,67,5589,2089};
    jrSTL::forward_list<int> des0{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    jrSTL::forward_list<int> des1{33,53421,4247,20,234,25,48,20,2350,18,2423,601,474,67,5589,2089};
    // begin
    src0.splice_after(src0.begin(), src1);
    des0.splice_after(des0.cbegin(), des1);
    ASSERT_EQ(std::distance(src0.begin(), src0.end()), jrSTL::distance(des0.begin(), des0.end()));
    auto it0 = src0.begin();
    auto dit0 = des0.begin();
    for(; dit0 != des0.end(); ++dit0, ++it0)
        EXPECT_EQ(*it0, *dit0);
    // 中间位置
    std::forward_list<int> src3{33,53421,4247,20,234,25,48};
    jrSTL::forward_list<int> des3{33,53421,4247,20,234,25,48};
    auto i0 = src0.begin();
    auto j0 = des0.cbegin();
    for(int i = 0; i < 4; i++) {
        ++i0;
        ++j0;
    }
    src0.splice_after(i0, src3);
    des0.splice_after(j0, des3);
    ASSERT_EQ(std::distance(src0.begin(), src0.end()), jrSTL::distance(des0.begin(), des0.end()));
    it0 = src0.begin();
    dit0 = des0.begin();
    for(; dit0 != des0.end(); ++dit0, ++it0)
        EXPECT_EQ(*it0, *dit0);
}

// unique测试
TEST(testCase, forward_list_unique_test) {
    // 普通情况
    std::forward_list<int> src0{32,53423,4245,25,234,25,45,2,4245,25,234,25,45,2,4245,25,234,25,45,2,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    jrSTL::forward_list<int> des0{32,53423,4245,25,234,25,45,2,4245,25,234,25,45,2,4245,25,234,25,45,2,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    src0.unique();
    des0.unique();
    ASSERT_EQ(std::distance(src0.begin(), src0.end()), jrSTL::distance(des0.begin(), des0.end()));
    auto it0 = src0.begin();
    auto dit0 = des0.begin();
    for(; dit0 != des0.end(); ++dit0, ++it0)
        EXPECT_EQ(*it0, *dit0);
    // 极端情况
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var, 20);
    std::forward_list<int> src(cnt, var);
    jrSTL::forward_list<int> des(cnt, var);
    src.unique();
    des.unique();
    EXPECT_EQ(std::distance(src.begin(), src.end()), jrSTL::distance(des.begin(), des.end()));
}

// remove测试
TEST(testCase, forward_list_reove_test) {
    std::forward_list<int> src0{32,53423,4245,25,234,25,45,2,4245,25,234,25,45,2,4245,25,234,25,45,2,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    jrSTL::forward_list<int> des0{32,53423,4245,25,234,25,45,2,4245,25,234,25,45,2,4245,25,234,25,45,2,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    src0.remove_if([](int n){ return n % 2; });
    des0.remove_if([](int n){ return n % 2; });
    ASSERT_EQ(std::distance(src0.begin(), src0.end()), jrSTL::distance(des0.begin(), des0.end()));
    auto it0 = src0.begin();
    auto dit0 = des0.begin();
    for(; dit0 != des0.end(); ++dit0, ++it0)
        EXPECT_EQ(*it0, *dit0);
}

// 反转测试
TEST(testCase, forward_list_reverse_test) {
    std::forward_list<int> src0{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    jrSTL::forward_list<int> des0{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    src0.reverse();
    des0.reverse();
    ASSERT_EQ(std::distance(src0.begin(), src0.end()), jrSTL::distance(des0.begin(), des0.end()));
    auto it0 = src0.begin();
    auto dit0 = des0.begin();
    for(; dit0 != des0.end(); ++dit0, ++it0)
        EXPECT_EQ(*it0, *dit0);
}
