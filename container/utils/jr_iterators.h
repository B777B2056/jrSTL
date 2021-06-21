#ifndef ITERATORS_H
#define ITERATORS_H

#include <iostream>
#include <cstddef>
#include "jr_nodes.h"
#include "../../iterator/jr_iterator.h"
#include "../../memory/jr_allocator.h"

namespace panzer {
    template<class U, class Ref, class Ptr>
    struct _forward_list_iterator {
        typedef U value_type;
        typedef Ptr pointer;
        typedef Ref reference;
        typedef ptrdiff_t difference_type;
        typedef forward_iterator_tag iterator_category;
        // point to current node
        _forward_node<U> *_cur_node;
        // construct iterator
        _forward_list_iterator() : _cur_node(nullptr) {}
        _forward_list_iterator(_forward_node<U> *x) : _cur_node(x) {}
        ~_forward_list_iterator() = default;
        // overloading ==, !=, *, ->, ++
        bool operator==(const _forward_list_iterator& x) const { return x._cur_node == _cur_node; }
        bool operator!=(const _forward_list_iterator& x) const { return x._cur_node != _cur_node; }
        reference operator*() const { return _cur_node->data; }
        pointer operator->() const { return &(operator*()); }
        // front postion ++
        _forward_list_iterator& operator++() {
           _cur_node = _cur_node->next;
           return *this;
        }
        // post position ++
        _forward_list_iterator operator++(int) {
           _forward_list_iterator tmp(this->_cur_node);
           _cur_node = _cur_node->next;
           return tmp;
        }
    };

    template<class U, class Ref, class Ptr>
    struct _list_iterator{
        typedef U value_type;
        typedef Ptr pointer;
        typedef Ref reference;
        typedef ptrdiff_t difference_type;
        typedef bidectional_iterator_tag iterator_category;
        // point to current node
        _node<U> *_cur_node;
        // construct iterator
        _list_iterator() : _cur_node(nullptr) {}
        _list_iterator(_node<U> *x) : _cur_node(x) {}
        ~_list_iterator() = default;
        // overloading ==, !=, *, ->, ++, --
        bool operator==(const _list_iterator& x) const { return x._cur_node == _cur_node; }
        bool operator!=(const _list_iterator& x) const { return x._cur_node != _cur_node; }
        reference operator*() const { return _cur_node->data; }
        pointer operator->() const { return &(operator*()); }
        // front postion ++
        _list_iterator& operator++() {
           _cur_node = _cur_node->next;
           return *this;
        }
        // post position ++
        _list_iterator operator++(int) {
           _list_iterator tmp(this->_cur_node);
           _cur_node = _cur_node->next;
           return tmp;
        }
        // front postion --
        _list_iterator& operator--() {
           _cur_node = _cur_node->prev;
           return *this;
        }
        // post position --
        _list_iterator operator--(int) {
           _list_iterator tmp(this->cur_node);
           _cur_node = _cur_node->prev;
           return tmp;
        }
    };

    template<class U, class Ref, class Ptr, size_t block_size>
    struct _deque_iterator {
        // 迭代器通用的类型定义
        typedef U value_type;
        typedef Ptr pointer;
        typedef Ref reference;
        typedef ptrdiff_t difference_type;
        typedef random_access_iterator_tag iterator_category;
        // deque迭代器特殊的定义
        typedef U** map_pointer;
        // 指示标志
        U *cur;  // 指向当前元素
        U *first;  // 指向当期存储区有效内容的头
        U *last;  // 指向存储区有效内容的尾
        map_pointer control_node;  // 指向管控当期存储区的节点
        // 构造函数
        _deque_iterator() :
            first(nullptr), cur(nullptr), last(nullptr),
            control_node(nullptr)
        {}
        _deque_iterator(map_pointer d) {
            jmp_node(d);
        }
        _deque_iterator(map_pointer d, U *c) {
            jmp_node(d);
            cur = c;
        }

        _deque_iterator(const _deque_iterator&) = default;

        _deque_iterator(_deque_iterator&& x) {
            cur = x.cur;
            first = x.first;
            last = x.last;
            control_node = x.control_node;
            x.cur = x.first = x.last = nullptr;
            x.control_node = nullptr;
        }
        // 析构函数
        ~_deque_iterator() = default;
        // 拷贝、移动运算符
        _deque_iterator& operator=(const _deque_iterator&) = default;

