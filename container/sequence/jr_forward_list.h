#ifndef JR_FORWARD_LIST_H
#define JR_FORWARD_LIST_H

#include <cstddef>
#include <type_traits>
#include "../../memory/jr_allocator.h"
#include "../utils/se_iterators.h"

namespace jr_std {
template<class T, class Allocator = jr_std::allocator<T> >
  class forward_list {
  public:
    // 类型
    typedef T value_type;
    typedef Allocator allocator_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef _forward_list_iterator<T, T&, T*> iterator;
    typedef _forward_list_iterator<T, const T&, const T*> const_iterator;

  protected:
     _forward_node<T> *_head, *_tail;
     Allocator _alloc;
     typename Allocator::template rebind<_forward_node<T> >::other _alloc_node;

     void _create_empty_list() {
         _head = _alloc_node.allocate(1);
         _tail = _alloc_node.allocate(1);
         _head->next = _tail;
         _tail->next = nullptr;
     }

     void _insert2front(_forward_node<T> **h, const T& value) {
         _forward_node<T> *node = _alloc_node.allocate(1);
         _alloc.construct(&(node->data), value);
         node->next = (*h)->next;
         (*h)->next = node;
     }

     void _insert2front(_forward_node<T> **h, T&& value) {
         _forward_node<T> *node = _alloc_node.allocate(1);
         _alloc.construct(&(node->data), static_cast<T&&>(value));
         node->next = (*h)->next;
         (*h)->next = node;
     }

     void _build_n(size_type n, const T& value, std::true_type) {
         while(n--)
             _insert2front(&_head, value);
     }

     template<class InputIt>
     void _build_n(InputIt first, InputIt last, std::false_type) {
         _forward_node<T> *t = _head;
         while(first != last) {
             _forward_node<T> *node = _alloc_node.allocate(1);
             _alloc.construct(&(node->data), *first);
             node->next = t->next;
             t->next = node;
             t = t->next;
             ++first;
         }
     }

     iterator _insert(const_iterator position, size_type n, const T& x, std::true_type) {
         if(!n)
             return iterator(position._cur_node);
         _forward_node<T> *t = position._cur_node;
         while(n--) {
             _forward_node<T> *node = _alloc_node.allocate(1);
             _alloc.construct(&(node->data), x);
             node->next = t->next;
             t->next = node;
             t = t->next;
         }
         return iterator(t);
     }

     template<class InputIt>
     iterator _insert(const_iterator position, InputIt first, InputIt last, std::false_type) {
         if(first == last)
             return iterator(position._cur_node);
         _forward_node<T> *t = position._cur_node;
         while(first != last) {
             _forward_node<T> *node = _alloc_node.allocate(1);
             _alloc.construct(&(node->data), *first);
             node->next = t->next;
             t->next = node;
             t = t->next;
             ++first;
         }
         return iterator(t);
     }

     template<class Compare>
     _forward_node<T> *_merge_list(_forward_node<T> *h1, _forward_node<T> *h2,
                                   _forward_node<T> *end1, _forward_node<T> *end2,
                                   Compare comp, bool& is_change) {
         if(!h1)    return h2;
         if(!h2)    return h1;
         _forward_node<T> *l1, *l2, *dummy1 = nullptr, *dummy2 = nullptr;
         dummy1 = _alloc_node.allocate(1);
         dummy2 = _alloc_node.allocate(1);
         if(comp(h1->data, h2->data)) {
             l1 = h1;
             l2 = h2;
             is_change = false;
         } else{
             l2 = h1;
             l1 = h2;
             _forward_node<T> *tmp = end1;
             end1 = end2;
             end2 = tmp;
             is_change = true;
         }
         dummy1->next = l1;
         l1 = dummy1;
         dummy2->next = l2;
         l2 = dummy2;
         while((l1->next != end1) && comp(l1->next->data, l2->next->data)) {
             l1 = l1->next;
         }
         while((l1->next != end1) && (l2->next != end2)) {
             if(comp(l1->next->data, l2->next->data)) {
                 l1 = l1->next;
             }else{
                 _forward_node<T> *tmp = l2->next;
                 l2->next = l2->next->next;
                 tmp->next = l1->next;
                 l1->next = tmp;
             }
         }
         if(l1->next != end1) {
             l2->next = l1->next;
         }else if(l2->next != end2) {
             l1->next = l2->next;
             if(end2) {
                 while(l2->next != end2)
                     l2 = l2->next;
                 l2->next = end1;
             }
         }
         l1 = dummy1->next;
         dummy1->next = nullptr;
         _alloc_node.deallocate(dummy1, 1);
         _alloc_node.deallocate(dummy2, 1);
         return l1;
     }

