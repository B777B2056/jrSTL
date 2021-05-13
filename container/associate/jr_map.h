#ifndef JR_MAP_H
#define JR_MAP_H

#include <iostream>
#include <cstddef>
#include "../../functional/jr_functional.h"
#include "../../memory/jr_allocator.h"
#include "../../iterator/jr_iterator.h"
#include "../../container/utils/se_iterators.h"
#include "../../container/utils/jr_utility.h"

namespace jr_std {
template<class Key, class T, class Compare = less<Key>,
           class Allocator = allocator<pair<const Key, T> > >
  class map {
  public:
    // 类型
      typedef Key key_type;
      typedef Compare key_compare;
      typedef pair<const Key, T> value_type;
      typedef Allocator allocator_type;
      typedef pair<const Key, T>* pointer;
      typedef const pair<const Key, T>* const_pointer;
      typedef value_type& reference;
      typedef const value_type& const_reference;
      typedef size_t size_type;
      typedef T mapped_type;
      typedef ptrdiff_t difference_type;
      typedef _balance_bst_iterator<pair<const Key, T>, pair<const Key, T>&, pair<const Key, T>*> iterator;
      typedef _balance_bst_iterator<pair<const Key, T>, const pair<const Key, T>&, const pair<const Key, T>*> const_iterator;

    class value_compare {
      friend class map;
    protected:
      Compare _comp;
      value_compare(Compare c = Compare()) : _comp(c) {}
    public:
      bool operator()(const value_type& x, const value_type& y) const {
        return _comp(x.first, y.first);
      }
    };

  private:
    typedef _tree_node<value_type> tnode;
    typedef _Balance_BST<value_type, false, value_compare,
            typename Allocator::template rebind<tnode>::other> tree;
    value_compare comp;
    tree t;
    size_type _size;
    Allocator _alloc_data;

  public:
    // 构造/复制/销毁
    map() : _size(0), comp(Compare()), t(comp)
    {}

    explicit map(const Allocator& a)
         : _size(0), comp(Compare()), t(comp), _alloc_data(a)
    {}

    explicit map(const Compare& c, const Allocator& a = Allocator())
         : _size(0), comp(c), t(comp), _alloc_data(a)
    {}

    template<class InputIt>
    map(InputIt first, InputIt last,
         const Compare& c = Compare(), const Allocator& a = Allocator())
         : _size(static_cast<size_type>(jr_std::distance(first, last))),
           comp(c), t(first, last, comp), _alloc_data(a)
    {}

    map(const map& x)
        : _size(x.size()), comp(x.key_comp()), t(x.begin(), x.end(), comp)
    {}

    map(map&& x) : _size(x._size), comp(x.key_comp()),
        t(static_cast<tree&&>(x.t), comp){
        x._size = 0;
    }

    map(const map& x, const Allocator& a)
        : _size(x.size()), comp(Compare()),
          t(x.begin(), x.end(), comp), _alloc_data(a)
    {}

    map(map&& x, const Allocator& a)
        : _size(x._size), comp(x.key_comp()),
        t(static_cast<tree&&>(x.t), comp), _alloc_data(a) {
        x._size = 0;
    }

    ~map() = default;

    map& operator=(const map& x) {
        if(this != & x) {
            _size = x.size();
            clear();
            insert(x.begin(), x.end());
        }
        return *this;
    }

    map& operator=(map&& x) {
        if(this != &x) {
            _size = x._size;
            x._size = 0;
            t = static_cast<tree&&>(x.t);
        }
        return *this;
    }

    allocator_type get_allocator() const noexcept { return _alloc_data; }

    // 迭代器
    iterator begin() noexcept {
        return iterator(t.get_min(), t.get_header());
    }

    const_iterator begin() const noexcept {
        return const_iterator(t.get_min(), t.get_header());
    }

    iterator end() noexcept {
        return iterator(t.get_header(), t.get_header());
    }

    const_iterator end() const noexcept {
        return const_iterator(t.get_header(), t.get_header());
    }

    const_iterator cbegin() const noexcept {
        return const_iterator(t.get_min(), t.get_header());
    }

