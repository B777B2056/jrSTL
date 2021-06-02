#include <gtest/gtest.h>
#include <random>
#include <list>
#include "../container/sequence/jr_list.h"

#define MAX_SIZE 2000

void get_random_size_var(size_t max_size,
                         size_t& size,
                         int& var,
                         size_t min_size = 0);

// 赋值构造函数与元素访问测试
TEST(testCase,list_assign_ctor_test){
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    // 赋值构造函数测试
    jr_std::list<int> des(cnt, var);
    ASSERT_EQ(cnt, des.size());
    // 迭代器遍历测试
    auto dit = des.begin();
    for(; dit != des.end(); ++dit)
        EXPECT_EQ(var, *dit);
    // 头尾访问测试
    EXPECT_EQ(var, des.front());
    EXPECT_EQ(var, des.back());
}

// 迭代器范围构造函数测试
TEST(testCase,list_iterator_ctor_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jr_std::list<int> src(cnt, var);
    // 迭代器范围构造函数
    jr_std::list<int> des(src.begin(), src.end());
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// 拷贝构造函数测试
TEST(testCase,list_copy_ctor_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jr_std::list<int> src(cnt, var);
    // 拷贝构造函数
    jr_std::list<int> des(src);
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// 移动构造函数测试
TEST(testCase,list_move_ctor_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jr_std::list<int> src(cnt, var);
    // 移动构造函数
    jr_std::list<int> des(std::move(src));
    ASSERT_EQ(src.empty(), true);
}

// 初始化列表构造函数测试
TEST(testCase,list_initializer_list_ctor_test) {
    std::list<int> src{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    jr_std::list<int> des{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// 拷贝运算符测试
TEST(testCase, list_operator_copy_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jr_std::list<int> src(cnt, var);
    jr_std::list<int> des{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    des = src;
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// 移动运算符测试
TEST(testCase, list_operator_move_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jr_std::list<int> src(cnt, var);
    jr_std::list<int> des{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    des = std::move(src);
    ASSERT_EQ(src.empty(), true);
    ASSERT_EQ(cnt, des.size());
    auto dit = des.begin();
    for(; dit != des.end(); ++dit)
        EXPECT_EQ(var, *dit);
}

// 赋值成员函数测试
TEST(testCase, list_assign_mem_fn_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    std::list<int> src(10, -1);
    jr_std::list<int> des(15, -4);
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
    ASSERT_EQ(src.size(), des.size());
    it = src.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    // 迭代器赋值测试
    jr_std::list<int> tmp(6, -100);
    des.assign(tmp.begin(), tmp.end());
    ASSERT_EQ(tmp.size(), des.size());
    auto fit = tmp.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++fit)
        EXPECT_EQ(*fit, *dit);
}

// clear测试
TEST(testCase, list_clear_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jr_std::list<int> des(cnt, var);
    des.clear();
    EXPECT_EQ(des.size(), 0);
}

// resize测试
TEST(testCase, list_resize_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jr_std::list<int> des(cnt, var);
    std::list<int> tmp(cnt, var);
    // 新size小于旧size
    des.resize(cnt / 2);
    ASSERT_EQ(des.size(), cnt / 2);
    auto it = tmp.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    // 新size大于旧size
    des.resize(cnt, var);
    ASSERT_EQ(des.size(), cnt);
    it = tmp.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    // 边界测试：新size等于旧size
    des.resize(cnt, var - 1);
    ASSERT_EQ(des.size(), cnt);
    it = tmp.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// push_front测试
TEST(testCase, list_push_front_test) {
    std::list<int> src;
    jr_std::list<int> des;
    // 拷贝push
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        src.push_front(n);
        des.push_front(n);
    }
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    // 移动push
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        src.push_front(std::move(n));
        des.push_front(std::move(n));
    }
    ASSERT_EQ(src.size(), des.size());
    it = src.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// push_back测试
TEST(testCase, list_push_back_test) {
    std::list<int> src;
    jr_std::list<int> des;
    // 拷贝push
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        src.push_back(n);
        des.push_back(n);
    }
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    // 移动push
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        src.push_back(std::move(n));
        des.push_back(std::move(n));
    }
    ASSERT_EQ(src.size(), des.size());
    it = src.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// pop_front测试
TEST(testCase, list_pop_front_test) {
    std::list<int> src{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    jr_std::list<int> des{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    src.pop_front();
    des.pop_front();
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// pop_back测试
TEST(testCase, list_pop_back_test) {
    std::list<int> src{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    jr_std::list<int> des{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    src.pop_back();
    des.pop_back();
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// 原位构造push_front测试
TEST(testCase, list_emplace_front_test) {
    std::list<int> src;
    jr_std::list<int> des;
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        src.emplace_front(n);
        des.emplace_front(n);
    }
    EXPECT_EQ(src.size(), des.size());
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// 原位构造push_back测试
TEST(testCase, list_emplace_back_test) {
    std::list<int> src;
    jr_std::list<int> des;
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        src.emplace_back(n);
        des.emplace_back(n);
    }
    EXPECT_EQ(src.size(), des.size());
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// insert测试
TEST(testCase, list_insert_test) {
    std::list<int> src;
    jr_std::list<int> des;
    // 拷贝insert
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        EXPECT_EQ(*(src.insert(src.begin(), n)),
                  *(des.insert(des.cbegin(), n)));
    }
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    // 移动insert
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        EXPECT_EQ(*(src.insert(src.begin(), std::move(n))),
                  *(des.insert(des.cbegin(), std::move(n))));
    }
    ASSERT_EQ(src.size(), des.size());
    it = src.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    // 赋值insert
    EXPECT_EQ(*(src.insert(src.begin(), 10, -901)),
              *(des.insert(des.cbegin(), 10, -901)));
    ASSERT_EQ(src.size(), des.size());
    it = src.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    // 初始化列表insert
    EXPECT_EQ(*(src.insert(src.begin(), {1,2,3,4,5})),
              *(des.insert(des.cbegin(), {1,2,3,4,5})));
    ASSERT_EQ(src.size(), des.size());
    it = src.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    // 迭代器insert
    jr_std::list<int> tmp({1,2,3,4,5});
    std::list<int> tmp0({1,2,3,4,5});
    EXPECT_EQ(*(src.insert(src.begin(), tmp0.begin(), tmp0.end())),
              *(des.insert(des.cbegin(), tmp.begin(), tmp.end())));
    ASSERT_EQ(src.size(), des.size());
    it = src.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// emplace测试
TEST(testCase, list_emplace_test) {
    std::list<int> src;
    jr_std::list<int> des;
    // 拷贝emplace
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        EXPECT_EQ(*(src.emplace(src.begin(), n)),
                  *(des.emplace(des.cbegin(), n)));
    }
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    // 移动emplace
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        EXPECT_EQ(*(src.emplace(src.begin(), std::move(n))),
                  *(des.emplace(des.cbegin(), std::move(n))));
    }
    ASSERT_EQ(src.size(), des.size());
    it = src.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// erase测试
TEST(testCase, list_erase_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var, 20);
    jr_std::list<int> des(cnt, var);
    std::list<int> src(cnt, var);
    // 单位置erase
    auto i = src.begin();
    auto j = des.cbegin();
    std::advance(i, 2);
    jr_std::advance(j, 2);
    EXPECT_EQ(*(src.erase(i)),
              *(des.erase(j)));
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    // 范围erase
    i = src.begin();
    j = des.cbegin();
    std::advance(i, 2);
    jr_std::advance(j, 2);
    auto i0 = src.begin();
    auto j0 = des.cbegin();
    std::advance(i0, 6);
    jr_std::advance(j0, 6);
    EXPECT_EQ(*(src.erase(i, i0)),
              *(des.erase(j, j0)));
    ASSERT_EQ(src.size(), des.size());
    it = src.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    // 边界测试
    i0 = src.begin();
    j0 = des.cbegin();
    std::advance(i0, 6);
    jr_std::advance(j0, 6);
    EXPECT_EQ(*(src.erase(src.begin(), i0)),
              *(des.erase(des.cbegin(), j0)));
    ASSERT_EQ(src.size(), des.size());
    it = src.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    i = src.begin();
    j = des.cbegin();
    std::advance(i, 2);
    jr_std::advance(j, 2);
    src.erase(i, src.end());
    des.erase(j, des.cend());
    ASSERT_EQ(src.size(), des.size());
    it = src.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    // 全区间删除
    src.erase(src.begin(), src.end());
    des.erase(des.cbegin(), des.cend());
    EXPECT_EQ(src.size(), des.size());
}

// swap测试
TEST(testCase, list_swap_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jr_std::list<int> src(cnt, var - 1);
    jr_std::list<int> des(cnt, var + 1);
    jr_std::list<int> tmp = src;
    src.swap(des);
    ASSERT_EQ(tmp.size(), des.size());
    auto it = tmp.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    des.swap(src);
    ASSERT_EQ(tmp.size(), src.size());
    it = src.begin();
    dit = tmp.begin();
    for(; dit != tmp.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// 逆序访问测试
TEST(testCase, list_reverse_index_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    std::list<int> src;
    jr_std::list<int> des;
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
TEST(testCase, list_iterator_sub_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var, 20);
    std::list<int> src(cnt, var);
    jr_std::list<int> des(cnt, var);
    auto it = src.begin();
    auto rit = des.begin();
    std::advance(it, 12);
    jr_std::advance(rit, 12);
    std::advance(it, -5);
    jr_std::advance(rit, -5);
    EXPECT_EQ(*it, *rit);
}

// sort测试
TEST(testCase, list_sort_test) {
    std::list<int> src{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    jr_std::list<int> des{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    src.sort();
    des.sort();
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// merge测试
TEST(testCase, list_merge_test) {
    std::list<int> src0{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    std::list<int> src1{33,53421,4247,20,234,25,48,20,2350,18,2423,601,474,67,5589,2089};
    jr_std::list<int> des0{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    jr_std::list<int> des1{33,53421,4247,20,234,25,48,20,2350,18,2423,601,474,67,5589,2089};
    // 同长度
    src0.sort();
    src1.sort();
    des0.sort();
    des1.sort();
    src0.merge(src1);
    des0.merge(des1);
    ASSERT_EQ(src0.size(), des0.size());
    ASSERT_EQ(src1.size(), des1.size());
    auto it0 = src0.begin();
    auto dit0 = des0.begin();
    for(; dit0 != des0.end(); ++dit0, ++it0)
        EXPECT_EQ(*it0, *dit0);
    // 左list长度<右list长度
    src1.insert(src1.end(), {86, 98, 342, 24, 2423, 8, 12});
    des1.insert(des1.cend(), {86, 98, 342, 24, 2423, 8, 12});
    src0.sort();
    src1.sort();
    des0.sort();
    des1.sort();
    src0.merge(src1);
    des0.merge(des1);
    ASSERT_EQ(src0.size(), des0.size());
    ASSERT_EQ(src1.size(), des1.size());
    it0 = src0.begin();
    dit0 = des0.begin();
    for(; dit0 != des0.end(); ++dit0, ++it0)
        EXPECT_EQ(*it0, *dit0);
    // 左list长度>右list长度
    src0.insert(src0.end(), {86, 98, 342, 24, 2423, 8, 12, 456, 2352, 65, 21312, 636,678, 112});
    des0.insert(des0.cend(), {86, 98, 342, 24, 2423, 8, 12, 456, 2352, 65, 21312, 636,678, 112});
    src0.sort();
    src1.sort();
    des0.sort();
    des1.sort();
    src0.merge(src1);
    des0.merge(des1);
    ASSERT_EQ(src0.size(), des0.size());
    ASSERT_EQ(src1.size(), des1.size());
    it0 = src0.begin();
    dit0 = des0.begin();
    for(; dit0 != des0.end(); ++dit0, ++it0)
        EXPECT_EQ(*it0, *dit0);
}

// splice测试
TEST(testCase, list_splice_test) {
    std::list<int> src0{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    std::list<int> src1{33,53421,4247,20,234,25,48,20,2350,18,2423,601,474,67,5589,2089};
    jr_std::list<int> des0{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    jr_std::list<int> des1{33,53421,4247,20,234,25,48,20,2350,18,2423,601,474,67,5589,2089};
    // begin
    src0.splice(src0.begin(), src1);
    des0.splice(des0.cbegin(), des1);
    ASSERT_EQ(src0.size(), des0.size());
    auto it0 = src0.begin();
    auto dit0 = des0.begin();
    for(; dit0 != des0.end(); ++dit0, ++it0)
        EXPECT_EQ(*it0, *dit0);
    // end
    std::list<int> src2{33,53421,4247,20,234,25,48};
    jr_std::list<int> des2{33,53421,4247,20,234,25,48};
    src0.splice(src0.end(), src2);
    des0.splice(des0.cend(), des2);
    ASSERT_EQ(src0.size(), des0.size());
    it0 = src0.begin();
    dit0 = des0.begin();
    for(; dit0 != des0.end(); ++dit0, ++it0)
        EXPECT_EQ(*it0, *dit0);
    // 中间位置
    std::list<int> src3{33,53421,4247,20,234,25,48};
    jr_std::list<int> des3{33,53421,4247,20,234,25,48};
    auto i0 = src0.begin();
    auto j0 = des0.cbegin();
    for(int i = 0; i < 4; i++) {
        ++i0;
        ++j0;
    }
    src0.splice(i0, src3);
    des0.splice(j0, des3);
    ASSERT_EQ(src0.size(), des0.size());
    it0 = src0.begin();
    dit0 = des0.begin();
    for(; dit0 != des0.end(); ++dit0, ++it0)
        EXPECT_EQ(*it0, *dit0);
}

// unique测试
TEST(testCase, list_unique_test) {
    // 普通情况
    std::list<int> src0{32,53423,4245,25,234,25,45,2,4245,25,234,25,45,2,4245,25,234,25,45,2,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    jr_std::list<int> des0{32,53423,4245,25,234,25,45,2,4245,25,234,25,45,2,4245,25,234,25,45,2,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    src0.unique();
    des0.unique();
    ASSERT_EQ(src0.size(), des0.size());
    auto it0 = src0.begin();
    auto dit0 = des0.begin();
    for(; dit0 != des0.end(); ++dit0, ++it0)
        EXPECT_EQ(*it0, *dit0);
    // 极端情况
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var, 20);
    std::list<int> src(cnt, var);
    jr_std::list<int> des(cnt, var);
    src.unique();
    des.unique();
    EXPECT_EQ(src.size(), des.size());
}

// remove测试
TEST(testCase, list_reove_test) {
    std::list<int> src0{32,53423,4245,25,234,25,45,2,4245,25,234,25,45,2,4245,25,234,25,45,2,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    jr_std::list<int> des0{32,53423,4245,25,234,25,45,2,4245,25,234,25,45,2,4245,25,234,25,45,2,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    src0.remove_if([](int n){ return n % 2; });
    des0.remove_if([](int n){ return n % 2; });
    ASSERT_EQ(src0.size(), des0.size());
    auto it0 = src0.begin();
    auto dit0 = des0.begin();
    for(; dit0 != des0.end(); ++dit0, ++it0)
        EXPECT_EQ(*it0, *dit0);
}

// 反转测试
TEST(testCase, list_reverse_test) {
    std::list<int> src0{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    jr_std::list<int> des0{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    src0.reverse();
    des0.reverse();
    ASSERT_EQ(src0.size(), des0.size());
    auto it0 = src0.begin();
    auto dit0 = des0.begin();
    for(; dit0 != des0.end(); ++dit0, ++it0)
        EXPECT_EQ(*it0, *dit0);
}
