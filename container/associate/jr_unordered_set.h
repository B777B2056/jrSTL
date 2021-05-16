#ifndef JR_UNORDERED_SET_H
#define JR_UNORDERED_SET_H

#include <cstddef>
#include <functional>
#include "../../memory/jr_allocator.h"
#include "../../container/utils/jr_hashtable.h"

namespace jr_std {
template<class Key,
           class Hash = std::hash<Key>,
           class Pred = std::equal_to<Key>,
           class Allocator = jr_std::allocator<Key>>
  class unordered_set {
  public:
      // 类型
      typedef Key key_type;
      typedef Key value_type;
      typedef Allocator allocator_type;
      typedef Key* pointer;
      typedef const Key* const_pointer;
      typedef value_type& reference;
      typedef const value_type& const_reference;
      typedef size_t size_type;
      typedef ptrdiff_t difference_type;
      typedef _hashtable_iterator<Key, Hash, Pred, Allocator, false> iterator;
      typedef _hashtable_iterator<Key, Hash, Pred, Allocator, false> const_iterator;
      typedef Hash hasher;
      typedef Pred key_equal;
      typedef iterator local_iterator;
      typedef const_iterator const_local_iterator;

   protected:
      typedef _hashtable_node<Key> hnode;
      typedef _hashtable<Key, Hash, Pred, Allocator, false> table;
      hasher _hf;
      key_equal _eql;
      Allocator _alloc_data;
      table _tab;
      size_type _num_of_elem;

   public:
    // 构造/复制/销毁
    unordered_set()
        : _hf(hasher()), _eql(key_equal()), _alloc_data(Allocator()),
          _tab(11), _num_of_elem(0)
    {}

    explicit unordered_set(const Allocator& a)
        : _hf(hasher()), _eql(key_equal()), _alloc_data(a),
          _tab(11), _num_of_elem(0)
    {}

    explicit unordered_set(size_type n,
                           const hasher& hf = hasher(),
                           const key_equal& eql = key_equal(),
                           const allocator_type& a = allocator_type())
        : _hf(hf), _eql(eql), _alloc_data(a), _tab(n), _num_of_elem(0)
    {}

    template<class InputIt>
    unordered_set(InputIt first, InputIt last, size_type n = 0,
                  const hasher& hf = hasher(),
                  const key_equal& eql = key_equal(),
                  const allocator_type& a = allocator_type())
        : _hf(hf), _eql(eql), _alloc_data(a), _num_of_elem(0) {
        _tab.set_length(jr_std::distance(first, last) + 5);
        while(first != last) {
            _tab.insert(*first);
            ++_num_of_elem;
            ++first;
        }
    }

    unordered_set(size_type n, const allocator_type& a)
        : _hf(hasher()), _eql(key_equal()), _alloc_data(a),
          _tab(n), _num_of_elem(0)
    {}

    unordered_set(size_type n, const hasher& hf, const allocator_type& a)
        : _hf(hf), _eql(key_equal()), _alloc_data(a),
          _tab(n), _num_of_elem(0)
    {}

    unordered_set(const unordered_set& x)
        : _hf(hasher()), _eql(key_equal()), _alloc_data(Allocator()),
          _tab(x._tab), _num_of_elem(x._num_of_elem)
    {}

    unordered_set(unordered_set&& x)
        : _hf(hasher()), _eql(key_equal()), _alloc_data(Allocator()),
          _tab(static_cast<table&&>(x._tab)), _num_of_elem(x._num_of_elem)
    {}

    unordered_set(const unordered_set& x, const Allocator& a)
        : _hf(hasher()), _eql(key_equal()), _alloc_data(a),
          _tab(x._tab), _num_of_elem(x._num_of_elem)
    {}

    unordered_set(unordered_set&& x, const Allocator& a)
        : _hf(hasher()), _eql(key_equal()), _alloc_data(a),
          _tab(static_cast<table&&>(x._tab)), _num_of_elem(x._num_of_elem)
    {}

    ~unordered_set() = default;

    unordered_set& operator=(const unordered_set& x) {
        _tab = x._tab;
        _num_of_elem = x._num_of_elem;
    }

    unordered_set& operator=(unordered_set&& x) {
        _tab = static_cast<table&&>(x._tab);
        _num_of_elem = x._num_of_elem;
    }