        _deque_iterator& operator=(_deque_iterator&& x) {
            if(this == &x)
                return *this;
            cur = x.cur;
            first = x.first;
            last = x.last;
            control_node = x.control_node;
            x.cur = x.first = x.last = nullptr;
            x.control_node = nullptr;
            return *this;
        }
        // 跳跃函数
        void jmp_node(map_pointer d) {
            control_node = d;
            first = *control_node;
            last = first + block_size;
            cur = first;
        }
        // 访问运算符重载（随机访问迭代器）
        reference operator*() const { return *cur; }
        pointer operator->() const { return &(operator*()); }
        // 迭代器间算数运算符运算符重载(两迭代器相减,计算的是两个迭代器间有多少个元素)
        difference_type operator-(const _deque_iterator& x) const {
            difference_type dis = 0;
            // 二者当前元素在同一段存储区内
            if(control_node == x.control_node)
                dis = cur - x.cur;
            // 二者当前元素不在同一段存储区内
            else if(control_node > x.control_node) {
                // 当前迭代器比x迭代器位置更靠后
                dis = (x.last - x.cur)
                    + (cur - first)
                    + block_size * (control_node - x.control_node - 1);
            } else {
                // 当前迭代器比x迭代器位置更靠前
                dis = (x.cur- x.first)
                    + (last - cur)
                    + block_size * (x.control_node - control_node - 1);
                dis = -dis;
            }
            return dis;
        }
        // 移位运算符重载
        _deque_iterator& operator++() {
            ++cur;
            // 当前节点自增后未落在当前区块范围内
            if(cur == last) {
                 // 进入下一区块，并更新标志节点
                jmp_node(++control_node);
            }
            return *this;
        }

        _deque_iterator operator++(int) {
            _deque_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        _deque_iterator& operator--() {
            if(cur == first) {
                jmp_node(--control_node);
                cur = last - 1;
            } else {
                --cur;
            }
            return *this;
        }

        _deque_iterator operator--(int) {
            _deque_iterator tmp = *this;
            --(*this);
            return tmp;
        }

        _deque_iterator& operator+=(difference_type n) {
            if(n > 0) {
                difference_type d = last - cur;
                if(n < d) {
                    cur += n;
                } else {
                    difference_type block_jmp = (n - d) / block_size + 1;
                    control_node += block_jmp;
                    first = control_node[0];
                    last = first + block_size;
                    cur = first + (n - d) % block_size;
                }
            } else if(n < 0) {
                n = -n;
                difference_type d = cur - first;
                if(d >= n) {
                    cur -= n;
                } else {
                    difference_type block_jmp = (n - d - 1) / block_size + 1;
                    control_node -= block_jmp;
                    first = control_node[0];
                    last = first + block_size;
                    cur = last - (n - d - (block_jmp - 1) * block_size);
                }
            }
            return *this;
        }

        _deque_iterator& operator-=(difference_type n) {
            *this += -n;
            return *this;
        }

        _deque_iterator operator+(difference_type n) const {
            _deque_iterator tmp = *this;
            tmp += n;
            return tmp;
        }

        _deque_iterator operator-(difference_type n) const {
            _deque_iterator tmp = *this;
            tmp -= n;
            return tmp;
        }
        // 随机存取运算符
        reference operator[](difference_type n) {
            return *(*this + n);
        }
        // 比较运算符
        bool operator==(const _deque_iterator& x) const {
            return cur == x.cur;
        }

        bool operator!=(const _deque_iterator& x) const {
            return !operator==(x);
        }

        bool operator<(const _deque_iterator& x) const {
            if(control_node == x.control_node)
                return cur < x.cur;
            else
                return control_node < x.control_node;
        }

        bool operator>=(const _deque_iterator& x) const {
            return !operator<(x);
        }

        bool operator>(const _deque_iterator& x) const {
            if(control_node == x.control_node)
                return cur > x.cur;
            else
                return control_node > x.control_node;
        }

        bool operator<=(const _deque_iterator& x) const {
            return !operator>(x);
        }
    };

