#ifndef JR_LIST_H
#define JR_LIST_H

#include <iostream>
#include <type_traits>
#include <cstddef>
#include "../../memory/jr_allocator.h"
#include "../utils/jr_iterators.h"

namespace jr_std {
    template<class T, class Allocator = allocator<T> >
    class list {
    public:
        // type define
        typedef T value_type;
        typedef ptrdiff_t difference_type;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_reference;
        typedef size_t size_type;
        typedef Allocator allocator_type;
        typedef _list_iterator<T, T&, T*> iterator;
        typedef _list_iterator<T, const T&, const T*> const_iterator;
        typedef jr_std::reverse_iterator<const_iterator> const_reverse_iterator;
        typedef jr_std::reverse_iterator<iterator> reverse_iterator;

    private:
        Allocator _alloc;
        // Use same rule with type T to build a list node allocator
        typename Allocator::template rebind<_node<T> >::other _alloc_node;
        _node<T> *_head, *_tail;
        size_t _size;
        // Create a list node without value
        _node<T> *_create_node() {
            return _alloc_node.allocate(1);
        }
        // Create a list node with value
        _node<T> *_create_node(const T& value) {
            _node<T> *node = _alloc_node.allocate(1);
            _alloc_node.construct(&(node->data), value);
            return node;
        }

        _node<T> *_create_node(T&& value) {
            _node<T> *node = _alloc_node.allocate(1);
            _alloc_node.construct(&(node->data), static_cast<T&&>(value));
            return node;
        }
        // Create empty list
        void _create_empty_node() {
            _head = _create_node();
            _tail = _create_node();
            _head->next = _tail;
            _tail->prev = _head;
        }
        // Destroy a list node
        void _destroy_node(_node<T> *node) {
            _alloc_node.destroy(&(node->data));
            _alloc_node.deallocate(node, 1);
        }
        // Insert a node into list head
        void _insert2head(const T& value) {
            _node<T> *node = _create_node(value), *tmp = _head->next;
            _head->next = node;
            node->prev = _head;
            node->next = tmp;
            tmp->prev = node;
        }

        void _insert2head(T&& value) {
            _node<T> *node = _create_node(static_cast<T&&>(value));
            _node<T> *tmp = _head->next;
            _head->next = node;
            node->prev = _head;
            node->next = tmp;
            tmp->prev = node;
        }
        // Delete a node from list tail
        void _delete4head() {
            _node<T> *n1 = _head->next, *n2 = _head->next->next;
            n2->prev= _head;
            _head->next = n2;
            _destroy_node(n1);
        }

        // Insert a node into list tail
        void _insert2tail(const T& value) {
            _node<T> *node = _create_node(value);
            _node<T> *tmp = _tail->prev;
            tmp->next = node;
            node->prev = tmp;
            _tail->prev = node;
            node->next = _tail;
        }

        void _insert2tail(T&& value) {
            _node<T> *node = _create_node(static_cast<T&&>(value));
            _node<T> *tmp = _tail->prev;
            tmp->next = node;
            node->prev = tmp;
            _tail->prev = node;
            node->next = _tail;
        }
        // Delete a node from list tail
        void _delete4tail() {
            _node<T> *n1 = _tail->prev, *n2 = _tail->prev->prev;
            n2->next = _tail;
            _tail->prev = n2;
            _destroy_node(n1);
        }

        void _ctor(size_t n, const T& value,
                   std::true_type) {
            if(_size != n)
                _size = n;
            _create_empty_node();
            while(n--)
                _insert2tail(value);
        }

        template<class InputIt>
        void _ctor(InputIt first, InputIt last, std::false_type) {
            _size = 0;
            _create_empty_node();
            while(first != last) {
                _insert2tail(first._cur_node->data);
                ++first;
                ++_size;
            }
        }

        void _assign(size_t n, const T& t, std::true_type) {
            _size = n;
            _node<T> *tmp = _head->next;
            while(n && (tmp != _tail)) {
                _alloc_node.destroy(&(tmp->data));
                tmp->data = t;
                tmp = tmp->next;
                --n;
            }
            while(n--) {
                _insert2tail(t);
            }
        }

