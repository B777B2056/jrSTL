#ifndef JR_SET_H
#define JR_SET_H

#include <cstddef>
#include "../../functional/jr_functional.h"
#include "../../memory/jr_allocator.h"
#include "../../iterator/jr_iterator.h"
#include "../../container/utils/se_iterators.h"
#include "../../container/utils/jr_utility.h"

namespace jr_std {
template<class Key, class Compare = jr_std::less<Key>,
         class Allocator = jr_std::allocator<Key> >
  class set {
  public:
    // 类型
    typedef Key key_type;
    typedef Compare key_compare;
    typedef Key value_type;
    typedef Compare value_compare;
    typedef Allocator allocator_type;
    typedef Key* pointer;
    typedef const Key* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef _balance_bst_iterator<Key, const Key&, const Key*> iterator;
    typedef _balance_bst_iterator<Key, const Key&, const Key*> const_iterator;

  private:
    typedef _tree_node<Key> tnode;
    typedef _Balance_BST<Key, false, Compare, typename Allocator::template rebind<tnode>::other> tree;
    tree t;
    Compare comp;
    size_type _size;
    Allocator _alloc_data;

  public:
    // 构造/复制/销毁
    set() : _size(0), t(), comp(Compare())
    {}

    explicit set(const Allocator&) : _size(0), t()
    {}

    explicit set(const Compare& c, const Allocator& a = Allocator())
        : _size(0), t(), comp(c), _alloc_data(a)
    {}

    template< class InputIt >
    set( InputIt first, InputIt last,
         const Compare& c = Compare(),
         const Allocator& a = Allocator() )
        : _size(static_cast<size_type>(jr_std::distance(first, last))),
          t(first, last), comp(c), _alloc_data(a)
    {}

    set(const set& x)
        : _size(x.size()), t(x.begin(), x.end())
    {}

    set(set&& x) {
        _size = x._size;
        x._size = 0;
        t = static_cast<tree&&>(x.t);
    }

    set(const set& x, const Allocator& a)
        : _size(x.size()), t(x.begin(), x.end()), _alloc_data(a)
    {}

    set(set&& x, const Allocator& a) : _alloc_data(a) {
        _size = x._size;
        x._size = 0;
        t = static_cast<tree&&>(x.t);
    }

    ~set()  = default;

    set& operator=(const set& x) {
        if(this != & x) {
            _size = x.size();
            clear();
            insert(x.begin(), x.end());
        }
        return *this;
    }

    set& operator=(set&& x) {
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
        return iterator(t.get_min(), t.get_header());
    }

    iterator end() noexcept {
        return iterator(t.get_header(), t.get_header());
    }

    const_iterator end() const noexcept {
        return iterator(t.get_header(), t.get_header());
    }

    const_iterator cbegin() const noexcept {
        return iterator(t.get_min(), t.get_header());
    }

    const_iterator cend() const noexcept {
        return iterator(t.get_header(), t.get_header());
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
        return pair<iterator, bool>(iterator(t.search(*a), t.get_header()), isInsert);
    }

    template<class... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args) {
        value_type *a;
        a = _alloc_data.allocate(1);
        _alloc_data.construct(a, static_cast<Args&&>(args)...);
        bool isInsert = t.insert_hint(*a, hint._node);
        if(isInsert) ++_size;
        return iterator(t.search(*a), t.get_header());
    }

    pair<iterator,bool> insert(const value_type& x) {
        bool isInsert = t.insert(x);
        if(isInsert) ++_size;
        return pair<iterator, bool>(iterator(t.search(x), t.get_header()), isInsert);
    }

    pair<iterator,bool> insert(value_type&& x) {
        auto m = static_cast<value_type&&>(x);
        bool isInsert = t.insert(m);
        if(isInsert) ++_size;
        return pair<iterator, bool>(iterator(t.search(m), t.get_header()), isInsert);
    }

    iterator insert(const_iterator hint, const value_type& x) {
        bool isInsert = t.insert_hint(x, hint._node);
        if(isInsert) ++_size;
        return iterator(t.search(x), t.get_header());
    }

    iterator insert(const_iterator hint, value_type&& x) {
        auto m = static_cast<value_type&&>(x);
        bool isInsert = t.insert_hint(m, hint._node);
        if(isInsert) ++_size;
        return iterator(t.search(m), t.get_header());
    }

    template< class InputIt >
    void insert( InputIt first, InputIt last ) {
        while(first != last) {
            bool isInsert = t.insert(*first);
            if(isInsert) ++_size;
            ++first;
        }
    }

    iterator erase(iterator position) {
        iterator result = position;
        ++result;
        value_type va = position._node->data;
        erase(va);
        return result;
    }

    void erase(const key_type& va) {
        if(t.search(va)) {
            t.erase(va);
            --_size;
        }
    }

