#include <gtest/gtest.h>
#include <stack>
#include "../container/adapter/jr_stack.h"

#define MAX_SIZE 2000

void get_random_size_var(size_t max_size,
                         size_t& size,
                         int& var,
                         size_t min_size = 0);

// 默认构造函数与push、pop、emplace、top测试
TEST(testCase,stack_mem_fn_test){
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    std::stack<int> src;
    jrSTL::stack<int> des;
    for(size_t i = 0 ; i < cnt; i++) {
        src.push(i);
        des.push(i);
    }
    src.emplace(-108);
    des.emplace(-108);
    ASSERT_EQ(src.size(), des.size());
    while(!des.empty()) {
        EXPECT_EQ(src.top(), des.top());
        src.pop();
        des.pop();
    }
}

// 拷贝构造函数测试
TEST(testCase,stack_copy_ctor_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jrSTL::stack<int> src;
    for(size_t i = 0 ; i < cnt; i++) {
        src.push(i);
    }
    jrSTL::stack<int> des(src);
    ASSERT_EQ(src.size(), des.size());
    while(!des.empty()) {
        EXPECT_EQ(src.top(), des.top());
        src.pop();
        des.pop();
    }
}

// 移动构造函数测试
TEST(testCase,stack_move_ctor_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jrSTL::stack<int> src;
    for(size_t i = 0 ; i < cnt; i++) {
        src.push(var);
    }
    jrSTL::stack<int> des(std::move(src));
    ASSERT_EQ(src.empty(), true);
    while(!des.empty()) {
        EXPECT_EQ(var, des.top());
        des.pop();
    }
}

// 拷贝构造函数测试
TEST(testCase,stack_copy_operator_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jrSTL::stack<int> src;
    jrSTL::stack<int> des;
    for(size_t i = 0 ; i < cnt; i++) {
        src.push(i + 1);
        des.push(i - 1);
    }
    des = src;
    ASSERT_EQ(src.size(), des.size());
    while(!des.empty()) {
        EXPECT_EQ(src.top(), des.top());
        src.pop();
        des.pop();
    }
}

// 拷贝构造函数测试
TEST(testCase,stack_move_operator_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jrSTL::stack<int> src;
    jrSTL::stack<int> des;
    for(size_t i = 0 ; i < cnt; i++) {
        src.push(var - 1);
        des.push(var + 1);
    }
    des = std::move(src);
    ASSERT_EQ(src.size(), 0);
    while(!des.empty()) {
        EXPECT_EQ(var - 1, des.top());
        des.pop();
    }
}

// swap测试
TEST(testCase, stack_swap_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jrSTL::stack<int> src;
    jrSTL::stack<int> des;
    for(size_t i = 0 ; i < cnt; i++) {
        src.push(i + 1);
        des.push(i - 1);
    }
    jrSTL::stack<int> tmp = src, tmp0 = des;
    src.swap(des);
    ASSERT_EQ(tmp.size(), des.size());
    while(!des.empty()) {
        EXPECT_EQ(tmp.top(), des.top());
        tmp.pop();
        des.pop();
    }
    ASSERT_EQ(src.size(), tmp0.size());
    while(!tmp0.empty()) {
        EXPECT_EQ(src.top(), tmp0.top());
        src.pop();
        tmp0.pop();
    }
}
