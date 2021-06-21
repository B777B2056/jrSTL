#include <gtest/gtest.h>
#include <unordered_map>
#include "../container/associate/jr_map.h"
#include "../container/associate/jr_unordered_map.h"

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
TEST(testCase,unordered_multimap_iterator_ctor_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var, 1);
    panzer::multimap<int, int> src;
    for(size_t i = 0; i < cnt; i++) {
        int t = var;
        src.insert(std::make_pair(t, i));
        --var;
    }
    // 迭代器范围构造函数
    panzer::unordered_multimap<int, int, int_hash> des(src.begin(), src.end());
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
TEST(testCase,unordered_multimap_copy_ctor_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    panzer::unordered_multimap<int, int, int_hash> src;
    for(size_t i = 0; i < cnt; i++){
        int t = var;
        src.insert(std::make_pair(t, i));
        src.insert(std::make_pair(t, i));
        --var;
    }
    // 拷贝构造函数
    panzer::unordered_multimap<int, int, int_hash> des(src);
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// 移动构造函数测试
TEST(testCase,unordered_multimap_move_ctor_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    panzer::unordered_multimap<int, int, int_hash> src;
    for(size_t i = 0; i < cnt; i++){
        int t = var;
        src.insert(std::make_pair(t, i));
        src.insert(std::make_pair(t, i));
        --var;
    }
    // 移动构造函数
    panzer::unordered_multimap<int, int, int_hash> des(std::move(src));
    ASSERT_EQ(src.empty(), true);
}

// 初始化列表构造函数测试
TEST(testCase,unordered_multimap_initializer_list_ctor_test) {
    std::unordered_multimap<int, int, int_hash> src{{32, 0},{53423,1},{4245,2},{25,3},{234,4},{25,5},{45,6},{2,7},{235,8},{23,9},{24,10},{6,11},{47,12},{6,13},{5224,14},{2,15}};
    panzer::unordered_multimap<int, int, int_hash> des{{32, 0},{53423,1},{4245,2},{25,3},{234,4},{25,5},{45,6},{2,7},{235,8},{23,9},{24,10},{6,11},{47,12},{6,13},{5224,14},{2,15}};
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
TEST(testCase, unordered_multimap_operator_copy_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    panzer::unordered_multimap<int, int, int_hash> src{{var, 0}, {var+1, 1}, {var+2,2}};
    panzer::unordered_multimap<int, int, int_hash> des{{32, 0},{53423,1},{4245,2},{25,3},{234,4},{25,5},{45,6},{2,7},{235,8},{23,9},{24,10},{6,11},{47,12},{6,13},{5224,14},{2,15}};
    des = src;
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// 移动运算符测试
TEST(testCase, unordered_multimap_operator_move_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    panzer::unordered_multimap<int, int, int_hash> src{{var, 0}, {var, 1}, {var, 2}};
    panzer::unordered_multimap<int, int, int_hash> des{{32, 0},{53423,1},{4245,2},{25,3},{234,4},{25,5},{45,6},{2,7},{235,8},{23,9},{24,10},{6,11},{47,12},{6,13},{5224,14},{2,15}};
    des = std::move(src);
    ASSERT_EQ(src.empty(), true);
}

// clear测试
TEST(testCase, unordered_multimap_clear_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    panzer::unordered_multimap<int, int, int_hash> des;
    for(size_t i = 0; i < cnt; i++){
        int t = var;
        des.insert(std::make_pair(t, i));
        des.insert(std::make_pair(t, i));
        --var;
    }
    des.clear();
    EXPECT_EQ(des.size(), 0);
}

// insert测试
TEST(testCase, unordered_multimap_insert_test) {
    std::unordered_multimap<int, int, int_hash> src;
    panzer::unordered_multimap<int, int, int_hash> des;
    std::pair<const int, int> m[] = {{32, 0},{53423,1},{4245,2},{25,3},{234,4},{25,5},{45,6},{2,7},{235,8},{23,9},{24,10},{6,11},{47,12},{6,13},{5224,14},{2,15}};
    // 拷贝insert
    for(auto n : m) {
        auto s = src.insert(n);
        auto d = des.insert(n);
        if(s == src.end())
            EXPECT_EQ(d, des.end());
        else
            EXPECT_EQ(*s, *d);
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
    for(auto n : m) {
        auto s = src.insert(std::move(n));
        auto d = des.insert(std::move(n));
        if(s == src.end())
            EXPECT_EQ(d, des.end());
        else
            EXPECT_EQ(*s, *d);
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
    src.insert({{1,0},{2,1},{3,2},{4,3},{5,4},{1,5},{2,6},{3,7},{4,8},{5,9}});
    des.insert({{1,0},{2,1},{3,2},{4,3},{5,4},{1,5},{2,6},{3,7},{4,8},{5,9}});
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
    panzer::unordered_multimap<int, int, int_hash> tmp{{1,0},{2,1},{3,2},{4,3},{5,4},{1,5},{2,6},{3,7},{4,8},{5,9}};
    std::unordered_multimap<int, int, int_hash> tmp0{{1,0},{2,1},{3,2},{4,3},{5,4},{1,5},{2,6},{3,7},{4,8},{5,9}};
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
TEST(testCase, unordered_multimap_insert_hint_test) {
    std::unordered_multimap<int, int, int_hash> src{{1,0},{2,1},{3,2},{4,3},{8,4},{9,5}};
    panzer::unordered_multimap<int, int, int_hash> des{{1,0},{2,1},{3,2},{4,3},{8,4},{9,5}};
    // insert hint
    auto s_pos = src.begin();
    auto d_pos = des.begin();
    std::advance(s_pos, 4);
    panzer::advance(d_pos, 4);
    auto s = src.insert(s_pos, {5, 5});
    auto d = des.insert(d_pos, {5, 5});
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
TEST(testCase, unordered_multimap_emplace_test) {
    std::unordered_multimap<int, int, int_hash> src;
    panzer::unordered_multimap<int, int, int_hash> des;
     std::pair<const int, int> m[] = {{32, 0},{53423,1},{4245,2},{25,3},{234,4},{25,5},{45,6},{2,7},{235,8},{23,9},{24,10},{6,11},{47,12},{6,13},{5224,14},{2,15}};
    // emplace
    for(auto n : m) {
        auto s = src.emplace(n);
        auto d = des.emplace(n);
        if(s == src.end())
            EXPECT_EQ(d, des.end());
        else
            EXPECT_EQ(*s, *d);
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
TEST(testCase, unordered_multimap_emplace_hint_test) {
    std::unordered_multimap<int, int, int_hash> src{{1,0},{2,1},{3,2},{4,3},{8,4},{9,5}};
    panzer::unordered_multimap<int, int, int_hash> des{{1,0},{2,1},{3,2},{4,3},{8,4},{9,5}};
    // insert hint
    auto s_pos = src.begin();
    auto d_pos = des.begin();
    std::advance(s_pos, 4);
    panzer::advance(d_pos, 4);
    auto s = src.emplace_hint(s_pos, 5, 5);
    auto d = des.emplace_hint(d_pos, 5, 5);
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
TEST(testCase, unordered_multimap_erase_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var, 100);
    panzer::unordered_multimap<int, int, int_hash> des;
    for(size_t i = 0; i < cnt; i++) {
        des.insert({i, i});
        des.insert({i, i});
    }
    cnt *= 2;
    ASSERT_EQ(cnt, des.size());
    // 关键字删除
    EXPECT_EQ(2, des.erase(cnt / 4));
    ASSERT_EQ((cnt -= 2), des.size());
    // 单位置erase
    auto i = des.cbegin();
    auto j = des.cbegin();
    panzer::advance(i, 3);
    panzer::advance(j, 2);
    EXPECT_EQ(*i, *(des.erase(j)));
    ASSERT_EQ(--cnt, des.size());
    // 范围erase
    i = des.cbegin();
    j = des.cbegin();
    panzer::advance(j, 2);
    auto j0 = des.cbegin();
    panzer::advance(j0, 6);
    panzer::advance(i, 6);
    EXPECT_EQ(*i, *(des.erase(j, j0)));
    ASSERT_EQ((cnt -= 4), des.size());
}

// swap测试
TEST(testCase, unordered_multimap_swap_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    panzer::unordered_multimap<int, int, int_hash> src;
    panzer::unordered_multimap<int, int, int_hash> des;
    for(size_t i = 0; i < cnt; i++) {
        src.insert({var + 1, i});
        des.insert({var - 1, i});
        src.insert({var + 1, i});
        des.insert({var - 1, i});
        --var;
    }
    panzer::unordered_multimap<int, int, int_hash> tmp = src;
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
TEST(testCase, unordered_multimap_count) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    std::unordered_multimap<int, int, int_hash> src;
    panzer::unordered_multimap<int, int, int_hash> des;
    for(int i = 0; i < static_cast<int>(cnt); i++) {
        src.insert({i, i});
        des.insert({i, i});
        src.insert({i, i});
        des.insert({i, i});
    }
    ASSERT_EQ(des.size(), src.size());
    for(int i = -static_cast<int>(cnt);
        i < static_cast<int>(cnt); i++) {
        EXPECT_EQ(src.count(i), des.count(i));
    }
}

// find测试
TEST(testCase, unordered_multimap_find) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    std::unordered_multimap<int, int, int_hash> src;
    panzer::unordered_multimap<int, int, int_hash> des;
    for(int i = 0; i < static_cast<int>(cnt); i++) {
        src.insert({i, i});
        des.insert({i, i});
        src.insert({i, i});
        des.insert({i, i});
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
TEST(testCase, unordered_multimap_range) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    panzer::unordered_multimap<int, int, int_hash> des;
    for(int i = 0; i < static_cast<int>(cnt); i++) {
        des.insert({i, i});
        des.insert({i, i});
    }
    ASSERT_EQ(cnt * 2, des.size());
    size_t v;
    get_random_size_var(MAX_SIZE, v, var);
    auto i1 = des.find(static_cast<int>(v));
    auto i2 = i1;
    while(i2 != des.end()
          && (i2->first == static_cast<int>(v))
          && (i2->second == static_cast<int>(v)))
        ++i2;
    auto p2 = des.equal_range(static_cast<int>(v));
    EXPECT_EQ(i1, p2.first);
    EXPECT_EQ(i2, p2.second);
}

//// 桶接口测试
//TEST(testCase, unordered_set_bucket) {
//    // 打印某个桶
//}