    const_iterator cend() const noexcept {
        return const_iterator(t.get_header(), t.get_header());
    }

    // 容量
    bool empty() const noexcept { return _size == 0; }
    size_type size() const noexcept { return _size; }
    size_type max_size() const noexcept { return UINT_MAX; }

    // 元素访问
    mapped_type& operator[](const key_type& x) {
        return (*((insert(value_type(x, T()))).first)).second;
    }

    mapped_type& operator[](key_type&& x) {
        return (*((insert(value_type(static_cast<key_type&&>(x), T()))).first)).second;
    }

    mapped_type& at(const key_type& x) { return (*this)[x]; }

    const mapped_type& at(const key_type& x) const { return (*this)[x]; }

    // 修改器
    template<class... Args>
    pair<iterator, bool> emplace(Args&&... args) {
        value_type *a;
        a = _alloc_data.allocate(1);
        _alloc_data.construct(a, static_cast<Args&&>(args)...);
        bool isInsert = t.insert(*a);
        if(isInsert) ++_size;
        return pair<iterator, bool>(find(a->first), isInsert);
    }

    template<class... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args) {
        value_type *a;
        a = _alloc_data.allocate(1);
        _alloc_data.construct(a, static_cast<Args&&>(args)...);
        bool isInsert = t.insert_hint(*a, hint._node);
        if(isInsert) ++_size;
        return find(a->first);
    }

    pair<iterator,bool> insert(const value_type& x) {
        bool isInsert = t.insert(x);
        if(isInsert) ++_size;
        return pair<iterator, bool>(find(x.first), isInsert);
    }

    pair<iterator,bool> insert(value_type&& x) {
        auto m = static_cast<value_type&&>(x);
        bool isInsert = t.insert(m);
        if(isInsert) ++_size;
        return pair<iterator, bool>(find(m.first), isInsert);
    }

    iterator insert(const_iterator hint, const value_type& x) {
        bool isInsert = t.insert_hint(x, hint._node);
        if(isInsert) ++_size;
        return find(x.first);
    }

    iterator insert(const_iterator hint, value_type&& x) {
        auto m = static_cast<value_type&&>(x);
        bool isInsert = t.insert_hint(m, hint._node);
        if(isInsert) ++_size;
        return find(m.first);
    }

    template< class InputIt >
    void insert( InputIt first, InputIt last ) {
        while(first != last) {
            bool isInsert = t.insert(*first);
            if(isInsert) ++_size;
            ++first;
        }
    }

    size_type erase(const key_type& va) {
        size_type cnt = 0;
        while(find(va) != end()) {
            t.erase(value_type(va, T()));
            --_size;
            ++cnt;
        }
        return cnt;
    }

    iterator erase(iterator position) {
        iterator result = position;
        ++result;
        erase(position->first);
        return result;
    }

    iterator erase(const_iterator position) {
        iterator result = iterator(position._node, position._header);
        ++result;
        erase(position->first);
        return result;
    }

    iterator erase(const_iterator first, const_iterator last) {
        iterator result = iterator(last._node, t._header);
        while(first != last) {
            erase(iterator(first._node, t.get_header(), first._offset));
            ++first;
        }
        return result;
    }

    void swap(map& other) {
        if(this == &other)  return;
        {
            int t = _size;
            _size = other._size;
            other._size = t;
        }
        {
            tnode *tmp_r = t._root;
            t._root = other.t._root;
            other.t._root = tmp_r;
        }
        {
            tnode *tmp_h = t._header;
            t._header = other.t._header;
            other.t._header = tmp_h;
        }
    }

    void clear() noexcept {
        t.erase_all();
        _size = 0;
    }

    // 观察器
    key_compare key_comp() const { return Compare(); }

    value_compare value_comp() const { return comp; }

    // map 操作
    iterator find(const key_type& x) { return iterator(t.search(value_type(x, T())), t.get_header()); }

    const_iterator find(const key_type& x) const { return const_iterator(t.search(value_type(x, T())), t.get_header()); }