    allocator_type get_allocator() const noexcept { return _alloc_data; }

    // 迭代器
    iterator begin() noexcept {
        size_type i;
        for(i = 0; i < _tab.table.size(); ++i) {
            if(_tab.table[i]->hasElem)
                break;
        }
        return _tab[i]->next ? iterator(_tab[i]->next, &_tab, i) : end();
    }

    const_iterator begin() const noexcept {
        size_type i;
        for(i = 0; i < _tab.table.size(); ++i) {
            if(_tab.table[i]->hasElem)
                break;
        }
        return _tab[i]->next ? const_iterator(_tab[i]->next, &_tab, i) : end();
    }

    iterator end() noexcept
    { return iterator(_tab[_tab.table.size() - 1], &_tab, _tab.table.size() - 1); }

    const_iterator end() const noexcept
    { return const_iterator(_tab[_tab.table.size() - 1], &_tab, _tab.table.size() - 1); }

    const_iterator cbegin() const noexcept {
        size_type i;
        for(i = 0; i < _tab.table.size(); ++i) {
            if(_tab.table[i]->hasElem)
                break;
        }
        return _tab[i]->next ? const_iterator(_tab[i]->next, &_tab, i) : end();
    }

    const_iterator cend() const noexcept
    { return const_iterator(_tab[_tab.table.size() - 1], &_tab, _tab.table.size() - 1); }

    // 容量
    bool empty() const noexcept { return _num_of_elem == 0; }

    size_type size() const noexcept { return _num_of_elem; }

    size_type max_size() const noexcept { return UINT_MAX; }

    // 修改器
    template<class... Args>
    pair<iterator, bool> emplace(Args&&... args) {
        auto n = _tab.insert(static_cast<Args&&>(args)...);
        bool isInsert = (n.second != nullptr);
        if(!isInsert)
            return pair<iterator, bool>(end(), isInsert);
        else
            return pair<iterator, bool>(iterator(n.second, &_tab, n.first), isInsert);
    }

    template<class... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args) {
        _tab.insert_hint(hint.cur, static_cast<Args&&>(args)...);
        return iterator(hint.cur->next, &_tab, hint.offset);
    }

    pair<iterator, bool> insert(const value_type& obj) {
        auto n = _tab.insert(obj);
        bool isInsert = (n.second != nullptr);
        if(!isInsert)
            return pair<iterator, bool>(end(), isInsert);
        else
            return pair<iterator, bool>(iterator(n.second, &_tab, n.first), isInsert);
    }

    pair<iterator, bool> insert(value_type&& obj) {
        auto n = _tab.insert(static_cast<value_type&&>(obj));
        bool isInsert = (n.second != nullptr);
        if(!isInsert)
            return pair<iterator, bool>(end(), isInsert);
        else
            return pair<iterator, bool>(iterator(n.second, &_tab, n.first), isInsert);
    }

    iterator insert(const_iterator hint, const value_type& obj) {
        _tab.insert_hint(hint.cur, obj);
        return iterator(hint.cur->next, &_tab, hint.offset);
    }

    iterator insert(const_iterator hint, value_type&& obj) {
        _tab.insert_hint(hint.cur, static_cast<value_type&&>(obj));
        return iterator(hint.cur->next, &_tab, hint.offset);
    }

    template<class InputIt>
    void insert(InputIt first, InputIt last) {
        while(first != last) {
            _tab.insert(*first);
            ++first;
        }
    }

    iterator erase(iterator position) {
        auto n = _tab.erase(*position);
        if(n.second)
            return iterator(n.second, &_tab, n.first);
        else
            return end();
    }

    size_type erase(const key_type& k) {
        auto n = _tab.erase(k);
        if(n.second)
            return iterator(n.second, &_tab, n.first);
        else
            return end();
    }

    iterator erase(const_iterator first, const_iterator last) {
        while(first != last) {
            erase(*first);
            ++first;
        }
        return iterator(last.second, &_tab, last.first);
    }

    void swap(unordered_set& x) {
        unordered_set tmp = x;
        x = *this;
        *this = tmp;
    }

    void clear() noexcept {
        erase(begin(), end());
        _num_of_elem = 0;
    }

    // 观察器
    hasher hash_function() const { return hasher(); }

    key_equal key_eq() const { return Pred(); }