     template<class Compare>
     _forward_node<T> *_merge_sort(_forward_node<T> *h, Compare comp) {
         if(!h || !h->next)
             return h;
         bool is_change; // 与排序无关，为了接口统一而妥协的多余参数(与merge成员有关)
         _forward_node<T> *slow = h, *fast = h, *new_h = nullptr;
         while(fast && fast->next && fast->next->next) {
             slow = slow->next;
             fast = fast->next->next;
         }
         new_h = slow->next;
         slow->next = nullptr;
         h = _merge_sort(h, comp);
         new_h = _merge_sort(new_h, comp);
         return _merge_list(h, new_h, nullptr, nullptr, comp, is_change);
     }

  public:
    // 构造/复制/销毁
    forward_list() {
        _create_empty_list();
    }

    explicit forward_list(const Allocator& a)
        : _alloc(a){
        _create_empty_list();
    }

    forward_list(size_type n, const T& value, const Allocator& a = Allocator())
        : _alloc(a) {
        _create_empty_list();
        _build_n(n, value, std::true_type());
    }

    template<class InputIt>
    forward_list(InputIt first, InputIt last, const Allocator& a = Allocator())
        : _alloc(a){
        _create_empty_list();
        typedef std::integral_constant<bool, std::is_integral<InputIt>::value> type;
        _build_n(first, last, type());
    }

    forward_list(const forward_list& x) {
        if(this == &x)
            return;
        _create_empty_list();
        _forward_node<T> *pre = _head, *cur = nullptr;
        for(auto it = x.begin(); it != x.end(); ++it) {
            cur = _alloc_node.allocate(1);
            _alloc.construct(&(cur->data), *it);
            cur->next = pre->next;
            pre->next = cur;
            pre = pre->next;
        }
    }

    forward_list(forward_list&& x) {
        if(this == &x)
            return;
        _head = x._head;
        _tail = x._tail;
        x._create_empty_list();
    }

    forward_list(const forward_list& x, const Allocator& a)
        : _alloc(a) {
        if(this == &x)
            return;
        _create_empty_list();
        _forward_node<T> *pre = _head, *cur = nullptr;
        for(auto it = x.begin(); it != x.end(); ++it) {
            cur = _alloc_node.allocate(1);
            _alloc.construct(&(cur->data), *it);
            cur->next = pre->next;
            pre->next = cur;
            pre = pre->next;
        }
    }

    forward_list(forward_list&& x, const Allocator& a)
        : _alloc(a) {
        if(this == &x)
            return;
        _head = x._head;
        _tail = x._tail;
        x._create_empty_list();
    }

    forward_list( std::initializer_list<T> init,
                  const Allocator& a = Allocator() )
        : forward_list(a) {
        _forward_node<T> *h = _head;
        for(auto it = init.begin(); it != init.end(); ++it) {
            _forward_node<T> *node = _alloc_node.allocate(1);
            _alloc.construct(&(node->data), *it);
            node->next = h->next;
            h->next = node;
            h = h->next;
        }
    }

    ~forward_list() {
        while(_head != _tail) {
            _forward_node<T> *t = _head->next;
            _alloc.destroy(&(_head->data));
            _alloc_node.deallocate(_head, 1);
            _head = t;
        }
        _alloc_node.deallocate(_tail, 1);
    }

    forward_list& operator=(const forward_list& x) {
        if(this == &x)
            return *this;
        _create_empty_list();
        _forward_node<T> *pre = _head, *cur = nullptr;
        for(auto it = x.begin(); it != x.end(); ++it) {
            cur = _alloc_node.allocate(1);
            _alloc.construct(&(cur->data), *it);
            cur->next = pre->next;
            pre->next = cur;
            pre = pre->next;
        }
        return *this;
    }

    forward_list& operator=(forward_list&& x) {
        if(this == &x)
            return *this;
        _head = x._head;
        _tail = x._tail;
        x._create_empty_list();
        return *this;
    }

    template<class InputIt>
    void assign(InputIt first, InputIt last) {
        clear();
        typedef std::integral_constant<bool, std::is_integral<InputIt>::value> type;
        _build_n(first, last, type());
    }