        template<class InputIt>
        void _assign(InputIt first, InputIt last, std::false_type) {
            _size = 0;
            _node<T> *tmp = _head->next;
            while((first != last) && (tmp != _tail)) {
                _alloc_node.destroy(&(tmp->data));
                tmp->data = *first;
                tmp = tmp->next;
                ++first;
                ++_size;
            }
            while(first != last) {
                _insert2tail(*first);
                ++first;
                ++_size;
            }
        }

        iterator _insert(const_iterator position, size_t n, const T& x, std::true_type) {
            difference_type dis = jr_std::distance(cbegin(), position);
            if(!n) {
                iterator it = begin();
                jr_std::advance(it, dis);
                return it;
            }
            _size += n;
            _node<T> *n1 = position._cur_node;
            _node<T> *n2 = position._cur_node->prev;
            while(n--) {
                _node<T> *node = _create_node(x);
                n2->next = node;
                node->next = n1;
                n1->prev = node;
                node->prev = n2;
                n2 = n2->next;
            }
            iterator it = begin();
            jr_std::advance(it, dis);
            return it;
        }

        template<class InputIt>
        iterator _insert(const_iterator position, InputIt first, InputIt last, std::false_type) {
            difference_type dis = jr_std::distance(cbegin(), position);
            if(first == last) {
                iterator it = begin();
                jr_std::advance(it, dis);
                return it;
            }
            difference_type n = distance(first, last);
            _node<T> *n1 = position._cur_node;
            _node<T> *n2 = position._cur_node->prev;
            _size += n;
            while(first != last) {
                _node<T> *node = _create_node(*first);
                node->next = n1;
                node->prev = n2;
                n1->prev = node;
                n2->next = node;
                n2 = n2->next;
                ++first;
            }
            iterator it = begin();
            jr_std::advance(it, dis);
            return it;
        }

        template<class Compare>
        _node<T> *_merge_list(_node<T> *l1_h, _node<T> *l2_h,
                              _node<T> *t1, _node<T> *t2,
                              Compare comp, bool& is_change) {
            if(!l1_h)
                return l2_h;
            if(!l2_h)
                return l1_h;
            _node<T> *dummy = _create_node();
            _node<T> *n1, *n2;
            if(comp(l1_h->data, l2_h->data)) {
                n1 = l1_h;
                n2 = l2_h;
                is_change = false;
            } else {
                n1 = l2_h;
                n2 = l1_h;
                _node<T> *tmp = t1;
                t1 = t2;
                t2 = tmp;
                is_change = true;
            }
            _node<T> *n3 = nullptr;
            _node<T> *n4 = nullptr;
            _node<T> *n5 = nullptr;
            _node<T> *last = nullptr;
            _node<T> *last2 = nullptr;
            n1->prev = dummy;
            dummy->next = n1;
            while((n1 != t1) && (n2 != t2)) {
                last = n1;
                last2 = n2;
                if(comp(n1->data, n2->data)) {
                    n1 = n1->next;
                }else{
                    n3 = n1->prev;
                    n4 = n2->prev;
                    n5 = n2->next;
                    n3->next = n2;
                    n2->prev = n3;
                    n2->next = n1;
                    n1->prev = n2;
                    if(n4)
                        n4->next = n5;
                    if(n5)
                        n5->prev = n4;
                    n2 = n5;
                }
            }
            if((n1 != t1) && last2) {
                last2->next = n1;
                n1->prev = last2;
            }
            if((n2 != t2) && last) {
                last->next = n2;
                n2->prev = last;
                if(t2) {
                    n2 = t2->prev;
                    n2->next = t1;
                    t1->prev = n2;
                }
            }
            n1 = dummy->next;
            n1->prev = dummy->next = nullptr;
            _destroy_node(dummy);
            return n1;
        }

        template<class Compare>
        _node<T> *_merge_sort(_node<T> *h, Compare comp) {
            if(!h || !h->next)
                return h;
            bool is_change;
            // Find center postion node(Double-pointer method)
            _node<T> *next, *slow, *fast;
            next = nullptr;
            slow = fast = h;
            while(fast &&
                  fast->next &&
                  fast->next->next) {
                slow = slow->next;
                fast = fast->next->next;
            }
            next = slow->next;
            // Cut to two parts from center node
            slow->next = nullptr;
            next->prev = nullptr;
            // Sort left list
            h = _merge_sort(h, comp);
            // Sort right list
            next = _merge_sort(next, comp);
            // Merge two parts list
            return _merge_list(h, next,
                                      nullptr, nullptr,
                                      comp, is_change);
        }

