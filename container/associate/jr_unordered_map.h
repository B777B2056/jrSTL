#ifndef JR_UNORDERED_MAP_H
#define JR_UNORDERED_MAP_H

#include <cstddef>
#include <utility>
#include "../../functional/jr_functional.h"
#include "../../memory/jr_allocator.h"
#include "../utils/jr_hashtable.h"
#include "../utils/jr_iterators.h"

namespace jr_std {
  template<class Key,
           class T,
           class Hash,
           class Pred,
           class Allocator,
           bool isMultiMap>
  class _hashmap_base {
  public:
      // 类型
      typedef Key key_type;
      typedef T mapped_type;
      typedef std::pair<const Key, T> value_type;
      typedef Allocator allocator_type;
      typedef value_type* pointer;
      typedef const value_type* const_pointer;
      typedef value_type& reference;
      typedef const value_type& const_reference;
      typedef size_t size_type;
      typedef ptrdiff_t difference_type;
      typedef typename forward_list<value_type>::iterator local_iterator;
      typedef typename forward_list<value_type>::const_iterator const_local_iterator;

      class hasher {
          protected:
              Hash _h;

          public:
              hasher() : _h(Hash()) {}

              hasher(Hash h) : _h(h) {}

              size_type operator()(const value_type& v) const {
                  return _h(v.first);
              }
      };

      class key_equal {
          protected:
              Pred _p;

          public:
              key_equal() : _p(Pred()) {}

              key_equal(Pred p) : _p(p) {}

              bool operator()(const value_type& x, const value_type& y) const {
                  return _p(x.first, y.first);
              }
      };

      typedef _hashtable_iterator<value_type, hasher, key_equal, Allocator, isMultiMap> iterator;
      typedef _hashtable_iterator<value_type, hasher, key_equal, Allocator, isMultiMap> const_iterator;

   protected:
      typedef typename forward_list<value_type>::const_iterator hnode;
      typedef _hashtable<value_type, hasher, key_equal, Allocator, isMultiMap> table;
      hasher _hf;
      key_equal _eql;
      Allocator _alloc_data;
      table _tab;
      size_type _num_of_elem;

   public:
    // 构造/复制/销毁
    _hashmap_base()
        : _hf(Hash()), _eql(Pred()), _alloc_data(Allocator()),
          _tab(11), _num_of_elem(0)
    {}

    explicit _hashmap_base(const Allocator& a)
        : _hf(Hash()), _eql(Pred()), _alloc_data(a),
          _tab(11), _num_of_elem(0)
    {}

    explicit _hashmap_base(size_type n,
                           const hasher& hf = hasher(),
                           const key_equal& eql = key_equal(),
                           const allocator_type& a = allocator_type())
        : _hf(hf), _eql(eql), _alloc_data(a), _tab(n, hf, eql), _num_of_elem(0)
    {}

    _hashmap_base(size_type n, const allocator_type& a)
        : _hf(Hash()), _eql(Pred()), _alloc_data(a),
          _tab(n), _num_of_elem(0)
    {}

    _hashmap_base(size_type n, const hasher& hf, const allocator_type& a)
        : _hf(hf), _eql(Pred()), _alloc_data(a),
          _tab(n, hf), _num_of_elem(0)
    {}

    template<class InputIt>
    _hashmap_base(InputIt first, InputIt last, size_type,
                  const hasher& hf = hasher(),
                  const key_equal& eql = key_equal(),
                  const allocator_type& a = allocator_type())
        : _hf(hf), _eql(eql),
          _tab(jr_std::distance(first, last) + 5, hf, eql),
          _alloc_data(a), _num_of_elem(0) {
        insert(first, last);
    }

    _hashmap_base(const _hashmap_base x, const Allocator& a)
        : _hf(x._hf), _eql(x._eql), _alloc_data(a),
          _tab(x._tab), _num_of_elem(x._num_of_elem)
    {}

    _hashmap_base(_hashmap_base&& x, const Allocator& a)
        : _hf(x._hf), _eql(x._eql), _alloc_data(a),
          _tab(static_cast<table&&>(x._tab)),
          _num_of_elem(x._num_of_elem) {
        x._num_of_elem = 0;
    }