    void assign( std::initializer_list<T> ilist ) {
        clear();
        _forward_node<T> *h = _head;
        for(auto it = ilist.begin(); it != ilist.end(); ++it) {
            _forward_node<T> *node = _alloc_node.allocate(1);
            _alloc.construct(&(node->data), *it);
            node->next = h->next;
            h->next = node;
            h = h->next;
        }
    }

    void assign(size_type n, const T& value) {
        clear();
        _build_n(n, value, std::true_type());
    }

    allocator_type get_allocator() const noexcept {
        return _alloc;
    }

    // 迭代器
    iterator before_begin() noexcept {
        return iterator(_head);
    }

    const_iterator before_begin() const noexcept {
        return const_iterator(_head);
    }

    iterator begin() noexcept {
        return iterator(_head->next);
    }

    const_iterator begin() const noexcept {
        return const_iterator(_head->next);
    }

    iterator end() noexcept {
        return iterator(_tail);
    }

    const_iterator end() const noexcept {
        return const_iterator(_tail);
    }

    const_iterator cbegin() const noexcept {
        return const_iterator(_head->next);
    }

    const_iterator cbefore_begin() const noexcept {
        return const_iterator(_head);
    }

    const_iterator cend() const noexcept {
        return const_iterator(_tail);
    }

    // 容量
    bool empty() const noexcept {
        return _head->next == _tail;
    }

    size_type max_size() const noexcept {
        return UINT_MAX;
    }

    // 元素访问
    reference front() {
        return _head->next->data;
    }

    const_reference front() const {
        return _head->next->data;
    }

    // 修改器
    template<class... Args>
    iterator emplace_after(const_iterator position, Args&&... args) {
        _forward_node<T> *h = position._cur_node, *node = _alloc_node.allocate(1);
        _alloc.construct(&(node->data), static_cast<Args&&>(args)...);
        node->next = h->next;
        h->next = node;
        return iterator(node);
    }

    iterator insert_after(const_iterator position, const T& x) {
        _forward_node<T> *h = position._cur_node, *node = _alloc_node.allocate(1);
        _alloc.construct(&(node->data), x);
        node->next = h->next;
        h->next = node;
        return iterator(node);
    }

    iterator insert_after(const_iterator position, T&& x) {
        _forward_node<T> *h = position._cur_node, *node = _alloc_node.allocate(1);
        _alloc.construct(&(node->data), static_cast<T&&>(x));
        node->next = h->next;
        h->next = node;
        return iterator(node);
    }

    iterator insert_after(const_iterator position, size_type n, const T& x) {
        return _insert(position, n, x, std::true_type());
    }

    iterator insert_after( const_iterator pos, std::initializer_list<T> ilist ) {
        for(auto it = ilist.begin(); it != ilist.end(); ++it) {
            insert_after(pos, *it);
            ++pos;
        }
        iterator it = before_begin();
        jr_std::advance(it, jr_std::distance(cbefore_begin(), pos));
        return it;
    }

    template<class InputIt>
    iterator insert_after(const_iterator position, InputIt first, InputIt last) {
        typedef std::integral_constant<bool, std::is_integral<InputIt>::value> type;
        return _insert(position, first, last, type());
    }

    iterator erase_after(const_iterator first, const_iterator last) {
        if(first == last || first._cur_node->next == last._cur_node)
            return iterator(first._cur_node);
        _forward_node<T> *t = first._cur_node, *m = t->next, *p = nullptr;
        t->next = last._cur_node;
        while(m != last._cur_node) {
            p = m->next;
            _alloc.destroy(&(m->data));
            _alloc_node.deallocate(m, 1);
            m = p;
        }
        return iterator(last._cur_node);
    }

    iterator erase_after(const_iterator position) {
        if(position == cbegin())    return begin();
        const_iterator addtwo = position;
        jr_std::advance(addtwo, 2);
        return erase_after(position, addtwo);
    }

    template<class... Args>
    void emplace_front(Args&&... args) {
        emplace_after(cbefore_begin(), static_cast<Args&&>(args)...);
    }

    void push_front(const T& x) {
        _insert2front(&_head, x);
    }

    void push_front(T&& x) {
        _insert2front(&_head, static_cast<T&&>(x));
    }

    void pop_front() {
        erase_after(cbefore_begin());
    }

