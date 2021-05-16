#ifndef JR_HASHTABLE_H
#define JR_HASHTABLE_H

#include <cstddef>
#include <functional>
#include "../../container/sequence/jr_vector.h"
#include "../../container/utils/jr_utility.h"
#include "se_iterators.h"

namespace jr_std {
template<class T, class Hash, class KeyEqual, class Allocator, bool isMulti>
class _hashtable{
    template<class U, class Ref, class Ptr,
             class T, class Hash, class KeyEqual,
             class Allocator, bool isMulti>
    friend struct _hashtable_iterator;

    template<class Key, class Hash, class Pred, class Allocator>
    friend class unordered_set;

private:
    typedef _hashtable_node<T> lnode;
    typename Allocator::template rebind<T>::other _alloc_data;
    typename Allocator::template rebind<lnode>::other _alloc_node;
    vector<lnode *, typename Allocator::template rebind<lnode>::other> table;

    lnode *_copy_list(lnode *h) {
        lnode *head = _alloc_node.allocate(1), *tmp = nullptr;
        tmp = head;
        for(lnode *n = h; n; n = n->next) {
            lnode *m = _alloc_node.allocate(1);
            _alloc_data.construct(&(m->data), n->data);
            tmp->next = m;
            tmp = tmp->next;
        }
        return head;
    }

    void _destroy_list(lnode *&m) {
        while(m) {
            lnode *tmp = m->next;
            _alloc_data.destroy(&(m->data));
            _alloc_node.dealocate(m, 1);
            m = tmp;
        }
    }

public:
    // 构造与析构
    _hashtable() = default;

    _hashtable(size_t size) : table(size) {
        for(size_t i = 0; i < table.size(); i++) {
            table[i] = _alloc_node.allocate(1);
        }
    }

    _hashtable(const _hashtable& x)
        : table(x.table.size()) {
        for(size_t i = 0; i < table.size(); i++) {
            _copy_list(x.table[i]);
        }
    }

    _hashtable(_hashtable&& x)
        : table(x.table.size()) {
        for(size_t i = 0; i < table.size(); i++) {
            table[i] = x.table[i];
            x.table[i] = _alloc_node.allocate(1);
        }
    }

    ~_hashtable() {
        for(lnode *&m : table) {
            _destroy_list(m);
        }
    }

    _hashtable& operator=(const _hashtable& x) {
        for(lnode *&m : table) {
            _destroy_list(m);
        }
        table.reserve(x.table.size());
        for(size_t i = 0; i < table.size(); i++) {
            _copy_list(x.table[i]);
        }
    }

    _hashtable& operator=(_hashtable&& x) {
        for(lnode *&m : table) {
            _destroy_list(m);
        }
        table.reserve(x.table.size());
        for(size_t i = 0; i < table.size(); i++) {
            table[i] = x.table[i];
            x.table[i] = _alloc_node.allocate(1);
        }
    }
    // 设置表长
    void set_length(size_t len) {
        bool isInit = table.empty();
        table.resize(len);
        if(isInit) {
            for(size_t i = 0; i < table.size(); i++) {
                table[i] = _alloc_node.allocate(1);
            }
        }
    }
    // 查找目标键
    pair<const int, lnode *> find(const T& target) {
        int hash_index = Hash(target);
        // 起始位置没有元素，或索引超出范围，说明该键值不存在，返回-1
        if((hash_index >= table.size()) || !table[hash_index]->hasElem)
            return pair<const int, lnode *>(-1, nullptr);
        lnode *m = table[hash_index];
        while(!KeyEqual(m->data, target))
            m = m->next;
        return pair<const int, lnode *>(hash_index, m);
    }
    // 插入新元素
    pair<const int, lnode *> insert(const T& a) {
        // 若不允许键值重复且该键值已存在，则什么都不做；否则，挂在哈希值对应索引的链表的尾部
        auto t = find(a);
        if(!isMulti && t.second)
            return pair<const int, lnode *>(-1, nullptr);
        else {
            int hash_index = Hash(a);
            // 插入元素数量超过原来的表长-1时，扩充表长(最后一个桶用于标记end迭代器)
            if(hash_index >= table.size() - 1) {
                table.resize(hash_index + 5);
            }
            lnode *m = table[hash_index], *n = nullptr;
            while(m->next)
                m = m->next;
            n = _alloc_node.allocate(1);
            _alloc_data.construct(&(n->data), a);
            table[hash_index]->hasElem = n->hasElem = true;
            m->next = n;
            return pair<const int, lnode *>(hash_index, n);
        }
    }

    pair<const int, lnode *> insert(T&& a) {
        auto t = find(a);
        if(!isMulti && t.second)
            return pair<const int, lnode *>(-1, nullptr);
        else {
            int hash_index = Hash(a);
            // 插入元素数量超过原来的表长-1时，扩充表长(最后一个桶用于标记end迭代器)
            if(hash_index >= table.size() - 1) {
                table.resize(hash_index + 5);
            }
            lnode *m = table[hash_index], *n = nullptr;
            while(m->next)
                m = m->next;
            n = _alloc_node.allocate(1);
            _alloc_data.construct(&(n->data), static_cast<T&&>(a));
            table[hash_index]->hasElem = n->hasElem = true;
            m->next = n;
            return pair<const int, lnode *>(hash_index, n);
        }
    }
    
    // 将元素插入到指定位置（hint）之后
    void insert_hint(lnode *hint, const T& a) {
        lnode *n = _alloc_node.allocate(1);
        _alloc_data.construct(&(n->data), a);
        n->next = hint->next;
        hint->next = n;
        
    }
    
    void insert_hint(lnode *hint, T&& a) {
        lnode *n = _alloc_node.allocate(1);
        _alloc_data.construct(&(n->data), static_cast<T&&>(a));
        n->next = hint->next;
        hint->next = n;
        
    }
    
    // 删除对应键值的所有元素(析构整条链表)
    pair<const int, lnode *> erase(T& target) {
        int hash_index = Hash(target);
        // 起始位置没有元素，或索引超出范围，说明该键值不存在，无需删除
        if((hash_index >= table.size()) || !table[hash_index]->hasElem)
            return pair<const int, lnode *>(-1, nullptr);
        lnode *m = table[hash_index]->next;
        while(m->next) {
            if(KeyEqual(m->next->data, target)) {
                lnode *tmp = m->next;
                m->next = m->next->next;
                _alloc_data.destroy(&(tmp->data));
                _alloc_node.dealocate(tmp, 1);
                break;
            }
            m = m->next;
        }
        return pair<const int, lnode *>(hash_index, m->next);
    }
};
}

#endif // JR_HASHTABLE_H