    _hashmap_base(const _hashmap_base& x)
        : _hf(x._hf), _eql(x._eql), _alloc_data(Allocator()),
          _tab(x._tab), _num_of_elem(x._num_of_elem)
    {}

    _hashmap_base(_hashmap_base&& x)
        : _hf(x._hf), _eql(x._eql), _alloc_data(Allocator()),
          _tab(static_cast<table&&>(x._tab)),
          _num_of_elem(x._num_of_elem) {
        x._num_of_elem = 0;
    }

    _hashmap_base( std::initializer_list<value_type> init,
                   size_type bucket_count,
                   const hasher& hash = hasher(),
                   const key_equal& equal = key_equal(),
                   const Allocator& alloc = Allocator() )
        : _hashmap_base(bucket_count, hash, equal, alloc) {
        for(auto it = init.begin(); it != init.end(); ++it)
            insert(*it);
    }

    virtual ~_hashmap_base() = default;

    _hashmap_base& operator=(const _hashmap_base& x) {
        if(this == &x)
            return *this;
        _hf = x._hf;
        _eql = x._eql;
        _tab = x._tab;
        _num_of_elem = x._num_of_elem;
        return *this;
    }

    _hashmap_base& operator=(_hashmap_base&& x) {
        if(this == &x)
            return *this;
        _hf = x._hf;
        _eql = x._eql;
        _tab = static_cast<table&&>(x._tab);
        _num_of_elem = x._num_of_elem;
        x._num_of_elem = 0;
        return *this;
    }

    allocator_type get_allocator() const noexcept {
        return _alloc_data;
    }

    // 迭代器
    iterator begin() noexcept {
        size_type i;
        for(i = 0; i < _tab.table.size(); ++i) {
            if(!_tab.table[i].empty())
                return iterator(&_tab, i,
                                _tab.table[i].cbegin());
        }
        return end();
    }

    const_iterator begin() const noexcept {
        size_type i;
        for(i = 0; i < _tab.table.size(); ++i) {
            if(!_tab.table[i].empty())
                return const_iterator(&_tab, i,
                                      _tab.table[i].begin());
        }
        return end();
    }

    iterator end() noexcept {
        return iterator(&_tab,
                        _tab.table.size() - 1,
                        _tab.table.back().cbegin());
    }

    const_iterator end() const noexcept {
        return const_iterator(&_tab,
                              _tab.table.size() - 1,
                              _tab.table.back().begin());
    }

    const_iterator cbegin() const noexcept {
        size_type i;
        for(i = 0; i < _tab.table.size(); ++i) {
            if(!_tab.table[i].empty())
                return const_iterator(&_tab, i,
                                      _tab.table[i].begin());
        }
        return end();
    }

    const_iterator cend() const noexcept {
        return const_iterator(&_tab,
                            _tab.table.size() - 1,
                            _tab.table.back().begin());
    }

    // 容量
    bool empty() const noexcept {
        return begin() == end();
    }

    size_type size() const noexcept {
        return _num_of_elem;
    }

    size_type max_size() const noexcept {
        return UINT_MAX;
    }

    // 修改器
  protected:
    template<class... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        value_type *a;
        a = _alloc_data.allocate(1);
        _alloc_data.construct(a, static_cast<Args&&>(args)...);
        return insert(std::move(*a));
    }