    void swap(forward_list& other) {
        if(this == &other)
            return;
        _forward_node<T> *tmp = nullptr;
        {
            tmp = _head;
            _head = other._head;
            other._head = tmp;
        }
        {
            tmp = _tail;
            _tail = other._tail;
            other._tail = tmp;
        }
    }

    void resize(size_type sz, const value_type& c) {
        size_type len = 0;
        _forward_node<T> *t = _head->next, *last = nullptr;
        while(t != _tail) {
            ++len;
            if(len == sz) break;
            last = t;
            t = t->next;
        }
        if(len < sz) {
            // 当期链表长度 < sz
            sz -= len;
            while(sz--) {
                _forward_node<T> *node = _alloc_node.allocate(1);
                _alloc.construct(&(node->data), c);
                node->next = last->next;
                last->next = node;
                last = last->next;
            }
        } else {
            // 当期链表长度 >= sz
            _forward_node<T> *m = t->next, *p = nullptr;
            t->next = _tail;
            while(m && m != _tail) {
                p = m->next;
                _alloc.destroy(&(m->data));
                _alloc_node.deallocate(m, 1);
                m = p;
            }
        }
    }

    void resize(size_type sz) {
        resize(sz, T());
    }

    void clear() noexcept {
        _forward_node<T> *t = nullptr, *th = _head->next;
        while(th != _tail) {
            t = th->next;
            _alloc.destroy(&(th->data));
            _alloc_node.deallocate(th, 1);
            th = t;
        }
        _head->next = _tail;
    }

    // forward_­list 操作
    void splice_after(const_iterator position, forward_list& x,
                      const_iterator first, const_iterator last) {
        if((position == cend()) || (this == &x))
            return;
        _forward_node<T> *x_last = first._cur_node, *t = position._cur_node->next;
        while(x_last->next != last._cur_node)
            x_last = x_last->next;
        position._cur_node->next = first._cur_node->next;
        first._cur_node->next = last._cur_node;
        x_last->next = t;
    }

    void splice_after(const_iterator position, forward_list&& x,
                      const_iterator first, const_iterator last) {
        if((position == cend()) || (this == &x))  return;
        _forward_node<T> *x_last = first._cur_node, *t = position._cur_node->next;
        while(x_last->next != last._cur_node)
            x_last = x_last->next;
        position._cur_node->next = first._cur_node->next;
        first._cur_node->next = last._cur_node;
        x_last->next = t;
    }

    void splice_after(const_iterator position, forward_list& x) {
        splice_after(position, x, x.cbefore_begin(), x.cend());
    }

    void splice_after(const_iterator position, forward_list&& x) {
        splice_after(position, static_cast<forward_list&&>(x),
                     x.cbefore_begin(), x.cend());
    }

    void splice_after(const_iterator position,
                      forward_list& x, const_iterator it) {
        splice_after(position, x, it, x.cend());
    }

    void splice_after(const_iterator position,
                      forward_list&& x, const_iterator it) {
        splice_after(position, static_cast<forward_list&&>(x),
                     it, x.cend());
    }

    template<class Predicate>
    void remove_if(Predicate pred) {
        _forward_node<T> *t = _head;
        while(t->next != _tail) {
            if(pred(t->next->data)) {
                _forward_node<T> *m = t->next;
                t->next = t->next->next;
                _alloc.destroy(&(m->data));
                _alloc_node.deallocate(m, 1);
            }else {
                t = t->next;
            }
        }
    }

    template<class BinaryPredicate>
    void unique(BinaryPredicate binary_pred) {
        _forward_node<T> *t = _head;
        while(t->next != _tail) {
            if(binary_pred(t->data, t->next->data)) {
                _forward_node<T> *m = t->next;
                t->next = t->next->next;
                _alloc.destroy(&(m->data));
                _alloc_node.deallocate(m, 1);
            }else {
                t = t->next;
            }
        }
    }

    template<class Compare>
    void merge(forward_list& x, Compare comp) {
        bool is_change;
        _forward_node<T> *h1 = _head->next, *h2 = x._head->next;
        _head->next = nullptr;
        x._head->next = nullptr;
        h1 = _merge_list(h1, h2, _tail, x._tail, comp, is_change);
        _head->next = h1;
        if(is_change) {
            _forward_node<T> *tmp = _tail;
            _tail = x._tail;
            x._tail = tmp;
        }
        x._head->next = x._tail;
    }

