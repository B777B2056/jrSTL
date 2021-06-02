#ifndef JR_HASHTABLE_H
#define JR_HASHTABLE_H

#include <cstddef>
#include <utility>
#include <functional>
#include "../../container/sequence/jr_vector.h"
#include "se_iterators.h"

namespace jr_std {
template<class T, class HashFun, class KeyEqualFun, class Allocator, bool isMulti>
class _hashtable{
    template<class U, class T1, class T2, class T3, bool a>
    friend struct _hashtable_iterator;

    template<class U, class T1, class T2, class T3, bool a>
    friend class _hashset_base;

    template<class U1, class U2, class T1, class T2, class T3, bool a>
    friend class _hashmap_base;

private:
    typedef _hashtable_node<T> hnode;
    HashFun Hash;
    KeyEqualFun KeyEqual;
    typename Allocator::template rebind<T>::other _alloc_data;
    typename Allocator::template rebind<hnode>::other _alloc_node;
    vector<hnode *, typename Allocator::template rebind<hnode *>::other> table;

    hnode *_copy_list(hnode *h) {
        hnode *head = _alloc_node.allocate(1);
        hnode *tmp = head;
        for(hnode *n = h; n; n = n->next) {
            hnode *m = _alloc_node.allocate(1);
            _alloc_data.construct(&(m->data), n->data);
            m->next = tmp->next;
            tmp->next = m;
            tmp = tmp->next;
        }
        return head;
    }