    iterator erase(const_iterator first, const_iterator last) {
        iterator result = iterator(last._node, t._header);
        while(first != last) {
            erase(iterator(first._node, t.get_header()));
            ++first;
        }
        return result;
    }

    void swap(set& other) {
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
    key_compare key_comp() const { return comp; }

    value_compare value_comp() const { return comp; }

    // set 操作
    iterator find(const key_type& x) { return iterator(t.search(x), t.get_header()); }

    const_iterator find(const key_type& x) const { return const_iterator(t.search(x), t.get_header()); }

    size_type count(const key_type& x) const {
        return  find(x) == end() ? 0 : 1;
    }

    iterator lower_bound(const key_type& x) {
        tnode *min_elem = t.get_min();
        if(comp(x, min_elem->data))
            return begin();
        iterator it = begin();
        while(comp(*it, x) && (it != end()))
            ++it;
        if(comp(x, *it))
            --it;
        return it;
    }

    const_iterator lower_bound(const key_type& x) const {
        tnode *min_elem = t.get_min();
        if(comp(x, min_elem->data))
            return cbegin();
        const_iterator it = begin();
        while(comp(*it, x) && (it != end()))
            ++it;
        if(comp(x, *it))
            --it;
        return it;
    }

    iterator upper_bound(const key_type& x) {
        tnode *max_elem = t.get_max();
        if(!comp(x, max_elem->data))
            return end();
        iterator it = begin();
        while(!comp(x, *it) && (it != end()))
            ++it;
        return it;
    }

    const_iterator upper_bound(const key_type& x) const {
        tnode *max_elem = t.get_max();
        if(!comp(x, max_elem->data))
            return cend();
        const_iterator it = begin();
        while(!comp(x, *it) && (it != end()))
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
  template<class Key, class Compare, class Allocator>
  void swap(set<Key, Compare, Allocator>& x,
            set<Key, Compare, Allocator>& y)
  { x.swap(y); }

  // 比较
  template< class Key, class Compare, class Alloc >
  bool operator==( const set<Key,Compare,Alloc>& lhs,
                   const set<Key,Compare,Alloc>& rhs ) {
      size_t lsz = lhs.size(), rsz = rhs.size();
      if(lsz != rsz)
          return false;
      Compare comp;
      auto lit = lhs.begin();
      auto rit = rhs.begin();
      while(lit != lhs.end()) {
          if(comp(*lit, *rit) || comp(*rit, *lit))
              return false;
          ++lit;
          ++rit;
      }
      return true;
  }

  template< class Key, class Compare, class Alloc >
  bool operator!=( const set<Key,Compare,Alloc>& lhs,
                   const set<Key,Compare,Alloc>& rhs )
  { return !(lhs == rhs); }

  template< class Key, class Compare, class Alloc >
  bool operator<( const set<Key,Compare,Alloc>& lhs,
                  const set<Key,Compare,Alloc>& rhs ) {
      size_t lsz = lhs.size(), rsz = rhs.size();
      if(lsz < rsz)
          return true;
      if(lsz > rsz)
          return false;
      Compare comp;
      auto lit = lhs.begin();
      auto rit = rhs.begin();
      while(lit != lhs.end()) {
          if(!comp(*lit, *rit))
              return false;
          ++lit;
          ++rit;
      }
      return true;
  }

  template< class Key, class Compare, class Alloc >
  bool operator>( const set<Key,Compare,Alloc>& lhs,
                  const set<Key,Compare,Alloc>& rhs ) {
      size_t lsz = lhs.size(), rsz = rhs.size();
      if(lsz > rsz)
          return true;
      if(lsz < rsz)
          return false;
      Compare comp;
      auto lit = lhs.begin();
      auto rit = rhs.begin();
      while(lit != lhs.end()) {
          if(!comp(*rit, *lit))
              return false;
          ++lit;
          ++rit;
      }
      return true;
  }

  template< class Key, class Compare, class Alloc >
  bool operator<=( const set<Key,Compare,Alloc>& lhs,
                   const set<Key,Compare,Alloc>& rhs )
  { return !(lhs > rhs); }

  template< class Key, class Compare, class Alloc >
  bool operator>=( const set<Key,Compare,Alloc>& lhs,
                   const set<Key,Compare,Alloc>& rhs )
  { return !(lhs < rhs); }

template<class Key, class Compare = jr_std::less<Key>,
           class Allocator = jr_std::allocator<Key>>
  class multiset {
  public:
    // 类型
      typedef Key key_type;
      typedef Compare key_compare;
      typedef Key value_type;
      typedef Compare value_compare;
      typedef Allocator allocator_type;
      typedef Key* pointer;
      typedef const Key* const_pointer;
      typedef value_type& reference;
      typedef const value_type& const_reference;
      typedef size_t size_type;
      typedef ptrdiff_t difference_type;
      typedef _balance_bst_iterator<Key, const Key&, const Key*> iterator;
      typedef _balance_bst_iterator<Key, const Key&, const Key*> const_iterator;
private:
      typedef _tree_node<Key> tnode;
      typedef _Balance_BST<Key, true, Compare, typename Allocator::template rebind<tnode>::other> tree;
      tree t;
      Compare comp;
      size_type _size;
      Allocator _alloc_data;

  public:
    // 构造/复制/销毁
    multiset()
        : _size(0), t(), comp(Compare())
    {}

    explicit multiset(const Allocator& a)
        : _size(0), t(), comp(Compare()), _alloc_data(a)
    {}

    explicit multiset(const Compare& c, const Allocator& a = Allocator())
        : _size(0), t(), comp(c), _alloc_data(a)
    {}

    template<class InputIt>
    multiset(InputIt first, InputIt last,
             const Compare& c = Compare(),
             const Allocator& a = Allocator())
        : _size(static_cast<size_type>(jr_std::distance(first, last))),
          t(first, last), comp(c), _alloc_data(a)
    {}

    multiset(const multiset& x)
        : _size(x.size()), t(x.begin(), x.end())
    {}

    multiset(multiset&& x) {
        _size = x._size;
        x._size = 0;
        t = static_cast<tree&&>(x.t);
    }

    multiset(const multiset& x, const Allocator& a)
        : _size(x.size()), t(x.begin(), x.end()), _alloc_data(a)
    {}

    multiset(multiset&& x, const Allocator& a)
        : _alloc_data(a) {
        _size = x._size;
        x._size = 0;
        t = static_cast<tree&&>(x.t);
    }

    ~multiset() = default;

    multiset& operator=(const multiset& x) {
        if(this != & x) {
            _size = x.size();
            clear();
            insert(x.begin(), x.end());
        }
        return *this;
    }

    multiset& operator=(multiset&& x) {
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
        return iterator(t.get_min(), t.get_header());
    }

    iterator end() noexcept {
        return iterator(t.get_header(), t.get_header());
    }

    const_iterator end() const noexcept {
        return iterator(t.get_header(), t.get_header());
    }

    const_iterator cbegin() const noexcept {
        return iterator(t.get_min(), t.get_header());
    }

    const_iterator cend() const noexcept {
        return iterator(t.get_header(), t.get_header());
    }

    // 容量
    bool empty() const noexcept { return _size == 0; }
    size_type size() const noexcept { return _size; }
    size_type max_size() const noexcept { return UINT_MAX; }

    // 修改器
    template<class... Args>
    iterator emplace(Args&&... args) {
        value_type *a;
        a = _alloc_data.allocate(1);
        _alloc_data.construct(a, static_cast<Args&&>(args)...);
        int offset = 0;
        tnode *n = t.search(*a);
        if(n != t.get_header())
            offset = n->cnt;
        t.insert(*a);
        ++_size;
        return iterator(n, t.get_header(), ++offset);
    }

    template<class... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args) {
        value_type *a;
        a = _alloc_data.allocate(1);
        _alloc_data.construct(a, static_cast<Args&&>(args)...);
        int offset = 0;
        tnode *n = t.search(*a);
        if(n != t.get_header())
            offset = n->cnt;
        t.insert_hint(*a, hint._node);
        ++_size;
        return iterator(t.search(*a), t.get_header(), ++offset);
    }

    iterator insert(const value_type& x) {
        int offset = 0;
        tnode *n = t.search(x);
        if(n != t.get_header())
            offset = n->cnt;
        t.insert(x);
        ++_size;
        return iterator(t.search(x), t.get_header(), ++offset);
    }

    iterator insert(value_type&& x) {
        auto m = static_cast<value_type&&>(x);
        int offset = 0;
        tnode *n = t.search(m);
        if(n != t.get_header())
            offset = n->cnt;
        t.insert(m);
        ++_size;
        return iterator(t.search(m), t.get_header(), ++offset);
    }

    iterator insert(const_iterator hint, const value_type& x) {
        int offset = 0;
        tnode *n = t.search(x);
        if(n != t.get_header())
            offset = n->cnt;
        t.insert_hint(x, hint._node);
        ++_size;
        return iterator(t.search(x), t.get_header(), ++offset);
    }

    iterator insert(const_iterator hint, value_type&& x) {
        auto m = static_cast<value_type&&>(x);
        int offset = 0;
        tnode *n = t.search(m);
        if(n != t.get_header())
            offset = n->cnt;
        t.insert_hint(m, hint._node);
        ++_size;
        return iterator(t.search(m), t.get_header(), ++offset);
    }

    template< class InputIt >
    void insert( InputIt first, InputIt last ) {
        while(first != last) {
            bool isInsert = t.insert(*first);
            if(isInsert) ++_size;
            ++first;
        }
    }

    iterator erase(iterator position) {
        iterator result = position;
        ++result;
        value_type va = position._node->data;
        erase(va);
        return result;
    }

    void erase(const key_type& va) {
        if(t.search(va) != t._header) {
            t.erase(va);
            --_size;
        }
    }

    iterator erase(const_iterator first, const_iterator last) {
        iterator result = iterator(last._node, t._header);
        while(first != last) {
            erase(iterator(first._node, t.get_header()));
            ++first;
        }
        return result;
    }

    void swap(multiset& other) {
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
    key_compare key_comp() const { return comp; }
    value_compare value_comp() const { return comp; }

    // set 操作
    iterator find(const key_type& x) { return iterator(t.search(x), t.get_header()); }

    const_iterator find(const key_type& x) const { return const_iterator(t.search(x), t.get_header()); }

    size_type count(const key_type& x) const {
        tnode *s = t.search(x);
        return  s == t._header ? 0 : s->cnt;
    }

    iterator lower_bound(const key_type& x) {
        tnode *min_elem = t.get_min();
        if(comp(x, min_elem->data))
            return begin();
        iterator it = begin();
        while(comp(*it, x) && (it != end()))
            ++it;
        if(comp(x, *it))
            --it;
        return it;
    }

    const_iterator lower_bound(const key_type& x) const {
        tnode *min_elem = t.get_min();
        if(comp(x, min_elem->data))
            return cbegin();
        const_iterator it = begin();
        while(comp(*it, x) && (it != end()))
            ++it;
        if(comp(x, *it))
            --it;
        return it;
    }

    iterator upper_bound(const key_type& x) {
        tnode *max_elem = t.get_max();
        if(!comp(x, max_elem->data))
            return end();
        iterator it = begin();
        while(!comp(x, *it) && (it != end()))
            ++it;
        return it;
    }

    const_iterator upper_bound(const key_type& x) const {
        tnode *max_elem = t.get_max();
        if(!comp(x, max_elem->data))
            return cend();
        const_iterator it = begin();
        while(!comp(x, *it) && (it != end()))
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
    template<class Key, class Compare, class Allocator>
    void swap(multiset<Key, Compare, Allocator>& x,
              multiset<Key, Compare, Allocator>& y)
    { x.swap(y); }

    // 比较
    template< class Key, class Compare, class Alloc >
    bool operator==( const multiset<Key,Compare,Alloc>& lhs,
                     const multiset<Key,Compare,Alloc>& rhs ) {
        size_t lsz = lhs.size(), rsz = rhs.size();
        if(lsz != rsz)
            return false;
        Compare comp;
        auto lit = lhs.begin(), rit = rhs.begin();
        while(lit != lhs.end()) {
            if(comp(*lit, *rit) || comp(*rit, *lit))
                return false;
            ++lit;
            ++rit;
        }
        return true;
    }

    template< class Key, class Compare, class Alloc >
    bool operator!=( const multiset<Key,Compare,Alloc>& lhs,
                     const multiset<Key,Compare,Alloc>& rhs )
    { return !(lhs == rhs); }

    template< class Key, class Compare, class Alloc >
    bool operator<( const multiset<Key,Compare,Alloc>& lhs,
                    const multiset<Key,Compare,Alloc>& rhs ) {
        size_t lsz = lhs.size(), rsz = rhs.size();
        if(lsz < rsz)
            return true;
        if(lsz > rsz)
            return false;
        Compare comp;
        auto lit = lhs.begin();
        auto rit = rhs.begin();
        while(lit != lhs.end()) {
            if(!comp(*lit, *rit))
                return false;
            ++lit;
            ++rit;
        }
        return true;
    }

    template< class Key, class Compare, class Alloc >
    bool operator>( const multiset<Key,Compare,Alloc>& lhs,
                    const multiset<Key,Compare,Alloc>& rhs ) {
        size_t lsz = lhs.size(), rsz = rhs.size();
        if(lsz > rsz)
            return true;
        if(lsz < rsz)
            return false;
        Compare comp;
        auto lit = lhs.begin();
        auto rit = rhs.begin();
        while(lit != lhs.end()) {
            if(!comp(*rit, *lit))
                return false;
            ++lit;
            ++rit;
        }
        return true;
    }

    template< class Key, class Compare, class Alloc >
    bool operator<=( const multiset<Key,Compare,Alloc>& lhs,
                     const multiset<Key,Compare,Alloc>& rhs )
    { return !(lhs > rhs); }

    template< class Key, class Compare, class Alloc >
    bool operator>=( const multiset<Key,Compare,Alloc>& lhs,
                     const multiset<Key,Compare,Alloc>& rhs )
    { return !(lhs < rhs); }
}

#endif // JR_SET_H