    public:
        explicit list() : _size(0) {
           _create_empty_node();
        }

        explicit list( const Allocator& a )
            : _size(0), _alloc(a) {
            _create_empty_node();
        }

        explicit list( size_type n )
            : _size(n) {
            _create_empty_node();
            while(n--) {
                _insert2tail(T());
            }
        }

        list(size_type n, const_reference value,
             const Allocator& a = Allocator())
            : _size(n), _alloc(a) {
            _ctor(n, value, std::true_type());
        }

        template<class InputIt>
        list(InputIt first, InputIt last,
             const Allocator& a = Allocator())
            : _alloc(a) {
            typedef std::integral_constant<bool, std::is_integral<InputIt>::value> type;
            _ctor(first, last, type());
        }

        list(const list& x)
            : _size(x._size) {
            if(this == &x)
                return;
            _create_empty_node();
            for(_node<T> *t=x._head->next; t!=x._tail; t=t->next)
                _insert2tail(t->data);
        }

        list( const list& x, const Allocator& a )
            : _alloc(a) {
            if(this == &x)
                return;
            _create_empty_node();
            for(_node<T> *t=x._head->next; t!=x._tail; t=t->next)
                _insert2tail(t->data);
        }

        list(list&& x) {
            if(this == &x)
                return;
            _size = x._size;
            _head = x._head;
            _tail = x._tail;
            x._size = 0;
            x._head = nullptr;
            x._tail = nullptr;
            x._create_empty_node();
        }

        list( list&& x, const Allocator& a )
            : _alloc(a) {
            if(this == &x)
                return;
            _size = x._size;
            _head = x._head;
            _tail = x._tail;
            x._size = 0;
            x._head = nullptr;
            x._tail = nullptr;
            x._create_empty_node();
        }

        list( std::initializer_list<T> init,
              const Allocator& a = Allocator() )
            : list(a) {
            for(auto it = init.begin(); it != init.end(); ++it)
                push_back(*it);
        }

        ~list() {
            while(_head != _tail) {
                _node<T> *tmp = _head->next;
                _destroy_node(_head);
                _head = tmp;
            }
            _destroy_node(_tail);
        }

        list& operator=(const list& x) {
            if(this != &x) {
                clear();
                _node<T> *tmp, *th = _head->next;
                while(th && th != _tail) {
                    tmp = th->next;
                    _destroy_node(th);
                    th = tmp;
                }
                for(_node<T> *tmp = x._head->next;
                    tmp != x._tail;
                    tmp = tmp->next) {
                    _insert2tail(tmp->data);
                }
                _size = x._size;
            }
            return *this;
        }

        list& operator=(list&& other) {
            if(this != &other) {
                clear();
                _size = other._size;
                _head = other._head;
                _tail = other._tail;
                other._size = 0;
                other._head = nullptr;
                other._tail = nullptr;
                other._create_empty_node();
            }
            return *this;
        }

        template<class InputIt>
        void assign(InputIt first, InputIt last) {
            clear();
            typedef std::integral_constant<bool, std::is_integral<InputIt>::value> type;
            _assign(first, last, type());
        }

        void assign(size_type n, const_reference t) {
            clear();
            _assign(n, t, std::true_type());
        }

        void assign( std::initializer_list<T> ilist ) {
            clear();
            for(auto it = ilist.begin(); it != ilist.end(); ++it)
                push_back(*it);
        }

        allocator_type get_allocator() const noexcept { return _alloc; }

        iterator begin() noexcept { return iterator(_head->next); }
        const_iterator begin() const noexcept { return const_iterator(_head->next); }

        const_iterator cbegin() noexcept { return const_iterator(_head->next); }
        iterator end() noexcept { return iterator(_tail); }
        const_iterator end() const noexcept { return const_iterator(_tail); }
        const_iterator cend() noexcept { return const_iterator(_tail); }

        reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

        const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }

        const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }

        reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

        const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

        const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }

        bool empty() const noexcept { return _size == 0; }
        size_type size() const noexcept { return _size; }
        size_type max_size() const noexcept { return UINT_MAX; }

        void resize(size_type sz, const_reference c) {
            if(_size > sz) {
                while(_size > sz) {
                    _delete4tail();
                    --_size;
                }
            }else if(_size < sz) {
                while(_size < sz) {
                    _insert2tail(c);
                    ++_size;
                }
            }
        }

        void resize(size_type sz) { resize(sz, T()); }

        inline reference front() { return _head->next->data; }
        inline const_reference front() const { return _head->next->data; }
        inline reference back() { return _tail->prev->data; }
        inline const_reference back() const { return _tail->prev->data; }

        void push_front(const_reference x) {
            ++_size;
            _insert2head(x);
        }

        void push_front(T&& x) {
            ++_size;
            _insert2head(static_cast<T&&>(x));
        }

        void pop_front() {
            --_size;
            _delete4head();
        }

        void push_back(const_reference x) {
            ++_size;
            _insert2tail(x);
        }

        void push_back(T&& x) {
            ++_size;
            _insert2tail(static_cast<T&&>(x));
        }

        void pop_back() {
            --_size;
            _delete4tail();
        }

        template<class... Args>
        void emplace_front(Args&&... args) {
            ++_size;
            T *p;
            p = _alloc.allocate(1);
            _alloc.construct(p, static_cast<Args&&>(args)...);
            _insert2head(*p);
        }

        template<class... Args>
        void emplace_back(Args&&... args) {
            ++_size;
            T *p;
            p = _alloc.allocate(1);
            _alloc.construct(p, static_cast<Args&&>(args)...);
            _insert2tail(*p);
        }

        template<class... Args>
        iterator emplace(const_iterator position, Args&&... args) {
            difference_type dis = jr_std::distance(cbegin(), position);
            _node<T> *n1 = position._cur_node;
            _node<T> *n2 = position._cur_node->prev;
            _node<T> *n = _create_node();
            _alloc_node.construct(&(n->data), static_cast<Args&&>(args)...);
            n2->next = n;
            n->next = n1;
            n1->prev = n;
            n->prev = n2;
            --position;
            ++_size;
            iterator it = begin();
            jr_std::advance(it, dis);
            return it;
        }

        iterator insert(const_iterator position, size_type n, const_reference x) {
            return _insert(position, n, x, std::true_type());
        }

        iterator insert(const_iterator position, const_reference x) {
            return insert(position, 1, x);
        }

        iterator insert(const_iterator position, T&& x) {
            difference_type dis = jr_std::distance(cbegin(), position);
            _node<T> *n1 = position._cur_node;
            _node<T> *n2 = position._cur_node->prev;
            _node<T> *node = _create_node(static_cast<T&&>(x));
            n2->next = node;
            node->next = n1;
            n1->prev = node;
            node->prev = n2;
            --position;
            ++_size;
            iterator it = begin();
            jr_std::advance(it, dis);
            return it;
        }

        iterator insert( const_iterator pos, std::initializer_list<T> ilist ) {
            difference_type dis = jr_std::distance(cbegin(), pos);
            for(auto it = ilist.begin(); it != ilist.end(); ++it)
                insert(pos, *it);
            iterator it = begin();
            jr_std::advance(it, dis);
            return it;
        }

        template<class InputIt>
        iterator insert(const_iterator position, InputIt first, InputIt last) {
            typedef std::integral_constant<bool, std::is_integral<InputIt>::value> type;
            return _insert(position, first, last, type());
        }

        iterator erase(const_iterator position) {
            --_size;
            _node<T> *n1 = position._cur_node;
            _node<T> *n2 = n1->prev;
            _node<T> *n3 = n1->next;
            n2->next = n3;
            n3->prev = n2;
            _destroy_node(n1);
            return iterator(n3);
        }

        iterator erase(const_iterator first, const_iterator last) {
            iterator it = begin();
            advance(it, distance(cbegin(), last));
            if(first == last)   return it;
            const_iterator tmp = first, tmp_n;
            while(tmp != last) {
                tmp_n = tmp;
                ++tmp;
                erase(tmp_n);
            }
            return it;
        }

        void swap(list& x) noexcept {
            if(this == &x)
                return;
            _node<T> *tmp;
            size_type t;
            {
                tmp = _head;
                _head = x._head;
                x._head = tmp;
            }
            {
                tmp = _tail;
                _tail = x._tail;
                x._tail = tmp;
            }
            {
                t = _size;
                _size = x._size;
                x._size = t;
            }
        }

        void clear() noexcept { erase(cbegin(), cend()); }

        void splice(const_iterator position, list& x,
                    const_iterator first, const_iterator last) {
            _node<T> *n1 = position._cur_node;
            _node<T> *n2 = position._cur_node->prev;
            _node<T> *n3 = first._cur_node;
            _node<T> *n4 = last._cur_node->prev;
            _node<T> *n5 = first._cur_node->prev;
            difference_type dis = distance(first, last);
            _size += dis;
            x._size -= dis;
            n2->next = n3;
            n3->prev = n2;
            n4->next = n1;
            n1->prev = n4;
            n5->next = last._cur_node;
            last._cur_node->prev = n5;
        }

        void splice(const_iterator position, list&& x,
                    const_iterator first, const_iterator last) {
            _node<T> *n1 = position._cur_node;
            _node<T> *n2 = position._cur_node->prev;
            _node<T> *n3 = first._cur_node;
            _node<T> *n4 = last._cur_node->prev;
            _node<T> *n5 = first._cur_node->prev;
            difference_type dis = distance(first, last);
            _size += dis;
            x._size -= dis;
            n2->next = n3;
            n3->prev = n2;
            n4->next = n1;
            n1->prev = n4;
            n5->next = last._cur_node;
            last._cur_node->prev = n5;
        }

        void splice(const_iterator position, list& x) {
            splice(position, x, x.cbegin(), x.cend());
        }

        void splice(const_iterator position, list&& x) {
            splice(position, static_cast<list&&>(x), x.cbegin(), x.cend());
        }

        void splice(const_iterator position, list& x, const_iterator it) {
            const_iterator it0 = it;
            it0 = next(it0);
            splice(position, x, it, it0);
        }

        void splice(const_iterator position, list&& x, const_iterator it) {
            const_iterator it0 = it;
            it0 = next(it0);
            splice(position, static_cast<T&&>(x), it, it0);
        }

        template<class Predicate>
        void remove_if(Predicate pred) {
            for(const_iterator it = cbegin(); it != cend(); ++it) {
                if(pred(*it))
                    erase(it);
            }
        }

        void remove(const_reference value) {
            remove_if([value](const T& a)->bool { return value == a;});
        }

        template<class BinaryPredicate>
        void unique(BinaryPredicate binary_pred) {
            _node<T> *t1 = _tail->prev, *t2 = _head;
            while(t1 != t2) {
                if(t1 &&
                   t1->prev &&
                   binary_pred(t1->data, t1->prev->data)) {
                    _node<T> *n1 = t1;
                    _node<T> *n2 = n1->prev;
                    _node<T> *n3 = n2->prev;
                    --_size;
                    n1->prev = n3;
                    n3->next = n1;
                    _destroy_node(n2);
                }else
                    t1 = t1->prev;
            }
        }

        void unique() {
            unique([=](const T& a, const T& b)->bool { return a == b; });
        }

        template<class Compare>
        void merge(list& x, Compare comp) {
            if(this == &x) return;
            bool is_change;
            _node<T> *h1 = _head->next, *h2 = x._head->next;
            h1->prev = _head->next = nullptr;
            h2->prev = x._head->next = nullptr;
            h1 = _merge_list(h1, h2, _tail, x._tail,
                                     comp, is_change);
            _head->next = h1;
            h1->prev = _head;
            if(is_change) {
                _node<T> *tmp = _tail;
                _tail = x._tail;
                x._tail = tmp;
            }
            x._head->next = x._tail;
            x._tail->prev = x._head;
            _size += x._size;
            x._size = 0;
        }

        template<class Compare>
        void merge(list&& x, Compare comp) {
            if(this == &x) return;
            bool is_change;
            _node<T> *h1 = _head->next, *h2 = x._head->next;
            h1->prev = _head->next = nullptr;
            h2->prev = x._head->next = nullptr;
            h1 = _merge_list(h1, h2,
                                    _tail, x._tail,
                                    comp, is_change);
            _head->next = h1;
            h1->prev = _head;
            if(is_change) {
                _node<T> *tmp = _tail;
                _tail = x._tail;
                x._tail = tmp;
            }
            x._head->next = x._tail;
            x._tail->prev = x._head;
            _size += x._size;
            x._size = 0;
        }

        void merge(list& x) {
            merge(x, [=](const T& a, const T& b)->bool {return a < b;});
        }
        void merge(list&& x) {
            merge(static_cast<list&&>(x), [=](const T& a, const T& b)->bool {return a < b;});
        }

        // Use merge sort, stable, O(nlogn)
        template<class Compare>
        void sort(Compare comp) {
            if(_size < 2)
                return;
            // Sort
            _node<T> *_h = _head->next;
            _head->next = nullptr;
            _h->prev = nullptr;
            _node<T> *_m = _tail->prev;
            _m->next = nullptr;
            _tail->prev = nullptr;
            _h = _merge_sort(_h, comp);
            _head->next = _h;
            _h->prev = _head;
            while(_h && _h->next)
                _h = _h->next;
            _h->next = _tail;
            _tail->prev = _h;
        }

        void sort() {
            sort([=](const T& a, const T& b)->bool { return a < b; });
        }

        void reverse() noexcept {
            if(_size < 2)
                return;
            _node<T> *pre = nullptr, *cur = nullptr, *next = nullptr;
            for(cur = _head->next; cur != _tail; cur = next) {
                next = cur->next;
                pre = cur->prev;
                cur->prev = cur->next;
                cur->next = pre;
            }
            _head->prev = _head->next;
            _head->next = nullptr;
            _tail->next = _tail->prev;
            _tail->prev = nullptr;
            _node<T> *tmp = _head;
            _head = _tail;
            _tail = tmp;
        }
    };

    template<class T, class Allocator>
    void swap(list<T, Allocator>& x, list<T, Allocator>& y)
    noexcept(noexcept(x.swap(y)))
    { x.swap(y); }

    template<class T, class Allocator>
    bool operator==(const list<T, Allocator>& lhs,
                    const list<T, Allocator>& rhs) {
        size_t lsz = lhs.size(), rsz = rhs.size();
        if(lsz != rsz)
            return false;
        auto lit = lhs.begin();
        auto rit = rhs.begin();
        while((lit != lhs.end()) && (rit != rhs.end())) {
            if(*lit != *rit)
                return false;
            ++lit;
            ++rit;
        }
        return true;
    }

    template< class T, class Alloc >
    bool operator!=( const jr_std::list<T,Alloc>& lhs,
                     const jr_std::list<T,Alloc>& rhs )
    { return !(lhs == rhs); }

    template< class T, class Alloc >
    bool operator<( const jr_std::list<T,Alloc>& lhs,
                    const jr_std::list<T,Alloc>& rhs ) {
        size_t lsz = lhs.size(), rsz = rhs.size();
        if(lsz < rsz)
            return true;
        if(lsz > rsz)
            return false;
        auto lit = lhs.begin();
        auto rit = rhs.begin();
        while((lit != lhs.end()) && (rit != rhs.end())) {
            if(*lit >= *rit)
                return false;
            ++lit;
            ++rit;
        }
        return true;
    }

    template< class T, class Alloc >
    bool operator>( const jr_std::list<T,Alloc>& lhs,
                    const jr_std::list<T,Alloc>& rhs ) {
        size_t lsz = lhs.size(), rsz = rhs.size();
        if(lsz < rsz)
            return false;
        if(lsz > rsz)
            return true;
        auto lit = lhs.begin();
        auto rit = rhs.begin();
        while((lit != lhs.end()) && (rit != rhs.end())) {
            if(*lit <= *rit)
                return false;
            ++lit;
            ++rit;
        }
        return true;
    }

    template< class T, class Alloc >
    bool operator<=( const jr_std::list<T,Alloc>& lhs,
                     const jr_std::list<T,Alloc>& rhs )
    { return !(lhs > rhs); }

    template< class T, class Alloc >
    bool operator>=( const jr_std::list<T,Alloc>& lhs,
                     const jr_std::list<T,Alloc>& rhs )
    { return !(lhs < rhs); }
}

#endif // JR_LIST_H
