#include <gtest/gtest.h>
#include <algorithm>
#include <thread>
#include <chrono>
#include <sstream>
#include <random>
#include "../algorithm/jr_algorithm.h"
#include "../algorithm/jr_numeric.h"
#include "../functional/jr_functional.h"
#include "../container/sequence/jr_vector.h"
#include "../container/sequence/jr_array.h"
#include "../container/sequence/jr_forward_list.h"

TEST(testCase, for_each) {
    struct Sum
    {
        void operator()(int n) { sum += n; }
        int sum{0};
    };
    std::vector<int> nums{3, 4, 2, 8, 15, 267};
    // 对每个数调用 Sum::operator()
    Sum s = std::for_each(nums.begin(), nums.end(), Sum());
    ASSERT_EQ(s.sum, 299);
}

TEST(testCase, count) {
    jrSTL::vector<int> v{ 1, 2, 3, 4, 4, 3, 7, 8, 9, 10 };
    // 确定 std::vector 中有多少个整数匹配目标值。
    int target1 = 3;
    int target2 = 5;
    int num_items1 = jrSTL::count(v.begin(), v.end(), target1);
    ASSERT_EQ(num_items1, 2);
    int num_items2 = jrSTL::count(v.begin(), v.end(), target2);
    ASSERT_EQ(num_items2, 0);
    // 用 lambda 表达式计量能被 3 整除的元素数。
    int num_items3 = jrSTL::count_if(v.begin(), v.end(), [](int i){return i % 3 == 0;});
    ASSERT_EQ(num_items3, 3);
}

TEST(testCase, mismatch) {
    std::string in = "abXYZba";
    ASSERT_EQ(std::string(in.begin(),
                          std::mismatch(in.begin(), in.end(), in.rbegin()).first), "ab");
}

TEST(testCase, find) {
    int n1 = 3;
    int n2 = 5;
    jrSTL::vector<int> v{0, 1, 2, 3, 4};
    auto result1 = jrSTL::find(jrSTL::begin(v), jrSTL::end(v), n1);
    auto result2 = jrSTL::find(jrSTL::begin(v), jrSTL::end(v), n2);
    ASSERT_EQ(*result1, 3);
    ASSERT_EQ(result2, v.end());
}

TEST(testCase, find_end) {
    // bid迭代器
    jrSTL::vector<int> v{1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4};
    jrSTL::vector<int>::iterator result;
    jrSTL::vector<int> t1{1, 2, 3};
    result = jrSTL::find_end(v.begin(), v.end(), t1.begin(), t1.end());
    ASSERT_EQ(jrSTL::distance(v.begin(), result), 8);
    jrSTL::vector<int> t2{4, 5, 6};
    result = jrSTL::find_end(v.begin(), v.end(), t2.begin(), t2.end());
    ASSERT_EQ(result, v.end());
    // forward迭代器
    jrSTL::forward_list<int> v0{1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4};
    jrSTL::forward_list<int>::iterator result0;
    jrSTL::forward_list<int> t10{1, 2, 3};
    result0 = jrSTL::find_end(v0.begin(), v0.end(), t10.begin(), t10.end());
    ASSERT_EQ(jrSTL::distance(v0.begin(), result0), 8);
    jrSTL::forward_list<int> t20{4, 5, 6};
    result0 = jrSTL::find_end(v0.begin(), v0.end(), t20.begin(), t20.end());
    ASSERT_EQ(result0, v0.end());
}

TEST(testCase, find_first_of) {
    jrSTL::vector<int> v{0, 2, 3, 25, 5};
    jrSTL::vector<int> t{3, 19, 10, 2};
    auto result = jrSTL::find_first_of(v.begin(), v.end(), t.begin(), t.end());
    ASSERT_EQ(jrSTL::distance(v.begin(), result), 1);
}

template<typename Container>
bool in_quote(const Container& cont, const std::string& s)
{
    return jrSTL::search(cont.begin(), cont.end(), s.begin(), s.end()) != cont.end();
}

