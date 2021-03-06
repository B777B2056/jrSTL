#include <gtest/gtest.h>
#include <unordered_set>
#include "../container/associate/jr_set.h"
#include "../container/associate/jr_unordered_set.h"

#define MAX_SIZE 2000

void get_random_size_var(size_t max_size,
                         size_t& size,
                         int& var,
                         size_t min_size = 0);

struct int_hash{
    size_t operator()(int n) const {
        if(n < 0)
            n = -n;
        return n % 111;
    }
};

// 迭代器范围构造函数测试
TEST(testCase,unordered_multiset_iterator_ctor_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var, 1);
    jrSTL::set<int> src;
    for(size_t i = 0; i < cnt; i++) {
        int t = var;
        src.insert(t);
        --var;
    }
    // 迭代器范围构造函数
    jrSTL::unordered_multiset<int, int_hash> des(src.begin(), src.end());
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    for(; it != src.end(); ++it) {
        bool a = false;
        for(auto dit = des.begin(); dit != des.end(); ++dit) {
            if(*dit == *it) {
                a = true;
                break;
            }
        }
        EXPECT_EQ(a, true);
    }
}

// 拷贝构造函数测试
TEST(testCase,unordered_multiset_copy_ctor_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jrSTL::unordered_multiset<int, int_hash> src;
    for(size_t i = 0; i < cnt; i++){
        int t = var;
        src.insert(t);
        src.insert(t);
        --var;
    }
    // 拷贝构造函数
    jrSTL::unordered_multiset<int, int_hash> des(src);
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// 移动构造函数测试
TEST(testCase,unordered_multiset_move_ctor_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jrSTL::unordered_multiset<int, int_hash> src;
    for(size_t i = 0; i < cnt; i++){
        int t = var;
        src.insert(t);
        src.insert(t);
        --var;
    }
    // 移动构造函数
    jrSTL::unordered_multiset<int, int_hash> des(std::move(src));
    ASSERT_EQ(src.empty(), true);
}

// 初始化列表构造函数测试
TEST(testCase,unordered_multiset_initializer_list_ctor_test) {
    std::unordered_multiset<int> src{2,2,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    jrSTL::unordered_multiset<int, int_hash> des{2,2,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    for(; it != src.end(); ++it) {
        bool a = false;
        for(auto dit = des.begin(); dit != des.end(); ++dit) {
            if(*dit == *it) {
                a = true;
                break;
            }
        }
        EXPECT_EQ(a, true);
    }
}

// 拷贝运算符测试
TEST(testCase, unordered_multiset_operator_copy_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jrSTL::unordered_multiset<int, int_hash> src{var, var, var};
    jrSTL::unordered_multiset<int, int_hash> des{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    des = src;
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// 移动运算符测试
TEST(testCase, unordered_multiset_operator_move_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jrSTL::unordered_multiset<int, int_hash> src{var, var, var};
    jrSTL::unordered_multiset<int, int_hash> des{32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2};
    des = std::move(src);
    ASSERT_EQ(src.empty(), true);
}

// clear测试
TEST(testCase, unordered_multiset_clear_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jrSTL::unordered_multiset<int, int_hash> des;
    for(size_t i = 0; i < cnt; i++){
        int t = var;
        des.insert(t);
        des.insert(t);
        --var;
    }
    des.clear();
    EXPECT_EQ(des.size(), 0);
}

// insert测试
TEST(testCase, unordered_multiset_insert_test) {
    std::unordered_multiset<int, int_hash> src;
    jrSTL::unordered_multiset<int, int_hash> des;
    // 拷贝insert
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        auto s = src.insert(n);
        auto d = des.insert(n);
        if(s == src.end())
            EXPECT_EQ(d, des.end());
        else
            EXPECT_EQ(*(s),
                      *(d));
    }
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    for(; it != src.end(); ++it) {
        bool a = false;
        for(auto dit = des.begin(); dit != des.end(); ++dit) {
            if(*dit == *it) {
                a = true;
                break;
            }
        }
        EXPECT_EQ(a, true);
    }
    // 移动insert
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        auto s = src.insert(std::move(n));
        auto d = des.insert(std::move(n));
        if(s == src.end())
            EXPECT_EQ(d, des.end());
        else
            EXPECT_EQ(*(s),
                      *(d));
    }
    ASSERT_EQ(src.size(), des.size());
    it = src.begin();
    for(; it != src.end(); ++it) {
        bool a = false;
        for(auto dit = des.begin(); dit != des.end(); ++dit) {
            if(*dit == *it) {
                a = true;
                break;
            }
        }
        EXPECT_EQ(a, true);
    }
    // 初始化列表insert
    src.insert({1,2,3,4,5,1,2,3,4,5});
    des.insert({1,2,3,4,5,1,2,3,4,5});
    ASSERT_EQ(src.size(), des.size());
    it = src.begin();
    for(; it != src.end(); ++it) {
        bool a = false;
        for(auto dit = des.begin(); dit != des.end(); ++dit) {
            if(*dit == *it) {
                a = true;
                break;
            }
        }
        EXPECT_EQ(a, true);
    }
    // 迭代器insert
    jrSTL::unordered_multiset<int, int_hash> tmp{1,2,3,4,5,1,2,3,4,5};
    std::unordered_multiset<int> tmp0{1,2,3,4,5,1,2,3,4,5};
    src.insert(tmp0.begin(), tmp0.end());
    des.insert(tmp.begin(), tmp.end());
    ASSERT_EQ(src.size(), des.size());
    it = src.begin();
    for(; it != src.end(); ++it) {
        bool a = false;
        for(auto dit = des.begin(); dit != des.end(); ++dit) {
            if(*dit == *it) {
                a = true;
                break;
            }
        }
        EXPECT_EQ(a, true);
    }
}

// insert hint测试
TEST(testCase, unordered_multiset_insert_hint_test) {
    std::unordered_multiset<int, int_hash> src{1,2,3,4,8,9};
    jrSTL::unordered_multiset<int, int_hash> des{1,2,3,4,8,9};
    // insert hint
    auto s_pos = src.begin();
    auto d_pos = des.begin();
    std::advance(s_pos, 4);
    jrSTL::advance(d_pos, 4);
    auto s = src.insert(s_pos, 5);
    auto d = des.insert(d_pos, 5);
    if(s == src.end())
        EXPECT_EQ(d, des.end());
    else
        EXPECT_EQ(*s, *d);
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    for(; it != src.end(); ++it) {
        bool a = false;
        for(auto dit = des.begin(); dit != des.end(); ++dit) {
            if(*dit == *it) {
                a = true;
                break;
            }
        }
        EXPECT_EQ(a, true);
    }
}

// emplace测试
TEST(testCase, unordered_multiset_emplace_test) {
    std::unordered_multiset<int, int_hash> src;
    jrSTL::unordered_multiset<int, int_hash> des;
    // emplace
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        auto s = src.emplace(n);
        auto d = des.emplace(n);
        if(s == src.end())
            EXPECT_EQ(d, des.end());
        else
            EXPECT_EQ(*(s), *(d));
    }
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    for(; it != src.end(); ++it) {
        bool a = false;
        for(auto dit = des.begin(); dit != des.end(); ++dit) {
            if(*dit == *it) {
                a = true;
                break;
            }
        }
        EXPECT_EQ(a, true);
    }
}

// emplace hint测试
TEST(testCase, unordered_multiset_emplace_hint_test) {
    std::unordered_multiset<int, int_hash> src{1,2,3,4,8,9};
    jrSTL::unordered_multiset<int, int_hash> des{1,2,3,4,8,9};
    auto s_pos = src.begin();
    auto d_pos = des.begin();
    std::advance(s_pos, 4);
    jrSTL::advance(d_pos, 4);
    auto s = src.emplace_hint(s_pos, 5);
    auto d = des.emplace_hint(d_pos, 5);
    if(s == src.end())
        EXPECT_EQ(d, des.end());
    else
        EXPECT_EQ(*s, *d);
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    for(; it != src.end(); ++it) {
        bool a = false;
        for(auto dit = des.begin(); dit != des.end(); ++dit) {
            if(*dit == *it) {
                a = true;
                break;
            }
        }
        EXPECT_EQ(a, true);
    }
}

// erase测试
TEST(testCase, unordered_multiset_erase_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var, 20);
    jrSTL::unordered_multiset<int, int_hash> des;
    for(size_t i = 0; i < cnt; i++) {
        des.insert(static_cast<int>(i));
        des.insert(static_cast<int>(i));
    }
    cnt *= 2;
    ASSERT_EQ(cnt, des.size());
    // 关键字删除
    EXPECT_EQ(2, des.erase(cnt / 4));
    ASSERT_EQ((cnt -= 2), des.size());
    // 单位置erase
    auto i = des.cbegin();
    auto j = des.cbegin();
    jrSTL::advance(i, 3);
    jrSTL::advance(j, 2);
    EXPECT_EQ(*i, *(des.erase(j)));
    ASSERT_EQ(--cnt, des.size());
    // 范围erase
    i = des.cbegin();
    j = des.cbegin();
    jrSTL::advance(j, 2);
    auto j0 = des.cbegin();
    jrSTL::advance(j0, 6);
    jrSTL::advance(i, 6);
    EXPECT_EQ(*i, *(des.erase(j, j0)));
    ASSERT_EQ((cnt -= 4), des.size());
}

// swap测试
TEST(testCase, unordered_multiset_swap_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jrSTL::unordered_multiset<int, int_hash> src;
    jrSTL::unordered_multiset<int, int_hash> des;
    for(size_t i = 0; i < cnt; i++) {
        src.insert(var + 1);
        des.insert(var - 1);
        src.insert(var + 1);
        des.insert(var - 1);
        --var;
    }
    jrSTL::unordered_multiset<int, int_hash> tmp = src;
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

// count测试
TEST(testCase, unordered_multiset_count) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    std::unordered_multiset<int, int_hash> src;
    jrSTL::unordered_multiset<int, int_hash> des;
    for(int i = 0; i < static_cast<int>(cnt); i++) {
        src.insert(i);
        des.insert(i);
        src.insert(i);
        des.insert(i);
    }
    ASSERT_EQ(des.size(), src.size());
    for(int i = -static_cast<int>(cnt);
        i < static_cast<int>(cnt); i++) {
        EXPECT_EQ(src.count(i), des.count(i));
    }
}

// find测试
TEST(testCase, unordered_multiset_find) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    std::unordered_multiset<int, int_hash> src;
    jrSTL::unordered_multiset<int, int_hash> des;
    for(int i = 0; i < static_cast<int>(cnt); i++) {
        src.insert(i);
        des.insert(i);
        src.insert(i);
        des.insert(i);
    }
    ASSERT_EQ(des.size(), src.size());
    for(int i = -static_cast<int>(cnt);
        i < static_cast<int>(cnt); i++) {
        if(src.find(i) == src.end())
            EXPECT_EQ(des.find(i), des.end());
        else
            EXPECT_EQ(*src.find(i), *des.find(i));
    }
}

// equal_range测试
TEST(testCase, unordered_multiset_range) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    jrSTL::unordered_multiset<int, int_hash> des;
    for(int i = 0; i < static_cast<int>(cnt); i++) {
        des.insert(i);
        des.insert(i);
    }
    cnt *= 2;
    ASSERT_EQ(cnt, des.size());
    size_t v;
    get_random_size_var(MAX_SIZE, v, var);
    auto i1 = des.find(static_cast<int>(v));
    auto i2 = i1;
    while(i2 != des.end() && (*i2 == static_cast<int>(v)))
        ++i2;
    auto p2 = des.equal_range(static_cast<int>(v));
    EXPECT_EQ(i1, p2.first);
    EXPECT_EQ(i2, p2.second);
}

//// 桶接口测试
//TEST(testCase, unordered_multiset_bucket) {
//    // 打印某个桶
//}