    template<class U, class Ref, class Ptr>
    struct _balance_bst_iterator{
        // 迭代器通用的类型定义
        typedef U value_type;
        typedef Ptr pointer;
        typedef Ref reference;
        typedef ptrdiff_t difference_type;
        typedef bidectional_iterator_tag iterator_category;
        // 自己的特殊定义
        typedef _balance_bst_iterator iterator;
        typedef _tree_node<U> tnode;
        tnode *_node;
        tnode *_header;
        // 构造与析构
        _balance_bst_iterator() = delete;
        _balance_bst_iterator(tnode *x, tnode *y) : _node(x), _header(y) {}
        ~_balance_bst_iterator() = default;
        // overloading ==, !=, *, ->, ++, --
        bool operator==(const iterator& x) const { return _node == x._node; }
        bool operator!=(const iterator& x) const { return !(*this == x); }
        reference operator*() const { return _node->data; }
        pointer operator->() const { return &(*(*this)); }
        // front postion ++
        iterator& operator++() {
            if(_node->right) {
                // 若当前节点存在右子树，则下一位置为右子树最小值
                _node = _node->right;
                 while(_node && _node->left)
                     _node = _node->left;
            } else {
                // 若当前节点不存在右子树，
                // 则向上回溯，找到第一个比当前节点大的位置
               tnode *f = _node->parent;
               if(f == _header) {
                   // 若当前节点无父亲且无右子树，
                   // 则当前节点已是最后元素(且为根节点)，置节点为空
                   _node = _header;
               } else {
                   // 若当前节点有父亲且为父亲的右子树，
                   // 则向上回溯直至成为父亲的左子树
                   //（即找到让当前刚刚小于父亲的父节点），
                   // 让当前节点指向父亲
                   //（若当前节点为根节点则父亲为空，说明原来的位置已是最后元素，置节点为空）
                   while(f && (f != _header) && (_node == f->right)) {
                       _node = f;
                       f = f->parent;
                   }
                   _node = f;
               }
            }
            if(!_node) {
                _node = _header;
            }
            return *this;
        }
        // post position ++
        iterator operator++(int) {
           iterator tmp = *this;
           ++(*this);
           return tmp;
        }
        // front postion --
        iterator& operator--() {
            if(_node->left) {
                _node = _node->left;
                 while(_node && _node->right)
                     _node = _node->right;
            } else {
               tnode *f = _node->parent;
               if(f == _header) {
                   _node = nullptr;
               } else {
                   while(_node == f->left) {
                       _node = f;
                       f = f->parent;
                   }
                   _node = f;
               }
            }
            if(!_node) {
                _node = _header;
            }
            return *this;
        }
        // post position --
        iterator operator--(int) {
            iterator tmp = *this;
            --(*this);
            return tmp;
        }
    };

    template<class T1, class T2, class T3, class T4, bool isMulti>
    class _hashtable;

    template<class U, class Hash, class KeyEqual,
             class Allocator, bool isMulti>
    struct _hashtable_iterator{
        // 迭代器通用的类型定义
        typedef U value_type;
        typedef U* pointer;
        typedef U& reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef forward_iterator_tag iterator_category;
        // 自己特殊定义
        typedef _hashtable<U, Hash, KeyEqual, Allocator, isMulti> table;
        typedef _forward_list_iterator<U, const U&, const U*> hnode;
        size_type index;
        const table *tab;
        hnode cur;

        // construct iterator
        _hashtable_iterator(table *t, size_type o, hnode n)
            : tab(t), index(o), cur(n)
        {}

        _hashtable_iterator(const table *t, size_type o, hnode n)
            : tab(t), index(o), cur(n)
        {}

        ~_hashtable_iterator() = default;

        // overloading ==, !=, *, ->, ++
        bool operator==(const _hashtable_iterator& x) const {
            return cur == x.cur;
        }

        bool operator!=(const _hashtable_iterator& x) const {
            return !(*this == x);
        }

        reference operator*() const {
            return const_cast<reference>(*cur);
        }

        pointer operator->() const {
            return &operator*();
        }

        // front postion ++
        _hashtable_iterator& operator++() {
            ++cur;
            // 桶内无内容，则跳至下一个桶
            if(cur == tab->table[index].cend()) {
                ++index;
                while(index < tab->table.size() - 1) {
                    if(!tab->table[index].empty()){
                        cur = tab->table[index].cbegin();
                        break;
                    }
                    ++index;
                }
                // 到达end迭代器
                if(index == tab->table.size() - 1) {
                    cur = tab->table[index].cend();
                }
            }
            return *this;
        }

        // post position ++
        _hashtable_iterator operator++(int) {
            _hashtable_iterator tmp(*this);
            ++(*this);
            return tmp;
        }
    };
}

#endif // ITERATORS_H