    void _destroy_list(hnode *m) {
        while(m) {
            hnode *tmp = m->next;
            _alloc_data.destroy(&(m->data));
            _alloc_node.deallocate(m, 1);
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
            table[i] = _copy_list(x.table[i]);
            table[i]->hasElem = x.table[i]->hasElem;
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
        for(size_t i = 0; i < table.size(); i++) {
            _destroy_list(table[i]);
            table[i] = nullptr;
        }
    }

    _hashtable& operator=(const _hashtable& x) {
        if(this == &x)  return *this;
        table.resize(x.table.size());
        for(size_t i = 0; i < table.size(); i++) {
            if(table[i] && table[i]->next)
                _destroy_list(table[i]->next);
            table[i] = _copy_list(x.table[i]->next);
            table[i]->hasElem = x.table[i]->hasElem;
        }
        return *this;
    }

    _hashtable& operator=(_hashtable&& x) {
        if(this == &x)  return *this;
        table.resize(x.table.size());
        for(size_t i = 0; i < table.size(); i++) {
            if(table[i] && table[i]->next)
                _destroy_list(table[i]->next);
            table[i] = x.table[i];
            x.table[i] = _alloc_node.allocate(1);
        }
        return *this;
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
    std::pair<const int, hnode *> find(const T& target) {
        int hash_index = Hash(target);
        // 起始位置没有元素，或索引超出范围，说明该键值不存在，返回-1
        if((hash_index >= table.size()) || !table[hash_index]->hasElem)
            return std::pair<const int, hnode *>(-1, nullptr);
        hnode *m = table[hash_index];
        while(m && !KeyEqual(m->data, target))
            m = m->next;
        return std::pair<const int, hnode *>(hash_index, m);
    }
    // 元素计数器
    size_t count(const T& k) {
        size_t cnt = 0;
        int hash_index = Hash(k);
        // 起始位置没有元素，或索引超出范围，说明该键值不存在，返回0
        if((hash_index >= table.size()) || !table[hash_index]->hasElem)
            return 0;
        hnode *m = table[hash_index];
        while(m) {
            if(KeyEqual(m->data, k))
                ++cnt;
            m = m->next;
        }
        return cnt;
    }
    // 插入新元素
    std::pair<const int, hnode *> insert(const T& a) {
        // 若不允许键值重复且该键值已存在，则什么都不做；否则，挂在哈希值对应索引的链表的头部
        auto t = find(a);
        if(!isMulti && t.second)
            return t;
        else {
            int hash_index = Hash(a);
            // 插入元素所需桶的数量超过原来的表长-1时，扩充表长(最后一个桶用于标记end迭代器)
            if(hash_index >= table.size() - 1) {
                table.resize(hash_index + 5);
                for(size_t i = 0; i < table.size(); i++) {
                    if(!table[i])
                        table[i] = _alloc_node.allocate(1);
                }
            }
            // 直接在链表头插入，节省插入时间
            hnode *n = _alloc_node.allocate(1);
            _alloc_data.construct(&(n->data), a);
            n->next = table[hash_index]->next;
            table[hash_index]->next = n;
            table[hash_index]->hasElem = n->hasElem = true;
            return std::pair<const int, hnode *>(hash_index, n);
        }
    }

    std::pair<const int, hnode *> insert(T&& a) {
        auto t = find(a);
        if(!isMulti && t.second)
            return t;
        else {
            int hash_index = Hash(a);
            // 插入元素所需桶的数量超过原来的表长-1时，扩充表长(最后一个桶用于标记end迭代器)
            if(hash_index >= table.size() - 1) {
                table.resize(hash_index + 5);
                for(size_t i = 0; i < table.size(); i++) {
                    if(!table[i])
                        table[i] = _alloc_node.allocate(1);
                }
            }
            // 直接在链表头插入，节省插入时间
            hnode *n = _alloc_node.allocate(1);
            _alloc_data.construct(&(n->data), static_cast<T&&>(a));
            n->next = table[hash_index]->next;
            table[hash_index]->next = n;
            table[hash_index]->hasElem = n->hasElem = true;
            return std::pair<const int, hnode *>(hash_index, n);
        }
    }
    
    // 将元素插入到指定位置（hint）之后
    hnode *insert_hint(hnode *hint, const T& a) {
        auto t = find(a);
        if(!isMulti && t.second)
            return nullptr;
        hnode *n = _alloc_node.allocate(1);
        _alloc_data.construct(&(n->data), a);
        n->next = hint->next;
        hint->next = n;
        return hint;
    }
    
    hnode *insert_hint(hnode *hint, T&& a) {
        auto t = find(a);
        if(!isMulti && t.second)
            return nullptr;
        hnode *n = _alloc_node.allocate(1);
        _alloc_data.construct(&(n->data), static_cast<T&&>(a));
        n->next = hint->next;
        hint->next = n;
        return hint;
    }
    
    // 删除指定位置的元素
    void erase(size_t hash_index, hnode *n) {
        // 起始位置没有元素，或索引超出范围，说明该键值不存在，无需删除
        if((hash_index >= table.size() - 1) || !table[hash_index]->hasElem) {
            return;
        }
        hnode *m = table[hash_index];
        for(; m && m->next && (m->next != n); m = m->next);
        if(!m->next)
            return;
        hnode *tmp = m->next;
        m->next = m->next->next;
        _alloc_data.destroy(&(tmp->data));
        _alloc_node.deallocate(tmp, 1);
        if(!table[hash_index]->next)
            table[hash_index]->hasElem = false;
    }

    // 删除对应键值的所有元素
    size_t erase(const T& target) {
        size_t cnt = 0;
        int hash_index = Hash(target);
        // 起始位置没有元素，或索引超出范围，说明该键值不存在，无需删除
        if((hash_index >= table.size() - 1) || !table[hash_index]->hasElem) {
            return 0;
        }
        hnode *m = table[hash_index];
        while(m && m->next) {
            if(KeyEqual(m->next->data, target)) {
                hnode *tmp = m->next;
                m->next = m->next->next;
                _alloc_data.destroy(&(tmp->data));
                _alloc_node.deallocate(tmp, 1);
                ++cnt;
            }else {
                m = m->next;
            }
        }
        if(!table[hash_index]->next)
            table[hash_index]->hasElem = false;
        return cnt;
    }
};
}

#endif // JR_HASHTABLE_H