TEST(testCase, search) {
    std::string str = "why waste time learning, when ignorance is instantaneous?";
    std::string s1 = "learning", s2 = "lemming";
    jrSTL::vector<char> vec;
    for(auto i = str.begin(); i != str.end(); ++i)
        vec.push_back(*i);
    ASSERT_EQ(true, in_quote(vec, s1));
    ASSERT_EQ(false, in_quote(vec, s2));
}

template <class Container, class Size, class T>
bool consecutive_values(const Container& c, Size count, const T& v, int f)
{
  return f ? jrSTL::search_n(std::begin(c),std::end(c),count,v) != std::end(c)
          : std::search_n(std::begin(c),std::end(c),count,v) != std::end(c);
}

TEST(testCase, search_n) {
   const char sequence[] = "1001010100010101001010101";
   ASSERT_EQ(consecutive_values(sequence,4,'0',0), consecutive_values(sequence,4,'0',1));
   ASSERT_EQ(consecutive_values(sequence,3,'0',0), consecutive_values(sequence,3,'0',1));
}

TEST(testCase, adjacent_find) {
   jrSTL::vector<int> v1{0, 1, 2, 3, 40, 40, 41, 41, 5};
   auto i1 = jrSTL::adjacent_find(v1.begin(), v1.end());
   ASSERT_EQ(jrSTL::distance(v1.begin(), i1), 4);
   auto i2 = jrSTL::adjacent_find(v1.begin(), v1.end(), std::greater<int>());
   ASSERT_EQ(jrSTL::distance(v1.begin(), i2), 7);
}

TEST(testCase, copy) {
    // POD类型
    const char *str = "afajehfjagefashjkfihufgh";
    char des_str[25];
    jrSTL::copy(str, str+25, des_str);
    ASSERT_EQ(*str, *des_str);
    // 非POD类型
    jrSTL::vector<int> from_vector(10);
    jrSTL::iota(from_vector.begin(), from_vector.end(), 0);
    jrSTL::vector<int> to_vector, des_vector, odd_vector;
    jrSTL::copy(from_vector.begin(), from_vector.end(),
              jrSTL::back_inserter(to_vector));
    jrSTL::copy(to_vector.begin(), to_vector.end(),
                 jrSTL::back_inserter(des_vector));
    ASSERT_EQ(to_vector.size(), des_vector.size());
    for(size_t i = 0; i < des_vector.size(); i++)
        ASSERT_EQ(to_vector[i], des_vector[i]);

    jrSTL::copy_if(to_vector.begin(), to_vector.end(),
                 jrSTL::back_inserter(odd_vector),
                 [](int x) { return (x % 2) == 1; });

    for(size_t i = 0; i < odd_vector.size(); i++)
        ASSERT_EQ(odd_vector[i], 2 * i + 1);
}

TEST(testCase, copy_n) {
    std::string in = "1234567890";
    std::string out;
    jrSTL::copy_n(in.begin(), 4, std::back_inserter(out));
    ASSERT_EQ(out, "1234");
}

TEST(testCase, copy_backward) {
    jrSTL::vector<int> from_vector;
    for (int i = 0; i < 10; i++) {
        from_vector.push_back(i);
    }
    jrSTL::vector<int> to_vector(15, 0);
    jrSTL::copy_backward(from_vector.begin(),
                          from_vector.end(),
                          to_vector.end());
    auto it = to_vector.begin();
    for (auto i: {0,0,0,0,0,0,1,2,3,4,5,6,7,8,9}) {
        ASSERT_EQ(i, *it++);
    }
}

static std::string str_thread;
void f(int n) {
    std::this_thread::sleep_for(std::chrono::seconds(n));
    std::stringstream ss;
    ss << "thread " << n << " ended" << " ";
    str_thread += ss.str();
}

TEST(testCase, move) {
    jrSTL::vector<std::thread> v;
    v.emplace_back(f, 1);
    v.emplace_back(f, 2);
    v.emplace_back(f, 3);
    jrSTL::vector<std::thread> l;
    jrSTL::move(v.begin(), v.end(), jrSTL::back_inserter(l));
    for (auto& t : l)
        t.join();
    ASSERT_EQ(str_thread, "thread 1 ended thread 2 ended thread 3 ended ");
}

