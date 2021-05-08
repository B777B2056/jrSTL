#ifndef ITERATORS_H
#define ITERATORS_H
#include <iostream>
#include <cstddef>
#include "../../iterator/jr_iterator.h"

namespace jr_std {
    template<class U>
    struct _forward_node {
        U data;
        _forward_node<U> *next;
        _forward_node() : next(nullptr) {}
        _forward_node(const U& val) : data(val), next(nullptr) {}
    };

    template<class U>
    struct _node {
        U data;
        _node<U> *prev, *next;
        _node() : prev(nullptr), next(nullptr) {}
        _node(const U& val) : data(val), prev(nullptr), next(nullptr) {}
    };

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
        // 析构函数
        ~_deque_iterator() = default;
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
                difference_type d = last - cur - 1;
                if(n <= d) {
                    cur += n;
                } else {
                    difference_type block_jmp = (n - d) / block_size + 1;
                    control_node += block_jmp;
                    first = *control_node;
                    last = first + block_size;
                    cur = first + (n - d - 1) % block_size;
                }
            } else if(n < 0) {
                n = -n;
                difference_type d = cur - first;
                if(d >= n) {
                    cur -= n;
                } else {
                    difference_type block_jmp = (n - d - 1) / block_size + 1;
                    control_node -= block_jmp;
                    first = *control_node;
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

}
#endif // ITERATORS_H