    size_type count(const key_type& x) const {
        return  find(x) == end() ? 0 : 1;
    }

    iterator lower_bound(const key_type& x) {
        tnode *min_elem = t.get_min();
        value_type tx(x, T());
        if(comp(tx, min_elem->data))
            return begin();
        iterator it = begin();
        while(comp(*it, tx) && (it != end()))
            ++it;
        if(comp(tx, *it))
            --it;
        return it;
    }

    const_iterator lower_bound(const key_type& x) const {
        tnode *min_elem = t.get_min();
        value_type tx(x, T());
        if(comp(tx, min_elem->data))
            return cbegin();
        const_iterator it = begin();
        while(comp(*it, tx) && (it != end()))
            ++it;
        if(comp(tx, *it))
            --it;
        return it;
    }

    iterator upper_bound(const key_type& x) {
        tnode *max_elem = t.get_max();
        value_type tx(x, T());
        if(!comp(tx, max_elem->data))
            return end();
        iterator it = begin();
        while(!comp(tx, *it) && (it != end()))
            ++it;
        return it;
    }

    const_iterator upper_bound(const key_type& x) const {
        tnode *max_elem = t.get_max();
        value_type tx(x, T());
        if(!comp(tx, max_elem->data))
            return cend();
        const_iterator it = begin();
        while(!comp(tx, *it) && (it != end()))
            ++it;
        return it;
    }

    pair<iterator, iterator> equal_range(const key_type& x) {
        return pair<iterator, iterator>(lower_bound(x), upper_bound(x));
    }