TEST(testCase, fill) {
    jrSTL::vector<int> v{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    jrSTL::fill(v.begin(), v.end(), -1);
    for (auto elem : v) {
        ASSERT_EQ(elem, -1);
    }
}

TEST(testCase, fill_n) {
    jrSTL::vector<int> v1{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    jrSTL::fill_n(v1.begin(), 5, -1);
    auto it = v1.begin();
    for (auto elem : {-1,-1,-1,-1,-1,5,6,7,8,9}) {
        ASSERT_EQ(elem, *it++);
    }
}

TEST(testCase, remove) {
    std::string str1 = "Text with some   spaces";
    str1.erase(jrSTL::remove(str1.begin(), str1.end(), ' '),
               str1.end());
    ASSERT_EQ(str1, "Textwithsomespaces");
    std::string str2 = "Text\n with\tsome \t  whitespaces\n\n";
    str2.erase(jrSTL::remove_if(str2.begin(),
                              str2.end(),
                              [](unsigned char x){return std::isspace(x);}),
               str2.end());
    ASSERT_EQ(str2, "Textwithsomewhitespaces");
}

TEST(testCase, replace) {
   jrSTL::array<int, 10> s{5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
   jrSTL::replace(s.begin(), s.end(), 8, 88);
   auto it = s.begin();
   for (int a : {5,7,4,2,88,6,1,9,0,3}) {
       ASSERT_EQ(a, *it++);
   }
   jrSTL::replace_if(s.begin(), s.end(),
                   std::bind(std::less<int>(), std::placeholders::_1, 5), 55);
   it = s.begin();
   for (int a : {5,7,55,55,88,6,55,9,55,55}) {
       ASSERT_EQ(a, *it++);
   }
}

TEST(testCase, rotate) {
    jrSTL::vector<int> v{2, 4, 2, 0, 5, 10, 7, 3, 7, 1};
    // 插入排序
    for (auto i = v.begin(); i != v.end(); ++i) {
        jrSTL::rotate(jrSTL::upper_bound(v.begin(), i, *i), i, i+1);
    }
    auto it = v.begin();
    for (int a : {0,1,2,2,3,4,5,7,7,10}) {
        ASSERT_EQ(a, *it++);
    }
    // 简单地旋转到左侧
    jrSTL::rotate(v.begin(), v.begin() + 1, v.end());
    it = v.begin();
    for (int a : {1,2,2,3,4,5,7,7,10,0}) {
        ASSERT_EQ(a, *it++);
    }
    // 简单地旋转到右侧
    jrSTL::rotate(v.rbegin(), v.rbegin() + 1, v.rend());
    it = v.begin();
    for (int a : {0,1,2,2,3,4,5,7,7,10}) {
        ASSERT_EQ(a, *it++);
    }
}

TEST(testCase, reverse) {
    jrSTL::vector<int> v{1,2,3};
    jrSTL::reverse(std::begin(v), std::end(v));
    auto it = v.begin();
    for (int a : {3,2,1}) {
        ASSERT_EQ(a, *it++);
    }
    int a[] = {4, 5, 6, 7};
    jrSTL::reverse(std::begin(a), std::end(a));
    int *i = a;
    for (int m : {7,6,5,4}) {
        ASSERT_EQ(m, *i++);
    }
}

TEST(testCase, unique) {
    // 含有数个重复元素的 vector
    jrSTL::vector<int> v{1,2,1,1,3,3,3,4,5,4};
    // 移除相继（毗邻）的重复元素
    auto last = jrSTL::unique(v.begin(), v.end());
    // v 现在保有 {1 2 1 3 4 5 4 x x x} ，其中 x 不确定
    v.erase(last, v.end());
    auto it = v.begin();
    for (int a : {1,2,1,3,4,5,4}) {
        ASSERT_EQ(a, *it++);
    }
    // sort 后 unique 以移除所有重复
    jrSTL::sort(v.begin(), v.end()); // {1 1 2 3 4 4 5}
    last = jrSTL::unique(v.begin(), v.end());
    // v 现在保有 {1 2 3 4 5 x x} ，其中 'x' 不确定
    v.erase(last, v.end());
    it = v.begin();
    for (int a : {1,2,3,4,5}) {
        ASSERT_EQ(a, *it++);
    }
}

TEST(testCase, is_partitioned) {
    jrSTL::array<int, 9> v = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    auto is_even = [](int i){ return i % 2 == 0; };
    ASSERT_EQ(jrSTL::is_partitioned(v.begin(), v.end(), is_even), false);
    jrSTL::partition(v.begin(), v.end(), is_even);
    ASSERT_EQ(jrSTL::is_partitioned(v.begin(), v.end(), is_even), true);
    jrSTL::reverse(v.begin(), v.end());
    ASSERT_EQ(jrSTL::is_partitioned(v.begin(), v.end(), is_even), false);
}

template <class ForwardIt>
void quicksort(ForwardIt first, ForwardIt last) {
    if(first == last)
        return;
    auto pivot = *first;
    ForwardIt middle1 = jrSTL::partition(first, last,
                         [pivot](const int& em){ return em < pivot; });
    ForwardIt middle2 = jrSTL::partition(middle1, last,
                         [pivot](const int& em){ return !(pivot < em); });
    if(middle1 == middle2)
        return;
    quicksort(first, middle1);
    quicksort(middle2, last);
 }

TEST(testCase, partition) {
    jrSTL::vector<int> v = {0,1,2,3,4,5,6,7,8,9};
    auto it = jrSTL::partition(v.begin(), v.end(), [](int i){return i % 2 == 0;});
    for(auto i = v.begin(); i != it; ++i) {
        ASSERT_EQ((*i) % 2 == 0, true);
    }
    jrSTL::forward_list<int> fl = {1, 30, -4, 3, 5, -4, 1, 6, -8, 2, -5, 64, 1, 92};
    quicksort(fl.begin(), fl.end());
    auto a = fl.begin();
    for(auto n : {-8,-5,-4,-4,1,1,1,2,3,5,6,30,64,92}) {
        ASSERT_EQ(*a++, n);
    }
}

TEST(testCase, stable_partition) {
    jrSTL::vector<int> v{0, 0, 3, 0, 2, 4, 5, 0, 7};
    jrSTL::stable_partition(v.begin(), v.end(), [](int n){return n>0;});
    auto a = v.begin();
    for(auto n : {3,2,4,5,7}) {
        ASSERT_EQ(*a++, n);
    }
}

TEST(testCase, partition_point) {
    jrSTL::array<int, 9> v = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    auto is_even = [](int i){ return i % 2 == 0; };
    jrSTL::partition(v.begin(), v.end(), is_even);
    auto p = jrSTL::partition_point(v.begin(), v.end(), is_even);
    ASSERT_EQ(*p, 5);
}

TEST(testCase, is_sorted) {
    int digits[] = {3, 1, 4, 1, 5};
    ASSERT_EQ(jrSTL::is_sorted(jrSTL::begin(digits), jrSTL::end(digits)), false);
    jrSTL::sort(jrSTL::begin(digits), jrSTL::end(digits));
    ASSERT_EQ(jrSTL::is_sorted(jrSTL::begin(digits), jrSTL::end(digits)), true);
}

TEST(testCase, is_sorted_until) {
    std::random_device rd;
    std::mt19937 g(rd());
    const int N = 6;
    int nums[N] = {1,1,2,5,-1,-19};
    ASSERT_EQ(jrSTL::distance(nums, jrSTL::is_sorted_until(nums, nums + N)), 4);
}

TEST(testCase, sort) {
    jrSTL::array<int, 10> s = {5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
    // 用默认的 operator< 排序
    jrSTL::sort(s.begin(), s.end());
    for(size_t i = 1; i < 10; i++) {
        ASSERT_LE(s[i-1], s[i]);
    }

    // 用标准库比较函数对象排序
    jrSTL::sort(s.begin(), s.end(), jrSTL::greater<int>());
    for(size_t i = 1; i < 10; i++) {
        ASSERT_GE(s[i-1], s[i]);
    }

    // 用自定义函数对象排序
    struct {
        bool operator()(int a, int b) const
        {
            return a < b;
        }
    } customLess;
    jrSTL::sort(s.begin(), s.end(), customLess);
    for(size_t i = 1; i < 10; i++) {
        ASSERT_LE(s[i-1], s[i]);
    }

    // 用 lambda 表达式排序
    jrSTL::sort(s.begin(), s.end(), [](int a, int b) {
        return b < a;
    });
    for(size_t i = 1; i < 10; i++) {
        ASSERT_GE(s[i-1], s[i]);
    }
}

TEST(testCase, partial_sort) {
    jrSTL::array<int, 10> s{5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
    jrSTL::partial_sort(s.begin(), s.begin() + 3, s.end());
    for(size_t i = 1; i < 3; i++) {
        ASSERT_LE(s[i-1], s[i]);
    }
}

struct Employee {
    int age;
    std::string name;  // 不参与比较
};

bool operator<(const Employee &lhs,
               const Employee &rhs) {
    return lhs.age < rhs.age;
}

TEST(testCase, stable_sort) {
    jrSTL::vector<Employee> v =
    {
        {108, "Zaphod"},
        {32, "Arthur"},
        {108, "Ford"},
    };

    jrSTL::stable_sort(v.begin(), v.end());

    ASSERT_EQ(v[0].age, 32);
    ASSERT_EQ(v[1].age, 108);
    ASSERT_EQ(v[2].age, 108);
    ASSERT_EQ(v[0].name, "Arthur");
    ASSERT_EQ(v[1].name, "Zaphod");
    ASSERT_EQ(v[2].name, "Ford");
}

TEST(testCase, nth_element) {
    jrSTL::vector<int> v{5, 6, 4, 3, 2, 6, 7, 9, 3};
    jrSTL::nth_element(v.begin(), v.begin() + v.size()/2, v.end());
    ASSERT_EQ(v[v.size()/2], 5);
    jrSTL::nth_element(v.begin(), v.begin()+1, v.end(), jrSTL::greater<int>());
    ASSERT_EQ(v[1], 7);
}

TEST(testCase, lower_bound) {
    std::vector<int> data = { 1, 2, 4, 5, 5, 6 };
    jrSTL::vector<int> data0 = { 1, 2, 4, 5, 5, 6 };

    for (int i = 0; i < 8; ++i) {
        // 搜索首个不小于 i 的元素
        auto lower = std::lower_bound(data.begin(), data.end(), i);
        auto lower0 = jrSTL::lower_bound(data0.begin(), data0.end(), i);
        if (lower0 != data0.end())
            ASSERT_EQ(*lower, *lower0);
        else
            ASSERT_EQ(data0.end(), lower0);
    }
}

TEST(testCase, upper_bound) {
    std::vector<int> data = { 1, 2, 4, 5, 5, 6 };
    jrSTL::vector<int> data0 = { 1, 2, 4, 5, 5, 6 };

    for (int i = 0; i < 8; ++i) {
        // 搜索首个不小于 i 的元素
        auto upper = std::upper_bound(data.begin(), data.end(), i);
        auto upper0 = jrSTL::upper_bound(data0.begin(), data0.end(), i);
        if (upper0 != data0.end())
            ASSERT_EQ(*upper, *upper0);
        else
            ASSERT_EQ(data0.end(), upper0);
    }
}

TEST(testCase, binary_search) {
   std::vector<int> haystack {1, 3, 4, 5, 9};
   std::vector<int> needles {1, 2, 3};

   jrSTL::vector<int> haystack0 {1, 3, 4, 5, 9};
   jrSTL::vector<int> needles0 {1, 2, 3};

   for (size_t i = 0; i < 3; ++i) {
       auto a = std::binary_search(haystack.begin(), haystack.end(), needles[i]);
       auto b = jrSTL::binary_search(haystack0.begin(), haystack0.end(), needles0[i]);
       ASSERT_EQ(a, b);
   }
}

struct S {
    int number;
    char name;
    // 注：此比较运算符忽略 name
    bool operator< ( const S& s ) const { return number < s.number; }
};

TEST(testCase, equal_range) {
    // 注：无序，仅相对定义如下的 S 划分
    const std::vector<S> vec = { {1,'A'}, {2,'B'}, {2,'C'}, {2,'D'}, {4,'G'}, {3,'F'} };
    const jrSTL::vector<S> vec0 = { {1,'A'}, {2,'B'}, {2,'C'}, {2,'D'}, {4,'G'}, {3,'F'} };

    const S value = {2, '?'};

    const auto p = std::equal_range(vec.begin(), vec.end(), value);
    const auto q = jrSTL::equal_range(vec0.begin(), vec0.end(), value);

    auto j = q.first;
    for ( auto i = p.first; i != p.second; ++i )
        ASSERT_EQ(i->name, (j++)->name);

    struct Comp {
        bool operator() ( const S& s, int i ) const { return s.number < i; }
        bool operator() ( int i, const S& s ) const { return i < s.number; }
    };

    const auto p2 = std::equal_range(vec.begin(),vec.end(), 2, Comp{});
    const auto q2 = jrSTL::equal_range(vec0.begin(),vec0.end(), 2, Comp{});

    auto j0 = q2.first;
    for ( auto i = p2.first; i != p2.second; ++i )
        ASSERT_EQ(i->name, (j0++)->name);
}

TEST(testCase, merge) {
    std::vector<int> a1 = {1,3,5,7,9}, a2 = {0,2,4,6}, a3(a1.size() + a2.size());
    jrSTL::vector<int> b1 = {1,3,5,7,9}, b2 = {0,2,4,6}, b3(b1.size() + b2.size());
    std::merge(a1.begin(), a1.end(), a2.begin(), a2.end(), a3.begin());
    jrSTL::merge(b1.begin(), b1.end(), b2.begin(), b2.end(), b3.begin());
    for(size_t i = 0; i < b3.size(); ++i)
        ASSERT_EQ(a3[i], b3[i]);
}

template<class Iter>
void merge_sort(Iter first, Iter last)
{
    if (last - first > 1) {
        Iter middle = first + (last - first) / 2;
        merge_sort(first, middle);
        merge_sort(middle, last);
        std::inplace_merge(first, middle, last);
    }
}

template<class Iter>
void merge_sort_j(Iter first, Iter last)
{
    if (last - first > 1) {
        Iter middle = first + (last - first) / 2;
        merge_sort_j(first, middle);
        merge_sort_j(middle, last);
        jrSTL::inplace_merge(first, middle, last);
    }
}

TEST(testCase, inplace_merge) {
    std::vector<int> v{8, 2, -2, 0, 11, 11, 1, 7, 3};
    jrSTL::vector<int> v0{8, 2, -2, 0, 11, 11, 1, 7, 3};
    merge_sort(v.begin(), v.end());
    merge_sort_j(v0.begin(), v0.end());
    for(size_t i = 0; i < v.size(); ++i)
        ASSERT_EQ(v[i], v0[i]);
}

TEST(testCase, includes) {
    const auto
    v1 = {'a', 'b', 'c', 'f', 'h', 'x'},
    v2 = {'a', 'b', 'c'},
    v3 = {'a', 'c'},
    v4 = {'a', 'a', 'b'},
    v5 = {'g'},
    v6 = {'a', 'c', 'g'},
    v7 = {'A', 'B', 'C'};

    auto no_case = [](char a, char b) { return std::tolower(a) < std::tolower(b); };

    ASSERT_EQ(std::includes(v1.begin(), v1.end(), v2.begin(), v2.end()),
              jrSTL::includes(v1.begin(), v1.end(), v2.begin(), v2.end()));

    ASSERT_EQ(std::includes(v1.begin(), v1.end(), v3.begin(), v3.end()),
              jrSTL::includes(v1.begin(), v1.end(), v3.begin(), v3.end()));

    ASSERT_EQ(std::includes(v1.begin(), v1.end(), v4.begin(), v4.end()),
              jrSTL::includes(v1.begin(), v1.end(), v4.begin(), v4.end()));

    ASSERT_EQ(std::includes(v1.begin(), v1.end(), v5.begin(), v5.end()),
              jrSTL::includes(v1.begin(), v1.end(), v5.begin(), v5.end()));

    ASSERT_EQ(std::includes(v1.begin(), v1.end(), v6.begin(), v6.end()),
              jrSTL::includes(v1.begin(), v1.end(), v6.begin(), v6.end()));

    ASSERT_EQ(std::includes(v1.begin(), v1.end(), v7.begin(), v7.end(), no_case),
              jrSTL::includes(v1.begin(), v1.end(), v7.begin(), v7.end(), no_case));
}

struct Order // 拥有某些有趣数据的类
{
    int order_id;

    bool operator==(const Order& r) const {
        return order_id == r.order_id;
    }

    bool operator!=(const Order& r) const {
        return order_id != r.order_id;
    }
};

template< class T >
bool is_vec_same(const std::vector<T>& a,
                 const jrSTL::vector<T>& b) {
    if(a.size() != b.size())
        return false;
    for(size_t i = 0; i < a.size(); ++i) {
        if(a[i] != b[i])
            return false;
    }
    return true;
}

TEST(testCase, difference) {
    const std::vector<int> v1 {1, 2, 5, 5, 5, 9};
    const std::vector<int> v2 {2, 5, 7};
    std::vector<int> diff;
    const jrSTL::vector<int> v1j {1, 2, 5, 5, 5, 9};
    const jrSTL::vector<int> v2j {2, 5, 7};
    jrSTL::vector<int> diffj;

    std::set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(),
                        std::inserter(diff, diff.begin()));
    jrSTL::set_difference(v1j.begin(), v1j.end(), v2j.begin(), v2j.end(),
                           jrSTL::inserter(diffj, diffj.begin()));

    ASSERT_EQ(is_vec_same(v1, v1j), true);
    ASSERT_EQ(is_vec_same(v2, v2j), true);

    std::vector<Order> old_orders { {1}, {2}, {5}, {9} };
    std::vector<Order> new_orders { {2}, {5}, {7} };
    std::vector<Order> changed_orders;
    jrSTL::vector<Order> old_ordersj { {1}, {2}, {5}, {9} };
    jrSTL::vector<Order> new_ordersj { {2}, {5}, {7} };
    jrSTL::vector<Order> changed_ordersj;

    std::set_difference(old_orders.begin(), old_orders.end(),
                        new_orders.begin(), new_orders.end(),
                        std::back_inserter(changed_orders),
                        [](const Order& a, const Order& b) { return a.order_id < b.order_id; });
    jrSTL::set_difference(old_ordersj.begin(), old_ordersj.end(),
                        new_ordersj.begin(), new_ordersj.end(),
                        jrSTL::back_inserter(changed_ordersj),
                        [](const Order& a, const Order& b) { return a.order_id < b.order_id; });

    ASSERT_EQ(is_vec_same(old_orders, old_ordersj), true);
    ASSERT_EQ(is_vec_same(new_orders, new_ordersj), true);
    ASSERT_EQ(is_vec_same(changed_orders, changed_ordersj), true);
}

TEST(testCase, intersection) {
    std::vector<int> v1{1,2,3,4,5,6,7,8};
    std::vector<int> v2{5,7,9,10};
    jrSTL::vector<int> v1j{1,2,3,4,5,6,7,8};
    jrSTL::vector<int> v2j{5,7,9,10};

    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());
    jrSTL::sort(v1j.begin(), v1j.end());
    jrSTL::sort(v2j.begin(), v2j.end());

    std::vector<int> v_intersection;
    jrSTL::vector<int> v_intersection0;

    std::set_intersection(v1.begin(), v1.end(),
                          v2.begin(), v2.end(),
                          std::back_inserter(v_intersection));
    jrSTL::set_intersection(v1j.begin(), v1j.end(),
                             v2j.begin(), v2j.end(),
                             jrSTL::back_inserter(v_intersection0));
    ASSERT_EQ(is_vec_same(v_intersection, v_intersection0), true);
}

TEST(testCase, set_symmetric_difference) {
    std::vector<int> v1{1,2,3,4,5,6,7,8};
    std::vector<int> v2{5,7,9,10};
    jrSTL::vector<int> v1j{1,2,3,4,5,6,7,8};
    jrSTL::vector<int> v2j{5,7,9,10};

    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());
    jrSTL::sort(v1j.begin(), v1j.end());
    jrSTL::sort(v2j.begin(), v2j.end());

    std::vector<int> v_symmetric;
    jrSTL::vector<int> v_symmetric0;

    std::set_symmetric_difference(v1.begin(), v1.end(),
                          v2.begin(), v2.end(),
                          std::back_inserter(v_symmetric));
    jrSTL::set_symmetric_difference(v1j.begin(), v1j.end(),
                             v2j.begin(), v2j.end(),
                             jrSTL::back_inserter(v_symmetric0));
    ASSERT_EQ(is_vec_same(v_symmetric, v_symmetric0), true);
}

TEST(testCase, set_union) {
    std::vector<int> v1 = {1, 2, 3, 4, 5};
    std::vector<int> v2 = {3, 4, 5, 6, 7};
    std::vector<int> dest1;
    jrSTL::vector<int> v1j = {1, 2, 3, 4, 5};
    jrSTL::vector<int> v2j = {3, 4, 5, 6, 7};
    jrSTL::vector<int> dest1j;

    std::set_union(v1.begin(), v1.end(),
                   v2.begin(), v2.end(),
                   std::back_inserter(dest1));
    jrSTL::set_union(v1j.begin(), v1j.end(),
                      v2j.begin(), v2j.end(),
                      jrSTL::back_inserter(dest1j));

    ASSERT_EQ(is_vec_same(dest1, dest1j), true);
}

TEST(testCase, is_permutation) {
    std::vector<int> v1{1,2,3,4,5};
    std::vector<int> v2{3,5,4,1,2};
    jrSTL::vector<int> v1j{1,2,3,4,5};
    jrSTL::vector<int> v2j{3,5,4,1,2};

    ASSERT_EQ(std::is_permutation(v1.begin(), v1.end(), v2.begin()),
              jrSTL::is_permutation(v1j.begin(), v1j.end(), v2j.begin()));

    std::vector<int> v3{3,5,4,1,1};
    jrSTL::vector<int> v3j{3,5,4,1,1};

    ASSERT_EQ(std::is_permutation(v1.begin(), v1.end(), v3.begin()),
              jrSTL::is_permutation(v1j.begin(), v1j.end(), v3j.begin()));
}

TEST(testCase, next_permutation) {
    std::vector<char> s = {'a', 'b', 'a'};
    jrSTL::vector<char> sj = {'a', 'b', 'a'};
    std::string r="", rj="";
    std::sort(s.begin(), s.end());
    jrSTL::sort(sj.begin(), sj.end());
    do {
        for(auto ch : s)
            r += ch;
    } while(std::next_permutation(s.begin(), s.end()));
    do {
        for(auto ch : sj)
            rj += ch;
    } while(jrSTL::next_permutation(sj.begin(), sj.end()));
    ASSERT_EQ(r, rj);
}

TEST(testCase, prev_permutation) {
    std::vector<char> s = {'a', 'b', 'c'};
    jrSTL::vector<char> sj = {'a', 'b', 'c'};
    std::string r="", rj="";
    std::sort(s.begin(), s.end(), std::greater<char>());
    jrSTL::sort(sj.begin(), sj.end(), jrSTL::greater<char>());
    do {
        for(auto ch : s)
            r += ch;
    } while(std::prev_permutation(s.begin(), s.end()));
    do {
        for(auto ch : sj)
            rj += ch;
    } while(jrSTL::prev_permutation(sj.begin(), sj.end()));
    ASSERT_EQ(r, rj);
}
