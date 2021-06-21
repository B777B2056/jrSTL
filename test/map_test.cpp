#include <gtest/gtest.h>
#include <map>
#include "../container/sequence/jr_deque.h"
#include "../container/associate/jr_map.h"

#define MAX_SIZE 2000

void get_random_size_var(size_t max_size,
                         size_t& size,
                         int& var,
                         size_t min_size = 0);

// 迭代器范围构造函数测试
TEST(testCase,map_iterator_ctor_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var, 1);
    std::map<int, int> src;
    for(size_t i = 0; i < cnt; i++) {
        int t = var;
        src.insert(std::make_pair(t, i));
        src.insert(std::make_pair(t, i));
        --var;
    }
    // 迭代器范围构造函数
    panzer::map<int, int> des(src.begin(), src.end());
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// 拷贝构造函数测试
TEST(testCase,map_copy_ctor_test) {
    size_t cnt;
    int var;
    get_random_size_var(50, cnt, var);
    panzer::map<int, int> src;
    for(size_t i = 0; i < cnt; i++){
        int t = var;
        src.insert(std::make_pair(t, i));
        src.insert(std::make_pair(t, i));
        --var;
    }
    // 拷贝构造函数
    panzer::map<int, int> des(src);
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// 移动构造函数测试
TEST(testCase,map_move_ctor_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    panzer::map<int, int> src;
    for(size_t i = 0; i < cnt; i++){
        int t = var;
        src.insert(std::make_pair(t, i));
        src.insert(std::make_pair(t, i));
        --var;
    }
    // 移动构造函数
    panzer::map<int, int> des(std::move(src));
    ASSERT_EQ(src.empty(), true);
}

// 初始化列表构造函数测试
TEST(testCase,map_initializer_list_ctor_test) {
    std::map<int, int> src{{32, 0},{53423,1},{4245,2},{25,3},{234,4},{25,5},{45,6},{2,7},{235,8},{23,9},{24,10},{6,11},{47,12},{6,13},{5224,14},{2,15}};
    panzer::map<int, int> des{{32, 0},{53423,1},{4245,2},{25,3},{234,4},{25,5},{45,6},{2,7},{235,8},{23,9},{24,10},{6,11},{47,12},{6,13},{5224,14},{2,15}};
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// 拷贝运算符测试
TEST(testCase, map_operator_copy_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    panzer::map<int,int> src{{var, 0}, {var, 1}, {var, 2}};
    panzer::map<int,int> des{{32, 0},{53423,1},{4245,2},{25,3},{234,4},{25,5},{45,6},{2,7},{235,8},{23,9},{24,10},{6,11},{47,12},{6,13},{5224,14},{2,15}};
    des = src;
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// 移动运算符测试
TEST(testCase, map_operator_move_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    panzer::map<int,int> src{{var, 0}, {var, 1}, {var, 2}};
    panzer::map<int,int> des{{32, 0},{53423,1},{4245,2},{25,3},{234,4},{25,5},{45,6},{2,7},{235,8},{23,9},{24,10},{6,11},{47,12},{6,13},{5224,14},{2,15}};
    des = std::move(src);
    ASSERT_EQ(src.empty(), true);
}

// clear测试
TEST(testCase, map_clear_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    panzer::map<int,int> des;
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
TEST(testCase, map_insert_test) {
    std::map<int,int> src;
    panzer::map<int,int> des;
    std::pair<const int, int> m[] = {{32, 0},{53423,1},{4245,2},{25,3},{234,4},{25,5},{45,6},{2,7},{235,8},{23,9},{24,10},{6,11},{47,12},{6,13},{5224,14},{2,15}};
    // 拷贝insert
    for(std::pair<const int, int> n : m) {
        auto s = src.insert(n);
        auto d = des.insert(n);
        if(s.first == src.end())
            EXPECT_EQ(d.first, des.end());
        else
            EXPECT_EQ(*(s.first),
                      *(d.first));
        EXPECT_EQ(s.second, d.second);
    }
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    // 移动insert
    for(std::pair<const int, int> n : m) {
        auto s = src.insert(std::move(n));
        auto d = des.insert(std::move(n));
        if(s.first == src.end())
            EXPECT_EQ(d.first, des.end());
        else
            EXPECT_EQ(*(s.first),
                      *(d.first));
        EXPECT_EQ(s.second, d.second);
    }
    ASSERT_EQ(src.size(), des.size());
    it = src.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    // 初始化列表insert
    src.insert({{1,0},{2,1},{3,2},{4,3},{5,4},{1,5},{2,6},{3,7},{4,8},{5,9}});
    des.insert({{1,0},{2,1},{3,2},{4,3},{5,4},{1,5},{2,6},{3,7},{4,8},{5,9}});
    ASSERT_EQ(src.size(), des.size());
    it = src.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
    // 迭代器insert
    panzer::map<int,int> tmp{{6,0},{7,1},{8,2},{9,3},{15,4},{21,5},{12,6},{33,7},{24,8},{75,9}};
    std::map<int,int> tmp0{{6,0},{7,1},{8,2},{9,3},{15,4},{21,5},{12,6},{33,7},{24,8},{75,9}};
    src.insert(tmp0.begin(), tmp0.end());
    des.insert(tmp.begin(), tmp.end());
    ASSERT_EQ(src.size(), des.size());
    it = src.begin();
    dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// insert hint测试
TEST(testCase, map_insert_hint_test) {
    std::map<int,int> src{{1,0},{2,1},{3,2},{4,3},{8,4},{9,5}};
    panzer::map<int,int> des{{1,0},{2,1},{3,2},{4,3},{8,4},{9,5}};
    // insert hint
    auto s_pos = src.begin();
    auto d_pos = des.cbegin();
    std::advance(s_pos, 4);
    panzer::advance(d_pos, 4);
    auto s = src.insert(s_pos, std::make_pair(5,3));
    auto d = des.insert(d_pos, std::make_pair(5,3));
    if(s == src.end())
        EXPECT_EQ(d, des.end());
    else
        EXPECT_EQ(*s,*d);
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// emplace测试
TEST(testCase, map_emplace_test) {
    std::map<int,int> src;
    panzer::map<int,int> des;
    // emplace
    for(int n : {32,53423,4245,25,234,25,45,2,235,23,24,6,47,6,5224,2}) {
        auto s = src.emplace(n, n);
        auto d = des.emplace(n, n);
        if(s.first == src.end())
            EXPECT_EQ(d.first, des.end());
        else
            EXPECT_EQ(*(s.first),
                      *(d.first));
        EXPECT_EQ(s.second, d.second);
    }
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// emplace hint测试
TEST(testCase, map_emplace_hint_test) {
    std::map<int,int> src{{1,0},{2,1},{3,2},{4,3},{8,4},{9,5}};
    panzer::map<int,int> des{{1,0},{2,1},{3,2},{4,3},{8,4},{9,5}};
    // insert hint
    auto s_pos = src.begin();
    auto d_pos = des.cbegin();
    std::advance(s_pos, 4);
    panzer::advance(d_pos, 4);
    auto s = src.emplace_hint(s_pos, 5, 10);
    auto d = des.emplace_hint(d_pos, 5, 10);
    if(s == src.end())
        EXPECT_EQ(d, des.end());
    else
        EXPECT_EQ(*s,*d);
    ASSERT_EQ(src.size(), des.size());
    auto it = src.begin();
    auto dit = des.begin();
    for(; dit != des.end(); ++dit, ++it)
        EXPECT_EQ(*it, *dit);
}

// erase测试
TEST(testCase, map_erase_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var, 20);
    panzer::map<int,int> des;
    std::map<int,int> src;
    for(size_t i = 0; i < cnt; i++) {
        src.insert(std::make_pair(var,i));
        des.insert(std::make_pair(var,i));
        src.insert(std::make_pair(var,i));
        des.insert(std::make_pair(var,i));
        --var;
    }
    // 单位置erase
    auto i = src.begin();
    auto j = des.cbegin();
    std::advance(i, 2);
    panzer::advance(j, 2);
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
    panzer::advance(j, 2);
    auto i0 = src.begin();
    auto j0 = des.cbegin();
    std::advance(i0, 6);
    panzer::advance(j0, 6);
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
    panzer::advance(j0, 6);
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
    panzer::advance(j, 2);
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
TEST(testCase, map_swap_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    panzer::map<int,int> src;
    panzer::map<int,int> des;
    for(size_t i = 0; i < cnt; i++) {
        src.insert(std::make_pair(var + 1, i));
        des.insert(std::make_pair(var - 1, i));
        src.insert(std::make_pair(var + 1, i));
        des.insert(std::make_pair(var - 1, i));
        --var;
    }
    panzer::map<int,int> tmp = src;
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
TEST(testCase, map_reverse_index_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    std::map<int,int> src;
    panzer::map<int,int> des;
    for(int i = 0; i < static_cast<int>(cnt); i++) {
        src.insert(std::make_pair(i, i));
        des.insert(std::make_pair(i, i));
        src.insert(std::make_pair(i, i));
        des.insert(std::make_pair(i, i));
    }
    ASSERT_EQ(des.size(), src.size());
    auto it = src.rbegin();
    auto rit = des.rbegin();
    while(rit != des.rend()) {
        EXPECT_EQ(*it, *rit);
        ++rit;
        ++it;
    }
}

// 迭代器-运算
TEST(testCase, map_iterator_sub_test) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var, 20);
    std::map<int,int> src;
    panzer::map<int,int> des;
    for(int i = 0; i < static_cast<int>(cnt); i++) {
        src.insert(std::make_pair(i, i));
        des.insert(std::make_pair(i, i));
        src.insert(std::make_pair(i, i));
        des.insert(std::make_pair(i, i));
    }
    ASSERT_EQ(des.size(), src.size());
    auto it = src.begin();
    auto rit = des.begin();
    std::advance(it, 12);
    panzer::advance(rit, 12);
    std::advance(it, -5);
    panzer::advance(rit, -5);
    EXPECT_EQ(*it, *rit);
}

// count测试
TEST(testCase, map_count) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    std::map<int,int> src;
    panzer::map<int,int> des;
    for(int i = 0; i < static_cast<int>(cnt); i++) {
        src.insert(std::make_pair(i, i));
        des.insert(std::make_pair(i, i));
        src.insert(std::make_pair(i, i));
        des.insert(std::make_pair(i, i));
        src.insert(std::make_pair(i, i));
        des.insert(std::make_pair(i, i));
    }
    ASSERT_EQ(des.size(), src.size());
    for(int i = -static_cast<int>(cnt);
        i < static_cast<int>(cnt); i++) {
        EXPECT_EQ(src.count(i), des.count(i));
    }
}

// find测试
TEST(testCase, map_find) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    std::map<int,int> src;
    panzer::map<int,int> des;
    for(int i = 0; i < static_cast<int>(cnt); i++) {
        src.insert(std::make_pair(i, i));
        des.insert(std::make_pair(i, i));
        src.insert(std::make_pair(i, i));
        des.insert(std::make_pair(i, i));
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

// lower_bound,upper_bound,equal_range测试
TEST(testCase, map_range) {
    size_t cnt;
    int var;
    get_random_size_var(MAX_SIZE, cnt, var);
    std::map<int,int> src;
    panzer::map<int,int> des;
    for(int i = 0; i < static_cast<int>(cnt); i++) {
        src.insert(std::make_pair(i, i));
        des.insert(std::make_pair(i, i));
        src.insert(std::make_pair(i, i));
        des.insert(std::make_pair(i, i));
    }
    ASSERT_EQ(des.size(), src.size());
    size_t v;
    get_random_size_var(MAX_SIZE, v, var);
    auto p1 = src.equal_range(static_cast<int>(v));
    auto p2 = des.equal_range(static_cast<int>(v));
    if(p1.first == src.end())
        EXPECT_EQ(p2.first == des.end(), true);
    else
        EXPECT_EQ(*p1.first, *p2.first);
    if(p1.second == src.end())
        EXPECT_EQ(p2.second == des.end(), true);
    else
        EXPECT_EQ(*p1.second, *p2.second);
}