    pair<const_iterator, const_iterator> equal_range(const key_type& x) const {
        return pair<iterator, iterator>(lower_bound(x), upper_bound(x));
    }
  };

  template<class Key, class T, class Compare = less<Key>,
             class Allocator = allocator<pair<const Key, T>>>
    class multimap {
    public:
      // 类型
        typedef Key key_type;
        typedef Compare key_compare;
        typedef pair<const Key, T> value_type;
        typedef Allocator allocator_type;
        typedef pair<const Key, T>* pointer;
        typedef const pair<const Key, T>* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef size_t size_type;
        typedef T mapped_type;
        typedef ptrdiff_t difference_type;
        typedef _balance_bst_iterator<pair<const Key, T>, pair<const Key, T>&, pair<const Key, T>*> iterator;
        typedef _balance_bst_iterator<pair<const Key, T>, const pair<const Key, T>&, const pair<const Key, T>*> const_iterator;

      class value_compare {
        friend class multimap;
      protected:
        Compare _comp;
        value_compare(Compare c) : _comp(c) { }
      public:
        bool operator()(const value_type& x, const value_type& y) const {
          return _comp(x.first, y.first);
        }
      };

    private:
      typedef _tree_node<value_type> tnode;
      typedef _Balance_BST<value_type, true, value_compare,
              typename Allocator::template rebind<tnode>::other> tree;
      value_compare comp;
      tree t;
      size_type _size;
      Allocator _alloc_data;

    public:
      // 构造/复制/销毁
      multimap() : _size(0), comp(Compare()), t(comp)
      {}

      explicit multimap(const Allocator& a)
           : _size(0), comp(Compare()), t(comp), _alloc_data(a)
      {}

      explicit multimap(const Compare& c, const Allocator& a = Allocator())
           : _size(0), comp(c), t(comp), _alloc_data(a)
      {}

      template<class InputIt>
      multimap(InputIt first, InputIt last,
           const Compare& c = Compare(), const Allocator& a = Allocator())
           : _size(static_cast<size_type>(jr_std::distance(first, last))),
             comp(c), t(first, last, comp), _alloc_data(a)
      {}

      multimap(const multimap& x)
          : _size(x.size()), comp(x.key_comp()), t(x.begin(), x.end(), comp)
      {}

      multimap(multimap&& x) : _size(x._size), comp(x.key_comp()),
          t(static_cast<tree&&>(x.t), comp){
          x._size = 0;
      }

      multimap(const multimap& x, const Allocator& a)
          : _size(x.size()), comp(Compare()),
            t(x.begin(), x.end(), comp), _alloc_data(a)
      {}

      multimap(multimap&& x, const Allocator& a)
          : _size(x._size), comp(x.key_comp()),
          t(static_cast<tree&&>(x.t), comp), _alloc_data(a) {
          x._size = 0;
      }

      ~multimap() = default;

      multimap& operator=(const multimap& x) {
          if(this != & x) {
              _size = x.size();
              clear();
              insert(x.begin(), x.end());
          }
          return *this;
      }

      multimap& operator=(multimap&& x) {
          if(this != &x) {
              _size = x._size;
              x._size = 0;
              t = static_cast<tree&&>(x.t);
          }
          return *this;
      }

      allocator_type get_allocator() const noexcept { return _alloc_data; }

      // 迭代器
      iterator begin() noexcept {
          return iterator(t.get_min(), t.get_header());
      }

      const_iterator begin() const noexcept {
          return const_iterator(t.get_min(), t.get_header());
      }

      iterator end() noexcept {
          return iterator(t.get_header(), t.get_header());
      }

      const_iterator end() const noexcept {
          return const_iterator(t.get_header(), t.get_header());
      }

      const_iterator cbegin() const noexcept {
          return const_iterator(t.get_min(), t.get_header());
      }

      const_iterator cend() const noexcept {
          return const_iterator(t.get_header(), t.get_header());
      }

      // 容量
      bool empty() const noexcept { return _size == 0; }
      size_type size() const noexcept { return _size; }
      size_type max_size() const noexcept { return UINT_MAX; }

      // 修改器
      template<class... Args>
      pair<iterator, bool> emplace(Args&&... args) {
          value_type *a;
          a = _alloc_data.allocate(1);
          _alloc_data.construct(a, static_cast<Args&&>(args)...);
          bool isInsert = t.insert(*a);
          if(isInsert) ++_size;
          return pair<iterator, bool>(find(a->first), isInsert);
      }

      template<class... Args>
      iterator emplace_hint(const_iterator hint, Args&&... args) {
          value_type *a;
          a = _alloc_data.allocate(1);
          _alloc_data.construct(a, static_cast<Args&&>(args)...);
          bool isInsert = t.insert_hint(*a, hint._node);
          if(isInsert) ++_size;
          return find(a->first);
      }

      pair<iterator,bool> insert(const value_type& x) {
          bool isInsert = t.insert(x);
          if(isInsert) ++_size;
          return pair<iterator, bool>(find(x.first), isInsert);
      }

      pair<iterator,bool> insert(value_type&& x) {
          auto m = static_cast<value_type&&>(x);
          bool isInsert = t.insert(m);
          if(isInsert) ++_size;
          return pair<iterator, bool>(find(m.first), isInsert);
      }

      iterator insert(const_iterator hint, const value_type& x) {
          bool isInsert = t.insert_hint(x, hint._node);
          if(isInsert) ++_size;
          return find(x.first);
      }

      iterator insert(const_iterator hint, value_type&& x) {
          auto m = static_cast<value_type&&>(x);
          bool isInsert = t.insert_hint(m, hint._node);
          if(isInsert) ++_size;
          return find(x.first);
      }

      template< class InputIt >
      void insert( InputIt first, InputIt last ) {
          while(first != last) {
              bool isInsert = t.insert(*first);
              if(isInsert) ++_size;
              ++first;
          }
      }

      size_type erase(const key_type& va) {
          size_type cnt = 0;
          while(find(va) != end()) {
              t.erase(value_type(va, T()));
              --_size;
              ++cnt;
          }
          return cnt;
      }

      iterator erase(iterator position) {
          iterator result = position;
          ++result;
          erase(position->first);
          return result;
      }

      iterator erase(const_iterator position) {
          iterator result = iterator(position._node, position._header, position._offset);
          ++result;
          erase(position->first);
          return result;
      }

      iterator erase(const_iterator first, const_iterator last) {
          iterator result = iterator(last._node, t._header);
          while(first != last) {
              erase(iterator(first._node, t.get_header(), first._offset));
              ++first;
          }
          return result;
      }

      void swap(multimap& other) {
          if(this == &other)  return;
          {
              int t = _size;
              _size = other._size;
              other._size = t;
          }
          {
              tnode *tmp_r = t._root;
              t._root = other.t._root;
              other.t._root = tmp_r;
          }
          {
              tnode *tmp_h = t._header;
              t._header = other.t._header;
              other.t._header = tmp_h;
          }
      }

      void clear() noexcept {
          t.erase_all();
          _size = 0;
      }

      // 观察器
      key_compare key_comp() const { return Compare(); }

      value_compare value_comp() const { return comp; }

      // map 操作
      iterator find(const key_type& x) { return iterator(t.search(value_type(x, T())), t.get_header()); }

      const_iterator find(const key_type& x) const { return const_iterator(t.search(value_type(x, T())), t.get_header()); }

      size_type count(const key_type& x) const {
          iterator it = find(x);
          return  it == end() ? 0 : it._node->cnt;
      }

      iterator lower_bound(const key_type& x) {
          tnode *min_elem = t.get_min();
          value_type tx(x, T());
          if(comp(tx, min_elem->data))
              return begin();
          iterator it = begin();
          while(comp(*it, tx) && (it != end()))
              ++it;
          if(comp(tx, *it))
              --it;
          return it;
      }

      const_iterator lower_bound(const key_type& x) const {
          tnode *min_elem = t.get_min();
          value_type tx(x, T());
          if(comp(tx, min_elem->data))
              return cbegin();
          const_iterator it = begin();
          while(comp(*it, tx) && (it != end()))
              ++it;
          if(comp(tx, *it))
              --it;
          return it;
      }

      iterator upper_bound(const key_type& x) {
          tnode *max_elem = t.get_max();
          value_type tx(x, T());
          if(!comp(tx, max_elem->data))
              return end();
          iterator it = begin();
          while(!comp(tx, *it) && (it != end()))
              ++it;
          return it;
      }

      const_iterator upper_bound(const key_type& x) const {
          tnode *max_elem = t.get_max();
          value_type tx(x, T());
          if(!comp(tx, max_elem->data))
              return cend();
          const_iterator it = begin();
          while(!comp(tx, *it) && (it != end()))
              ++it;
          return it;
      }

      pair<iterator, iterator> equal_range(const key_type& x) {
          return pair<iterator, iterator>(lower_bound(x), upper_bound(x));
      }

      pair<const_iterator, const_iterator> equal_range(const key_type& x) const {
          return pair<iterator, iterator>(lower_bound(x), upper_bound(x));
      }
    };

      // 交换
      template<class Key, class T, class Compare, class Allocator>
      void swap(map<Key, T, Compare, Allocator>& x,
              map<Key, T, Compare, Allocator>& y)
      { x.swap(y); }

      template<class Key, class T, class Compare, class Allocator>
      void swap(multimap<Key, T, Compare, Allocator>& x,
                multimap<Key, T, Compare, Allocator>& y)
      { x.swap(y); }

      // 比较
      template< class Key, class T, class Compare, class Alloc >
      bool operator==( const map<Key,T,Compare,Alloc>& lhs,
                       const map<Key,T,Compare,Alloc>& rhs ) {
          size_t lsz = lhs.size(), rsz = rhs.size();
          if(lsz != rsz)
              return false;
          Compare comp;
          auto lit = lhs.begin();
          auto rit = rhs.begin();
          while(lit != lhs.end()) {
              if(comp((*lit).first, (*rit).first) || comp((*rit).first, (*lit).first))
                  return false;
              ++lit;
              ++rit;
          }
          return true;
      }

      template< class Key, class T, class Compare, class Alloc >
      bool operator<( const map<Key,T,Compare,Alloc>& lhs,
                      const map<Key,T,Compare,Alloc>& rhs ) {
          size_t lsz = lhs.size(), rsz = rhs.size();
          if(lsz < rsz)
              return true;
          if(lsz > rsz)
              return false;
          Compare comp;
          auto lit = lhs.begin();
          auto rit = rhs.begin();
          while(lit != lhs.end()) {
              if(!comp((*lit).first, (*rit).first))
                  return false;
              ++lit;
              ++rit;
          }
          return true;
      }

      template< class Key, class T, class Compare, class Alloc >
      bool operator>( const map<Key,T,Compare,Alloc>& lhs,
                      const map<Key,T,Compare,Alloc>& rhs ) {
          size_t lsz = lhs.size(), rsz = rhs.size();
          if(lsz > rsz)
              return true;
          if(lsz < rsz)
              return false;
          Compare comp;
          auto lit = lhs.begin();
          auto rit = rhs.begin();
          while(lit != lhs.end()) {
              if(!comp((*rit).first, (*lit).first))
                  return false;
              ++lit;
              ++rit;
          }
          return true;
      }

      template< class Key, class T, class Compare, class Alloc >
      bool operator!=( const map<Key,T,Compare,Alloc>& lhs,
                       const map<Key,T,Compare,Alloc>& rhs )
      { return !(lhs == rhs); }

      template< class Key, class T, class Compare, class Alloc >
      bool operator<=( const map<Key,T,Compare,Alloc>& lhs,
                       const map<Key,T,Compare,Alloc>& rhs )
      { return !(lhs > rhs); }

      template< class Key, class T, class Compare, class Alloc >
      bool operator>=( const map<Key,T,Compare,Alloc>& lhs,
                       const map<Key,T,Compare,Alloc>& rhs )
      { return !(lhs < rhs); }

      template< class Key, class T, class Compare, class Alloc >
      bool operator==( const multimap<Key,T,Compare,Alloc>& lhs,
                       const multimap<Key,T,Compare,Alloc>& rhs ) {
          size_t lsz = lhs.size(), rsz = rhs.size();
          if(lsz != rsz)
              return false;
          Compare comp;
          auto lit = lhs.begin();
          auto rit = rhs.begin();
          while(lit != lhs.end()) {
              if(comp((*lit).first, (*rit).first) || comp((*rit).first, (*lit).first))
                  return false;
              ++lit;
              ++rit;
          }
          return true;
      }

      template< class Key, class T, class Compare, class Alloc >
      bool operator<( const multimap<Key,T,Compare,Alloc>& lhs,
                      const multimap<Key,T,Compare,Alloc>& rhs ) {
          size_t lsz = lhs.size(), rsz = rhs.size();
          if(lsz < rsz)
              return true;
          if(lsz > rsz)
              return false;
          Compare comp;
          auto lit = lhs.begin();
          auto rit = rhs.begin();
          while(lit != lhs.end()) {
              if(!comp((*lit).first, (*rit).first))
                  return false;
              ++lit;
              ++rit;
          }
          return true;
      }

      template< class Key, class T, class Compare, class Alloc >
      bool operator>( const multimap<Key,T,Compare,Alloc>& lhs,
                      const multimap<Key,T,Compare,Alloc>& rhs ) {
          size_t lsz = lhs.size(), rsz = rhs.size();
          if(lsz > rsz)
              return true;
          if(lsz < rsz)
              return false;
          Compare comp;
          auto lit = lhs.begin();
          auto rit = rhs.begin();
          while(lit != lhs.end()) {
              if(!comp((*rit).first, (*lit).first))
                  return false;
              ++lit;
              ++rit;
          }
          return true;
      }

      template< class Key, class T, class Compare, class Alloc >
      bool operator!=( const multimap<Key,T,Compare,Alloc>& lhs,
                       const multimap<Key,T,Compare,Alloc>& rhs )
      { return !(lhs == rhs); }

      template< class Key, class T, class Compare, class Alloc >
      bool operator<=( const multimap<Key,T,Compare,Alloc>& lhs,
                       const multimap<Key,T,Compare,Alloc>& rhs )
      { return !(lhs > rhs); }

      template< class Key, class T, class Compare, class Alloc >
      bool operator>=( const multimap<Key,T,Compare,Alloc>& lhs,
                       const multimap<Key,T,Compare,Alloc>& rhs )
      { return !(lhs < rhs); }
}

#endif // JR_MAP_H
