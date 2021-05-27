#include <gtest/gtest.h>
#include <random>
#include <vector>
#include "../container/sequence/jr_vector.h"

#define MAX_SIZE 2000

void get_random_size_var(size_t max_size, size_t& size, int& var, size_t min_size = 0) {
    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<unsigned int> cnt_dist(min_size, max_size);
    std::uniform_int_distribution<int> var_dist(INT_MIN, INT_MAX);
    size = cnt_dist(e1);
    var = var_dist(e1);
}

// 赋值构造函数与元素访问测试
TEST(testCase,assign_ctor_test){
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    // 赋值构造函数测试
    std::vector<int> src(cnt, var);
    jr_std::vector<int> des(cnt, var);
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

// 迭代器范围构造函数测试
TEST(testCase,iterator_ctor_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jr_std::vector<int> src(cnt, var);
    // 迭代器范围构造函数
    jr_std::vector<int> des(src.begin(), src.end());
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < src.size(); i++)
        EXPECT_EQ(src[i], des[i]);
}

// 拷贝构造函数测试
TEST(testCase,copy_ctor_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jr_std::vector<int> src(cnt, var);
    // 拷贝构造函数
    jr_std::vector<int> des(src);
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < src.size(); i++)
        EXPECT_EQ(src[i], des[i]);
}

// 移动构造函数测试
TEST(testCase,move_ctor_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jr_std::vector<int> src(cnt, var);
    // 移动构造函数
    jr_std::vector<int> des(std::move(src));
    ASSERT_EQ(src.empty(), true);
}

// 初始化列表构造函数测试
TEST(testCase,initializer_list_ctor_test) {
    std::vector<int> src{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    // 迭代器范围构造函数
    jr_std::vector<int> des{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < src.size(); i++)
        EXPECT_EQ(src[i], des[i]);
}

// 拷贝运算符测试
TEST(testCase, operator_copy_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jr_std::vector<int> src(cnt, var);
    jr_std::vector<int> des{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    des = src;
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < src.size(); i++)
        EXPECT_EQ(src[i], des[i]);
}

// 移动运算符测试
TEST(testCase, operator_move_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jr_std::vector<int> src(cnt, var);
    jr_std::vector<int> des{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    des = std::move(src);
    EXPECT_EQ(src.empty(), true);
    ASSERT_EQ(cnt, des.size());
    for(size_t i = 0; i < des.size(); i++)
        EXPECT_EQ(var, des[i]);
}

// 赋值成员函数测试
TEST(testCase, assign_mem_fn_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    std::vector<int> src(10, -1);
    jr_std::vector<int> des(15, -4);
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
    jr_std::vector<int> tmp(6, -100);
    des.assign(tmp.begin(), tmp.end());
    ASSERT_EQ(tmp.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(tmp[i], des[i]);
}

// clear测试
TEST(testCase, clear_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jr_std::vector<int> des(cnt, var);
    des.clear();
    EXPECT_EQ(des.size(), 0);
}

// resize测试
TEST(testCase, resize_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jr_std::vector<int> des(cnt, var);
    std::vector<int> tmp(cnt, var);
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

// push_back测试
TEST(testCase, push_back_test) {
    std::vector<int> src;
    jr_std::vector<int> des;
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

// pop_back测试
TEST(testCase, pop_back_test) {
    std::vector<int> src{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    jr_std::vector<int> des{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    src.pop_back();
    des.pop_back();
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);

}

// 原位构造push测试
TEST(testCase, emplace_back_test) {
    std::vector<int> src;
    jr_std::vector<int> des;
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        src.emplace_back(n);
        des.emplace_back(n);
    }
    EXPECT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
}

// insert测试
TEST(testCase, insert_test) {
    std::vector<int> src;
    jr_std::vector<int> des;
    // 拷贝insert
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        EXPECT_EQ(*(src.insert(src.begin(), n)),
                  *(des.insert(des.begin(), n)));
    }
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
    // 移动insert
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        EXPECT_EQ(*(src.insert(src.begin(), std::move(n))),
                  *(des.insert(des.begin(), std::move(n))));
    }
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
    // 赋值insert
    EXPECT_EQ(*(src.insert(src.begin(), 10, -901)),
              *(des.insert(des.begin(), 10, -901)));
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
    // 初始化列表insert
    EXPECT_EQ(*(src.insert(src.begin(), {1,2,3,4,5})),
              *(des.insert(des.begin(), {1,2,3,4,5})));
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
    // 迭代器insert
    std::vector<int> tmp({1,2,3,4,5});
    EXPECT_EQ(*(src.insert(src.begin(), tmp.begin(), tmp.end())),
              *(des.insert(des.begin(), tmp.begin(), tmp.end())));
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
}

// emplace测试
TEST(testCase, emplace_test) {
    std::vector<int> src;
    jr_std::vector<int> des;
    // 拷贝emplace
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        EXPECT_EQ(*(src.emplace(src.begin(), n)),
                  *(des.emplace(des.begin(), n)));
    }
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
    // 移动emplace
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        EXPECT_EQ(*(src.emplace(src.begin(), std::move(n))),
                  *(des.emplace(des.begin(), std::move(n))));
    }
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
}

// erase测试
TEST(testCase, erase_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var, 20);
    jr_std::vector<int> des(cnt, var);
    std::vector<int> src(cnt, var);
    // 单位置erase
    EXPECT_EQ(*(src.erase(src.begin() + 2)),
              *(des.erase(des.begin() + 2)));
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
    // 范围erase
    EXPECT_EQ(*(src.erase(src.begin() + 2, src.begin() + 6)),
              *(des.erase(des.begin() + 2, des.begin() + 6)));
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
    // 边界测试
    EXPECT_EQ(*(src.erase(src.begin(), src.begin() + 6)),
              *(des.erase(des.begin(), des.begin() + 6)));
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
    src.erase(src.begin() + 2, src.end());
    des.erase(des.begin() + 2, des.end());
    ASSERT_EQ(src.size(), des.size());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(src[i], des[i]);
    // 全区间删除
    src.erase(src.begin(), src.end());
    des.erase(des.begin(), des.end());
    EXPECT_EQ(src.size(), des.size());
}

// swap测试
TEST(testCase, swap_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jr_std::vector<int> src(cnt, var - 1);
    jr_std::vector<int> des(cnt, var + 1);
    jr_std::vector<int> tmp = src;
    src.swap(des);
    ASSERT_EQ(tmp.size(), des.size());
    EXPECT_EQ(tmp.capacity(), des.capacity());
    for(size_t i = 0; i < des.size(); ++i)
        EXPECT_EQ(tmp[i], des[i]);
    des.swap(src);
    ASSERT_EQ(tmp.size(), src.size());
    EXPECT_EQ(tmp.capacity(), src.capacity());
    for(size_t i = 0; i < src.size(); ++i)
        EXPECT_EQ(tmp[i], src[i]);
}

// reserve与shrink_to_fit测试
TEST(testCase, shrink_to_fit_test) {
    jr_std::vector<int> des(5, 1);
    des.reserve(100);
    EXPECT_EQ(des.capacity(), 100);
    des.shrink_to_fit();
    EXPECT_EQ(des.capacity(), 5);
}

// 逆序访问测试
TEST(testCase, reverse_test) {
    jr_std::vector<int> des{1,2,3,4,5,6,7,8,9};
    jr_std::vector<int> tmp{9,8,7,6,5,4,3,2,1};
    auto it = tmp.begin();
    auto rit = des.rbegin();
    while(rit != des.rend()) {
        EXPECT_EQ(*it, *rit);
        ++rit;
        ++it;
    }
}
