#ifndef JR_HASHTABLE_H
#define JR_HASHTABLE_H

#include <cstddef>
#include <utility>
#include "../sequence/jr_vector.h"
#include "../sequence/jr_forward_list.h"
#include "jr_nodes.h"

namespace jrSTL {
template<class T, class HashFun, class KeyEqualFun, class Allocator, bool isMulti>
class _hashtable{
    template<class U, class T1, class T2, class T3, bool a>
    friend struct _hashtable_iterator;

    template<class U, class T1, class T2, class T3, bool a>
    friend class _hashset_base;

    template<class U1, class U2, class T1, class T2, class T3, bool a>
    friend class _hashmap_base;

private:
    typedef typename forward_list<T>::const_iterator hnode;
    HashFun Hash;
    KeyEqualFun KeyEqual;
    vector<forward_list<T, Allocator>,
           typename Allocator::template rebind<forward_list<T, Allocator> >::other> table;

public:
    // 构造与析构
    _hashtable() = default;

    _hashtable(size_t size,
               const HashFun& h = HashFun(),
               const KeyEqualFun& k = KeyEqualFun())
        : Hash(h), KeyEqual(k) {
        for(size_t i = 0; i < size; i++) {
            table.push_back(forward_list<T>());
        }
    }

    _hashtable(const _hashtable& x)
        : Hash(x.Hash), KeyEqual(x.KeyEqual) {
        for(size_t i = 0; i < x.table.size(); i++) {
            table.push_back(forward_list<T>(x.table[i]));
        }
    }

    _hashtable(_hashtable&& x)
        : Hash(x.Hash), KeyEqual(x.KeyEqual) {
        for(size_t i = 0; i < x.table.size(); i++) {
            table.push_back(std::move(forward_list<T>(std::move(x.table[i]))));
        }
    }

    ~_hashtable() {}

    _hashtable& operator=(const _hashtable& x) {
        if(this == &x)
            return *this;
        Hash = x.Hash;
        KeyEqual = x.KeyEqual;
        for(auto n : table)
            n.clear();
        table.clear();
        for(size_t i = 0; i < x.table.size(); i++) {
            table.push_back(forward_list<T>(x.table[i]));
        }
        return *this;
    }

    _hashtable& operator=(_hashtable&& x) {
        if(this == &x)
            return *this;
        Hash = x.Hash;
        KeyEqual = x.KeyEqual;
        for(auto n : table)
            n.clear();
        table.clear();
        for(size_t i = 0; i < x.table.size(); i++) {
            table.push_back(std::move(forward_list<T>(std::move(x.table[i]))));
        }
        return *this;
    }

    // 设置表长
    void set_length(size_t len) {
        while(table.size() < len) {
            table.push_back(forward_list<T>());
        }
    }

    // 查找目标键
    std::pair<const int, hnode> find(const T& target) {
        const int hash_index = Hash(target);
        // 起始位置没有元素，或索引超出范围，说明该键值不存在，返回-1
        if((hash_index >= table.size() - 1)
         || table[hash_index].empty())
            return std::pair<const int, hnode>(-1, table.back().cend());
        hnode m = table[hash_index].cbegin();
        while(m != table[hash_index].cend() && !KeyEqual(*m, target))
            ++m;
        if(m == table[hash_index].cend())
            return std::pair<const int, hnode>(-1, m);
        else
            return std::pair<const int, hnode>(hash_index, m);
    }

    // 元素计数器
    size_t count(const T& k) {
        size_t cnt = 0;
        const int hash_index = Hash(k);
        // 起始位置没有元素，或索引超出范围，说明该键值不存在，返回0
        if((hash_index >= table.size() - 1)
         || table[hash_index].empty())
            return 0;
        hnode m = table[hash_index].cbegin();
        while(m != table[hash_index].cend()) {
            if(KeyEqual(*m, k))
                ++cnt;
            ++m;
        }
        return cnt;
    }

    // 插入新元素
    std::pair<const int, hnode> insert(const T& a, bool& flag) {
        // 若不允许键值重复且该键值已存在，则什么都不做；否则，挂在哈希值对应索引的链表的头部
        auto t = find(a);
        if(!isMulti && (t.first != -1)) {
            flag = false;
            return t;
        }
        else {
            flag = true;
            int hash_index = Hash(a);
            // 插入元素所需桶的数量超过原来的表长-1时，扩充表长(最后一个桶用于标记end迭代器)
//            while(hash_index >= table.size() - 1) {
//                table.push_back(forward_list<T>());
//            }
            set_length(hash_index + 5);
            // 直接在链表头插入
            table[hash_index].push_front(a);
            return std::pair<const int, hnode>(hash_index,
                                               table[hash_index].cbegin());
        }
    }

    std::pair<const int, hnode> insert(T&& a, bool& flag) {
        auto t = find(a);
        if(!isMulti && (t.first != -1)) {
            flag = false;
            return t;
        }
        else {
            flag = true;
            int hash_index = Hash(a);
            // 插入元素所需桶的数量超过原来的表长-1时，扩充表长(最后一个桶用于标记end迭代器)
            while(hash_index >= table.size() - 1) {
                table.push_back(forward_list<T>());
            }
            // 直接在链表头插入
            table[hash_index].push_front(static_cast<T&&>(a));
            return std::pair<const int, hnode>(hash_index,
                                               table[hash_index].cbegin());
        }
    }
    
    // 将元素插入到指定位置（hint）之后
    hnode insert_hint(size_t index, hnode hint,
                      const T& a, bool& flag) {
        auto t = find(a);
        if(!isMulti && (t.first != -1)) {
            flag = false;
            return t.second;
        }
        flag = true;
        table[index].insert_after(hint, a);
        return ++hint;
    }
    
    hnode insert_hint(size_t index, hnode hint,
                      T&& a, bool& flag) {
        auto t = find(a);
        if(!isMulti && (t.first != -1)) {
            flag = false;
            return t.second;
        }
        flag = true;
        table[index].insert_after(hint,
                                  static_cast<T&&>(a));
        return ++hint;
    }
    
    // 删除指定位置的元素
    void erase(size_t hash_index, hnode n) {
        hnode pre, m = table[hash_index].cbefore_begin();
        while(m != n) {
            pre = m++;
        }
        table[hash_index].erase_after(pre);
    }

    // 删除对应键值的所有元素
    size_t erase(const T& target) {
        size_t cnt = 0;
        int hash_index = Hash(target);
        // 该键值不存在，无需删除
        if((hash_index >= table.size() - 1)
         || table[hash_index].empty()) {
            return 0;
        }
        hnode pre = table[hash_index].cbefore_begin();
        hnode m = table[hash_index].cbegin();
        while(m != table[hash_index].cend()) {
            if(KeyEqual(*m, target)) {
                table[hash_index].erase_after(pre);
                ++cnt;
                if(!isMulti)
                    break;
                m = pre;
            } else {
                pre = m;
            }
            ++m;
        }
        return cnt;
    }
};
}

#endif // JR_HASHTABLE_H