    // set 操作
    iterator find(const key_type& k) {
        auto n = _tab.find(k);
        if(!n.second)
            return end();
        else
            return iterator(n.second, &_tab, n.first);
    }

    const_iterator find(const key_type& k) const {
        auto n = _tab.find(k);
        if(!n.second)
            return cend();
        else
            return const_iterator(n.second, &_tab, n.first);
    }

    size_type count(const key_type& k) const {
        size_type cnt = 0;
        for(auto it = begin(); it != end(); ++it) {
            if(_eql(k, *it))
                ++cnt;
        }
        return cnt;
    }

    pair<iterator, iterator> equal_range(const key_type& k) {
        iterator first, last;
        for(iterator it = begin(); it != end(); ++it) {
            if(_eql(k, *it)) {
                first = it;
                while(_eql(k, *it))
                    ++it;
                last = it;
                break;
            }
        }
        return pair<iterator, iterator>(first, last);
    }

    pair<const_iterator, const_iterator> equal_range(const key_type& k) const {
        const_iterator first, last;
        for(const_iterator it = begin(); it != end(); ++it) {
            if(_eql(k, *it)) {
                first = it;
                while(_eql(k, *it))
                    ++it;
                last = it;
                break;
            }
        }
        return pair<const_iterator, const_iterator>(first, last);
    }

    // 桶接口
    size_type bucket_count() const noexcept {
        size_type cnt = 0;
        for(size_type i = 0; i < _tab.table.size(); ++i) {
            if(_tab.table[i]->hasElem)
                ++cnt;
        }
        return cnt;
    }

    size_type max_bucket_count() const noexcept { return _tab.table.size(); }

    size_type bucket_size(size_type n) const {
        size_type cnt = 0;
        hnode *t = _tab.table[n]->next;
        while(t) {
            ++cnt;
            t = t->next;
        }
        return cnt;
    }

    size_type bucket(const key_type& k) const { return hasher(k); }

    local_iterator begin(size_type n)
    { return local_iterator(_tab[n]->next, &_tab, n); }

    const_local_iterator begin(size_type n) const
    { return const_local_iterator(_tab[n]->next, &_tab, n); }

    local_iterator end(size_type n)
    { return local_iterator(_tab[n+1], &_tab, n+1); }

    const_local_iterator end(size_type n) const
    { return const_local_iterator(_tab[n+1], &_tab, n+1); }

    const_local_iterator cbegin(size_type n) const
    { return const_local_iterator(_tab[n]->next, &_tab, n); }

    const_local_iterator cend(size_type n) const
    { return const_local_iterator(_tab[n+1], &_tab, n+1); }

    // 散列策略
    float load_factor() const noexcept
    { return static_cast<float>(size()) / bucket_count(); }

    float max_load_factor() const noexcept
    { return static_cast<float>(size()) / max_bucket_count(); }

    void max_load_factor(float z) {
        if(load_factor() > z) {
            _tab.set_length(static_cast<size_type>(size() / z));
        }
    }

    void rehash(size_type n) { _tab.set_length(n); }

    void reserve(size_type n) { rehash(n / max_load_factor()); }
  };

  // 交换
  template<class Key, class Hash, class Pred, class Alloc>
  void swap(unordered_set<Key, Hash, Pred, Alloc>& x,
            unordered_set<Key, Hash, Pred, Alloc>& y)
  { x.swap(y); };

  // 比较
  template< class Key, class Hash, class KeyEqual, class Allocator >
  bool operator==( const unordered_set<Key,Hash,KeyEqual,Allocator>& lhs,
                   const unordered_set<Key,Hash,KeyEqual,Allocator>& rhs ) {
        if(lhs.size() != rhs.size())
            return false;
        auto lit = lhs.begin();
        auto rit = rhs.begin();
        while(lit != lhs.end()) {
            if(!KeyEqual(*lit, *rit))
                return false;
            ++lit;
            ++rit;
        }
        return true;
  }

  template< class Key, class Hash, class KeyEqual, class Allocator >
  bool operator!=( const unordered_set<Key,Hash,KeyEqual,Allocator>& lhs,
                   const unordered_set<Key,Hash,KeyEqual,Allocator>& rhs )
  { return !(lhs == rhs); }
}

#endif // JR_UNORDERED_SET_H