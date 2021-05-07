#ifndef JR_LIST_H
#define JR_LIST_H

#include <type_traits>
#include <cstddef>
#include "../../memory/jr_allocator.h"
#include "../utils/se_iterators.h"

namespace jr_std {
    template<class T, class Allocator = allocator<T> >
    class list {
    public:
        typedef _list_iterator<T, T&, T*> iterator;
        typedef _list_iterator<T, const T&, const T*> const_iterator;

    protected:
        Allocator _alloc;
        // Use same rule with type T to build a list node allocator
        typename Allocator::template rebind<_node<T> >::other _alloc_node;
        _node<T> *_head, *_tail;
        size_t _size;
        // Create empty list
        void _create_empty_node() {
            _head = _create_node();
            _tail = _create_node();
            _head->next = _tail;
            _tail->prev = _head;
        }
        // Create a list node without value
        _node<T> *_create_node() {
            _node<T> *node = _alloc_node.allocate(1);
            return node;
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

        void _ctor(size_t n, const T& value, const Allocator& a, std::true_type) {
            _alloc = a;
            _create_empty_node();
            while(n--)
                _insert2tail(value);
        }

        template<class InputIt>
        void _ctor(InputIt first, InputIt last, const Allocator& a, std::false_type) {
            _size = 0;
            _alloc = a;
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
            iterator it = begin();
            difference_type dis = distance(cbegin(), position);
            if(!n) {
                advance(it, dis);
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
                --position;
            }
            advance(it, dis);
            return it;
        }

        template<class InputIt>
        iterator _insert(const_iterator position, InputIt first, InputIt last, std::false_type) {
            iterator it = begin();
            difference_type dis = distance(cbegin(), position);
            if(first == last) {
                advance(it, dis);
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
            advance(it, dis);
            return it;
        }

        template<class Compare>
        _node<T> *_merge_sorted_list(_node<T> *l1_h, _node<T> *l2_h, Compare comp) {
            if(!l1_h)   return l2_h;
            if(!l2_h)   return l1_h;
            _node<T> *dummy = _create_node();
            _node<T> *n1, *n2;
            if(comp(l1_h->data, l2_h->data)) {
                n1 = l1_h;
                n2 = l2_h;
            } else {
                n1 = l2_h;
                n2 = l1_h;
            }
            _node<T> *n3 = nullptr;
            _node<T> *n4 = nullptr;
            _node<T> *n5 = nullptr;
            _node<T> *last = nullptr;
            _node<T> *last2 = nullptr;
            n1->prev = dummy;
            dummy->next = n1;
            while(n1 && n2) {
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
            if(n1) {
                last2->next = n1;
                n1->prev = last2;
            }
            if(n2) {
                last->next = n2;
                n2->prev = last;
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
            return _merge_sorted_list(h, next, comp);
        }

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

        explicit list() : _size(0) {
           _create_empty_node();
        }

        explicit list( size_type n ) : _size(n) {
            _create_empty_node();
            while(n--) { _insert2tail(T()); }
        }

        list(size_type n, const_reference value, const Allocator& a = Allocator()) : _size(n) {
            _ctor(n, value, a, std::true_type());
        }

        template<class InputIt>
        list(InputIt first, InputIt last, const Allocator& a = Allocator()) {
            typedef std::integral_constant<bool, std::is_integral<InputIt>::value> type;
            _ctor(first, last, a, type());
        }

        list(const list& x) : _size(x._size) {
            if(this == &x)  return;
            _create_empty_node();
            for(_node<T> *t=x._head->next; t!=x._tail; t=t->next)
                _insert2tail(t->data);
        }

        list(list&& x) {
            if(this == &x)  return;
            _size = x._size;
            _head = x._head;
            _tail = x._tail;
            x._size = 0;
            x._head = nullptr;
            x._tail = nullptr;
            x._create_empty_node();
        }

        ~list() {
            _node<T> *tmp;
            while(_head) {
                tmp = _head->next;
                _destroy_node(_head);
                _head = tmp;
            }
        }

        list& operator=(const list& x) {
            if(this != &x) {
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

        allocator_type get_allocator() const noexcept { return _alloc; }

        iterator begin() noexcept { return iterator(_head->next); }
        const_iterator begin() const noexcept { return const_iterator(_head->next); }

        const_iterator cbegin() noexcept { return const_iterator(_head->next); }
        iterator end() noexcept { return iterator(_tail); }
        const_iterator end() const noexcept { return const_iterator(_tail); }
        const_iterator cend() noexcept { return const_iterator(_tail); }

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
            advance(it, distance(cbegin(), position));
            return it;
        }

        iterator insert(const_iterator position, size_type n, const_reference x) {
            return _insert(position, n, x, std::true_type());
        }

        iterator insert(const_iterator position, const_reference x) {
            return insert(position, 1, x);
        }

        iterator insert(const_iterator position, T&& x) {
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
            advance(it, distance(cbegin(), position));
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
            _node<T> *n1, *n2, *flag, *flag2, *last, *last2;
            if(comp(_head->data, x._head->data)) {
                n1 = _head->next;
                n2 = x._head->next;
                flag = _tail;
                flag2 = x._tail;
            } else {
                n1 = x._head->next;
                n2 = _head->next;
                flag = x._tail;
                flag2 = _tail;
            }
            _node<T> *n3 = nullptr;
            _node<T> *n4 = nullptr;
            _node<T> *n5 = nullptr;
            while((n1 != flag) && (n2 != flag2)) {
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
                    n4->next = n5;
                    n5->prev = n4;
                    n2 = n5;
                }
            }
            if(n1 != flag) {
                last2->next = n1;
                n1->prev = last2;
            }
            if(n2 != flag2) {
                last->next = n2;
                n2->prev = last;
                n2 = flag2->prev;
                n2->next = flag;
                flag->prev = n2;
            }
            if(flag != _tail) {
                this->swap(x);
            }
            _size += x._size;
            x._size = 0;
            x._head->next = x._tail;
            x._tail->prev = x._head;
        }

        template<class Compare>
        void merge(list&& x, Compare comp) {
            if(this == &x) return;
            _node<T> *n1, *n2, *flag, *flag2, *last, *last2;
            if(comp(_head->data, x._head->data)) {
                n1 = _head->next;
                n2 = x._head->next;
                flag = _tail;
                flag2 = x._tail;
            } else {
                n1 = x._head->next;
                n2 = _head->next;
                flag = x._tail;
                flag2 = _tail;
            }
            _node<T> *n3 = nullptr;
            _node<T> *n4 = nullptr;
            _node<T> *n5 = nullptr;
            while((n1 != flag) && (n2 != flag2)) {
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
                    n4->next = n5;
                    n5->prev = n4;
                    n2 = n5;
                }
            }
            if(n1 != flag) {
                last2->next = n1;
                n1->prev = last2;
            }
            if(n2 != flag2) {
                last->next = n2;
                n2->prev = last;
                n2 = flag2->prev;
                n2->next = flag;
                flag->prev = n2;
            }
            if(flag != _tail) {
                this->swap(x);
            }
            _size += x._size;
            x._size = 0;
            x._head->next = x._tail;
            x._tail->prev = x._head;
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
            if(_size < 2)  return;
            _node<T> *pre = nullptr, *cur = nullptr, *next = nullptr;
            for(cur = _head; cur; cur = next) {
                next = cur->next;
                cur->prev = cur->next;
                cur->next = pre;
                pre = cur;
            }
            _node<T> *tmp = _head;
            _head = _tail;
            _tail = tmp;
        }
    };

    template<class T, class Allocator>
    void swap(list<T, Allocator>& x, list<T, Allocator>& y)
    noexcept(noexcept(x.swap(y))) {
        x.swap(y);
    }

    template<class T, class Allocator>
    bool operator==(const list<T, Allocator>& x, const list<T, Allocator>& y) {
        if(x.size() != y.size())
            return false;
        typename list<T, Allocator>::iterator itx = x.begin();
        typename list<T, Allocator>::iterator ity = y.begin();
        while((itx != x.end()) && (ity != y.end())) {
            if(*itx != *ity)
                return false;
            ++itx;
            ++ity;
        }
        return true;
    }
}

#endif // JR_LIST_H