    template<class Compare>
    void merge(forward_list&& x, Compare comp) {
        bool is_change;
        _forward_node<T> *h1 = _head->next, *h2 = x._head->next;
        _head->next = nullptr;
        x._head->next = nullptr;
        h1 = _merge_list(h1, h2, _tail, x._tail, comp, is_change);
        _head->next = h1;
        if(is_change) {
            _forward_node<T> *tmp = _tail;
            _tail = x._tail;
            x._tail = tmp;
        }
        x._head->next = x._tail;
    }

    template<class Compare>
    void sort(Compare comp) {
        _forward_node<T> *tmp_h = _head->next, *tmp_t = _head;
        while(tmp_t->next != _tail)
            tmp_t = tmp_t->next;
        _head->next = nullptr;
        tmp_t->next = nullptr;
        tmp_h = _merge_sort(tmp_h, comp);
        _head->next = tmp_h;
        tmp_t = _head->next;
        while(tmp_t->next)
            tmp_t = tmp_t->next;
        tmp_t->next = _tail;
    }

    void remove(const T& value) {
        remove_if([value](const T& a)
                  ->bool { return value == a;});
    }

    void unique() {
        unique([=](const T& a, const T& b)
               ->bool { return a == b; });
    }

    void merge(forward_list& x) {
        merge(x, [=](const T& a, const T& b)
                 ->bool { return a < b; });
    }

    void merge(forward_list&& x) {
        merge(static_cast<forward_list&&>(x),
              [=](const T& a, const T& b)
              ->bool { return a < b; });
    }

    void sort() {
        sort([=](const T& a, const T& b)
             ->bool { return a < b; });
    }

    void reverse() noexcept {
        if((_head->next == _tail) || (_head->next->next == _tail))  return;
        _forward_node<T> *pre = _head, *cur = pre->next, *next = nullptr, *tmp = nullptr;
        while(cur) {
            next = cur->next;
            cur->next = pre;
            pre = cur;
            cur = next;
        }
        _head->next = nullptr;
        tmp = _head;
        _head = _tail;
        _tail = tmp;
    }
  };

  // 交换
  template< class T, class Alloc >
  void swap( jr_std::forward_list<T,Alloc>& lhs,
             jr_std::forward_list<T,Alloc>& rhs ) {
      lhs.swap(rhs);
  }

  // 比较
  template< class T, class Alloc >
  bool operator==( const jr_std::forward_list<T,Alloc>& lhs,
                   const jr_std::forward_list<T,Alloc>& rhs ) {
      typename jr_std::forward_list<T,Alloc>::const_iterator lit, rit;
      lit = lhs.begin();
      rit = rhs.begin();
      while(lit != lhs.end()) {
          if((rit == rhs.end()) || (*lit != *rit))
              return false;
          ++lit;
          ++rit;
      }
      return rit == rhs.end();
  }

  template< class T, class Alloc >
  bool operator!=( const jr_std::forward_list<T,Alloc>& lhs,
                   const jr_std::forward_list<T,Alloc>& rhs ) {
      return !(lhs == rhs);
  }

  template< class T, class Alloc >
  bool operator<( const jr_std::forward_list<T,Alloc>& lhs,
                  const jr_std::forward_list<T,Alloc>& rhs ) {
      auto lit = lhs.begin();
      auto rit = rhs.begin();
      while(lit != lhs.end()) {
          if((rit == rhs.end()) || (*lit >= *rit))
              return false;
          ++lit;
          ++rit;
      }
      return true;
  }

  template< class T, class Alloc >
  bool operator>( const jr_std::forward_list<T,Alloc>& lhs,
                  const jr_std::forward_list<T,Alloc>& rhs ) {
      auto lit = lhs.begin();
      auto rit = rhs.begin();
      while(rit != rhs.end()) {
          if((lit == lhs.end()) || (*lit <= *rit))
              return false;
          ++lit;
          ++rit;
      }
      return true;
  }

  template< class T, class Alloc >
  bool operator<=( const jr_std::forward_list<T,Alloc>& lhs,
                   const jr_std::forward_list<T,Alloc>& rhs ) {
      return !(lhs > rhs);
  }

  template< class T, class Alloc >
  bool operator>=( const jr_std::forward_list<T,Alloc>& lhs,
                   const jr_std::forward_list<T,Alloc>& rhs ) {
      return !(lhs < rhs);
  }
}

#endif // JR_FORWARD_LIST_H