  public:
    template<class... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args) {
        value_type *a;
        a = _alloc_data.allocate(1);
        _alloc_data.construct(a, static_cast<Args&&>(args)...);
        return insert(hint, std::move(*a));
    }

  protected:
    std::pair<iterator, bool> insert(const value_type& obj) {
        bool flag;
        auto n = _tab.insert(obj, flag);
        if(flag)
            ++_num_of_elem;
        return std::pair<iterator, bool>(
                    iterator(&_tab, n.first, n.second),
                    flag);
    }

    std::pair<iterator, bool> insert(value_type&& obj) {
        bool flag;
        auto n = _tab.insert(static_cast<value_type&&>(obj), flag);
        if(flag)
            ++_num_of_elem;
        return std::pair<iterator, bool>(
                    iterator(&_tab, n.first, n.second),
                    flag);
    }

  public:
    iterator insert(const_iterator hint, const value_type& obj) {
        bool flag;
        auto d = _tab.insert_hint(hint.index, hint.cur, obj, flag);
        if(flag)
            ++_num_of_elem;
        return iterator(&_tab, hint.index, d);
    }

    iterator insert(const_iterator hint, value_type&& obj) {
        bool flag;
        auto d = _tab.insert_hint(hint.index, hint.cur,
                                  static_cast<value_type&&>(obj), flag);
        if(flag)
            ++_num_of_elem;
        return iterator(&_tab, hint.index, d);
    }

    void insert( std::initializer_list<value_type> ilist ) {
        for(auto it = ilist.begin(); it != ilist.end(); ++it)
            insert(*it);
    }

    template<class InputIt>
    void insert(InputIt first, InputIt last) {
        while(first != last) {
            insert(*first);
            ++first;
        }
    }

    iterator erase(iterator position) {
        difference_type dis = jr_std::distance(cbegin(), position);
        _tab.erase(position.index, position.cur);
        --_num_of_elem;
        iterator r = begin();
        jr_std::advance(r, dis);
        return r;
    }

    size_type erase(const key_type& k) {
        size_type cnt = _tab.erase(value_type(k, T()));
        _num_of_elem -= cnt;
        return cnt;
    }

    iterator erase(const_iterator first, const_iterator last) {
        difference_type dis = jr_std::distance(cbegin(), first);
        while(first != last) {
            first = erase(first);
        }
        iterator r = begin();
        jr_std::advance(r, dis);
        return r;
    }

    void swap(_hashmap_base& x) {
        if(this == &x)
            return;
        table tmp = static_cast<table&&>(_tab);
        _tab = static_cast<table&&>(x._tab);
        x._tab = static_cast<table&&>(tmp);
        int t = _num_of_elem;
        _num_of_elem = x._num_of_elem;
        x._num_of_elem = t;
    }

    void clear() noexcept {
        erase(begin(), end());
        _num_of_elem = 0;
    }

    // 观察器
    hasher hash_function() const {
        return hasher();
    }

    key_equal key_eq() const {
        return Pred();
    }

    // set 操作
    iterator find(const key_type& k) {
        auto n = _tab.find(value_type(k, T()));
        if(n.first == -1)
            return end();
        else
            return iterator(&_tab, n.first, n.second);
    }

    const_iterator find(const key_type& k) const {
        auto n = _tab.find(value_type(k, T()));
        if(n.first == -1)
            return cend();
        else
            return const_iterator(&_tab, n.first, n.second);
    }

    size_type count(const key_type& k) {
        return _tab.count(value_type(k, T()));
    }

    std::pair<iterator, iterator> equal_range(const key_type& k) {
        iterator first = end(), last = end();
        for(iterator it = begin(); it != end(); ++it) {
            if(_eql(value_type(k, T()), *it)) {
                first = it;
                while((it != end()) && _eql(value_type(k, T()), *it))
                    ++it;
                last = it;
                break;
            }
        }
        return std::pair<iterator, iterator>(first, last);
    }

    std::pair<const_iterator, const_iterator> equal_range(const key_type& k) const {
        const_iterator first = end(), last = end();
        for(const_iterator it = begin(); it != end(); ++it) {
            if(_eql(value_type(k, T()), *it)) {
                first = it;
                while((it != end()) && _eql(value_type(k, T()), *it))
                    ++it;
                last = it;
                break;
            }
        }
        return std::pair<const_iterator,
                         const_iterator>(first, last);
    }

    // 桶接口
    size_type bucket_count() const noexcept {
        size_type cnt = 0;
        for(size_type i = 0; i < _tab.table.size(); ++i) {
            if(!_tab.table[i].empty())
                ++cnt;
        }
        return cnt;
    }

    size_type max_bucket_count() const noexcept {
        return _tab.table.size() - 1;
    }

    size_type bucket_size(size_type n) const {
        size_type cnt = 0;
        hnode t = _tab.table[n].cbegin();
        while(t != _tab.table[n].cend()) {
            ++cnt;
            ++t;
        }
        return cnt;
    }

    size_type bucket(const key_type& k) const {
        return hasher(k);
    }

    local_iterator
    begin(size_type n) {
        return _tab.table[n].begin();
    }

    const_local_iterator
    begin(size_type n) const {
        return _tab.table[n].cbegin();
    }

    local_iterator
    end(size_type n) {
        return _tab.table[n].end();
    }

    const_local_iterator
    end(size_type n) const {
        return _tab.table[n].cend();
    }

    const_local_iterator
    cbegin(size_type n) const {
        return _tab.table[n].cbegin();
    }

    const_local_iterator
    cend(size_type n) const {
        return _tab.table[n].cend();
    }

    // 散列策略
    float load_factor() const noexcept {
        return static_cast<float>(size()) / bucket_count();
    }

    float max_load_factor() const noexcept {
        return static_cast<float>(size()) / max_bucket_count();
    }

    void max_load_factor(float z) {
        if(load_factor() > z) {
            _tab.set_length(static_cast<size_type>(size() / z));
        }
    }

    void rehash(size_type n) {
        _tab.set_length(n);
    }

    void reserve(size_type n) {
        rehash(n / max_load_factor());
    }
  };

  template<class Key, class T,
           class Hash = std::hash<Key>,
           class Pred = jr_std::equal_to<Key>,
           class Allocator = jr_std::allocator<std::pair<const Key, T> > >
  class unordered_map
          : public _hashmap_base<Key, T, Hash, Pred, Allocator, false> {
  private:
      typedef _hashmap_base<Key, T, Hash, Pred, Allocator, false> _base;

  public:
      using _base::insert;
      // 构造（继承父类）
      unordered_map() {}

      explicit unordered_map(const Allocator& a)
          : _base(a)
      {}

      explicit unordered_map(typename _base::size_type n,
                                  const Hash& hf = Hash(),
                                  const Pred& eql = Pred(),
                                  const Allocator& a = Allocator())
          : _base(n, hf, eql, a)
      {}

      unordered_map(typename _base::size_type n, const Allocator& a)
          : _base(n, a)
      {}

      unordered_map(typename _base::size_type n, const Hash& hf, const Allocator& a)
          : _base(n, hf, a)
      {}

      template<class InputIt>
      unordered_map(InputIt first, InputIt last,
                    typename _base::size_type n = 0,
                    const Hash& hf = Hash(),
                    const Pred& eql = Pred(),
                    const Allocator& a = Allocator())
          : _base(first, last, n, hf, eql, a)
      {}

      unordered_map(const unordered_map x, const Allocator& a)
          : _base(x, a)
      {}

      unordered_map(unordered_map&& x, const Allocator& a)
          : _base(static_cast<unordered_map&&>(x), a)
      {}

      unordered_map( std::initializer_list<typename _base::value_type> init,
                     typename _base::size_type bucket_count = 11,
                     const Hash& hash = Hash(),
                     const Pred& equal = Pred(),
                     const Allocator& alloc = Allocator() )
        : _base(init, bucket_count, hash, equal, alloc)
      {}
      // 元素访问
      typename _base::mapped_type& operator[](const typename _base::key_type& x) {
          return (*((_base::insert(typename _base::value_type(x, T()))).first)).second;
      }

      typename _base::mapped_type& operator[](typename _base::key_type&& x) {
          return (*((_base::insert(
                         typename _base::value_type(
                                       static_cast<typename _base::key_type&&>(x), T()))).first)).second;
      }

      typename _base::mapped_type& at(const typename _base::key_type& x) {
          return (*this)[x];
      }

      const typename _base::mapped_type& at(const typename _base::key_type& x) const {
          return (*this)[x];
      }

      template< class... Args >
      std::pair<typename _base::iterator, bool>
      emplace( Args&&... args ) {
          return _base::emplace(static_cast<Args&&>(args)...);
      }

      std::pair<typename _base::iterator, bool>
      insert( const typename _base::value_type& value ) {
          return _base::insert(value);
      }

      std::pair<typename _base::iterator, bool>
      insert( typename _base::value_type&& value ) {
          return _base::insert(static_cast<typename _base::value_type&&>(value));
      }
  };

  template<class Key, class T,
           class Hash = std::hash<Key>,
           class Pred = jr_std::equal_to<Key>,
           class Allocator = jr_std::allocator<std::pair<const Key, T> > >
  class unordered_multimap
          : public _hashmap_base<Key, T, Hash, Pred, Allocator, true> {
  private:
      typedef _hashmap_base<Key, T, Hash, Pred, Allocator, true> _base;

  public:
      using _base::insert;
      // 构造（继承父类）
      unordered_multimap() {}

      explicit unordered_multimap(const Allocator& a)
          : _base(a)
      {}

      explicit unordered_multimap(typename _base::size_type n,
                                  const Hash& hf = Hash(),
                                  const Pred& eql = Pred(),
                                  const Allocator& a = Allocator())
          : _base(n, hf, eql, a)
      {}

      unordered_multimap(typename _base::size_type n, const Allocator& a)
          : _base(n, a)
      {}

      unordered_multimap(typename _base::size_type n, const Hash& hf, const Allocator& a)
          : _base(n, hf, a)
      {}

      template<class InputIt>
      unordered_multimap(InputIt first, InputIt last,
                    typename _base::size_type n = 0,
                    const Hash& hf = Hash(),
                    const Pred& eql = Pred(),
                    const Allocator& a = Allocator())
          : _base(first, last, n, hf, eql, a)
      {}

      unordered_multimap(const unordered_multimap x, const Allocator& a)
          : _base(x, a)
      {}

      unordered_multimap(unordered_multimap&& x, const Allocator& a)
          : _base(static_cast<unordered_multimap&&>(x), a)
      {}

      unordered_multimap( std::initializer_list<typename _base::value_type> init,
                     typename _base::size_type bucket_count = 11,
                     const Hash& hash = Hash(),
                     const Pred& equal = Pred(),
                     const Allocator& alloc = Allocator() )
        : _base(init, bucket_count, hash, equal, alloc)
      {}

      // 特化操作
      template< class... Args >
      typename _base::iterator emplace( Args&&... args ) {
          return (_base::emplace(static_cast<Args&&>(args)...)).first;
      }

      typename _base::iterator insert( const typename _base::value_type& value ) {
          return (_base::insert(value)).first;
      }

      typename _base::iterator insert( typename _base::value_type&& value ) {
          return (_base::insert(static_cast<typename _base::value_type&&>(value))).first;
      }
  };

  // 交换
  template<class Key, class Hash, class Pred, class Alloc>
  void swap(unordered_map<Key, Hash, Pred, Alloc>& x,
            unordered_map<Key, Hash, Pred, Alloc>& y) {
      x.swap(y);
  }

  template< class Key, class Hash, class KeyEqual, class Alloc >
  void swap( unordered_multimap<Key,Hash,KeyEqual,Alloc>& x,
             unordered_multimap<Key,Hash,KeyEqual,Alloc>& y ) {
      x.swap(y);
  }

  // 比较
  template< class Key, class Hash, class KeyEqual, class Allocator >
  bool operator==( const unordered_map<Key,Hash,KeyEqual,Allocator>& lhs,
                   const unordered_map<Key,Hash,KeyEqual,Allocator>& rhs ) {
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
  bool operator!=( const unordered_map<Key,Hash,KeyEqual,Allocator>& lhs,
                   const unordered_map<Key,Hash,KeyEqual,Allocator>& rhs ) {
      return !(lhs == rhs);
  }

  template< class Key, class Hash, class KeyEqual, class Allocator >
  bool operator==( const unordered_multimap<Key,Hash,KeyEqual,Allocator>& lhs,
                   const unordered_multimap<Key,Hash,KeyEqual,Allocator>& rhs ) {
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
  bool operator!=( const unordered_multimap<Key,Hash,KeyEqual,Allocator>& lhs,
                   const unordered_multimap<Key,Hash,KeyEqual,Allocator>& rhs ) {
      return !(lhs == rhs);
  }
}
#endif // JR_UNORDERED_MAP_H
