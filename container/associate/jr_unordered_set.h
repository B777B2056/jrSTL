#ifndef JR_UNORDERED_SET_H
#define JR_UNORDERED_SET_H

#include <utility>
#include <cstddef>
#include "../../functional/jr_functional.h"
#include "../../memory/jr_allocator.h"
#include "../utils/jr_hashtable.h"
#include "../utils/jr_iterators.h"

namespace panzer {
  template<class Key,
           class Hash,
           class Pred,
           class Allocator,
           bool isMultiSet>
  class _hashset_base {
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
      typedef _hashtable_iterator<Key, Hash, Pred, Allocator, isMultiSet> iterator;
      typedef _hashtable_iterator<Key, Hash, Pred, Allocator, isMultiSet> const_iterator;
      typedef Hash hasher;
      typedef Pred key_equal;
      typedef typename forward_list<Key>::iterator local_iterator;
      typedef typename forward_list<Key>::const_iterator const_local_iterator;

   protected:
      typedef typename forward_list<Key>::const_iterator hnode;
      typedef _hashtable<Key, Hash, Pred, Allocator, isMultiSet> table;
      hasher _hf;
      key_equal _eql;
      Allocator _alloc_data;
      table _tab;
      size_type _num_of_elem;

   public:
    // 构造/复制/销毁
    _hashset_base()
        : _hf(hasher()),
          _eql(key_equal()),
          _alloc_data(Allocator()),
          _tab(11), _num_of_elem(0)
    {}

    explicit _hashset_base(const Allocator& a)
        : _hf(hasher()),
          _eql(key_equal()),
          _alloc_data(a),
          _tab(11), _num_of_elem(0)
    {}

    explicit _hashset_base(size_type n,
                           const hasher& hf = hasher(),
                           const key_equal& eql = key_equal(),
                           const allocator_type& a = allocator_type())
        : _hf(hf), _eql(eql), _alloc_data(a),
          _tab(n, hf, eql), _num_of_elem(0)
    {}

    template<class InputIt>
    _hashset_base(InputIt first, InputIt last,
                  size_type,
                  const hasher& hf = hasher(),
                  const key_equal& eql = key_equal(),
                  const allocator_type& a = allocator_type())
        : _hf(hf), _eql(eql),
          _tab(panzer::distance(first, last) + 5, hf, eql),
          _alloc_data(a), _num_of_elem(0) {
        insert(first, last);
    }

    _hashset_base(size_type n,
                  const allocator_type& a)
        : _hf(hasher()), _eql(key_equal()), _alloc_data(a),
          _tab(n), _num_of_elem(0)
    {}

    _hashset_base(size_type n,
                  const hasher& hf,
                  const allocator_type& a)
        : _hf(hf), _eql(key_equal()), _alloc_data(a),
          _tab(n, hf), _num_of_elem(0)
    {}

    _hashset_base(const _hashset_base& x,
                  const Allocator& a)
        : _hf(hasher()), _eql(key_equal()), _alloc_data(a),
          _tab(x._tab), _num_of_elem(x._num_of_elem)
    {}

    _hashset_base(_hashset_base&& x,
                  const Allocator& a)
        : _hf(hasher()), _eql(key_equal()), _alloc_data(a),
          _tab(static_cast<table&&>(x._tab)),
               _num_of_elem(x._num_of_elem) {
        x._num_of_elem = 0;
    }

    _hashset_base(const _hashset_base& x)
        : _hf(hasher()), _eql(key_equal()),
          _alloc_data(Allocator()),
          _tab(x._tab), _num_of_elem(x._num_of_elem)
    {}

    _hashset_base(_hashset_base&& x)
        : _hf(hasher()), _eql(key_equal()),
          _alloc_data(Allocator()),
          _tab(static_cast<table&&>(x._tab)),
               _num_of_elem(x._num_of_elem) {
        x._num_of_elem = 0;
    }

    _hashset_base( std::initializer_list<value_type> init,
                   size_type bucket_count,
                   const hasher& hash = hasher(),
                   const key_equal& equal = key_equal(),
                   const Allocator& alloc = Allocator() )
        : _hashset_base(bucket_count, hash, equal, alloc) {
        for(auto it = init.begin(); it != init.end(); ++it)
            insert(*it);
    }

    ~_hashset_base() = default;

    _hashset_base& operator=(const _hashset_base& x) {
        if(this == &x)
            return *this;
        _hf = x._hf;
        _eql = x._eql;
        _tab = x._tab;
        _num_of_elem = x._num_of_elem;
        return *this;
    }

    _hashset_base& operator=(_hashset_base&& x) {
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

    iterator erase(const_iterator position) {
        difference_type dis = panzer::distance(cbegin(), position);
        _tab.erase(position.index, position.cur);
        --_num_of_elem;
        iterator r = begin();
        panzer::advance(r, dis);
        return r;
    }

    size_type erase(const key_type& k) {
        size_type cnt = _tab.erase(k);
        _num_of_elem -= cnt;
        return cnt;
    }

    iterator erase(const_iterator first, const_iterator last) {
        difference_type dis = panzer::distance(cbegin(), first);
        while(first != last) {
            first = erase(first);
        }
        iterator r = begin();
        panzer::advance(r, dis);
        return r;
    }

    void swap(_hashset_base& x) {
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
        auto n = _tab.find(k);
        if(n.first == -1)
            return end();
        else
            return iterator(&_tab, n.first, n.second);
    }

    const_iterator find(const key_type& k) const {
        auto n = _tab.find(k);
        if(n.first == -1)
            return cend();
        else
            return const_iterator(&_tab, n.first, n.second);
    }

    size_type count(const key_type& k) {
        return _tab.count(k);
    }

    std::pair<iterator, iterator>
    equal_range(const key_type& k) {
        iterator first = end(), last = end();
        for(iterator it = begin(); it != end(); ++it) {
            if(_eql(k, *it)) {
                first = it;
                while((it != end()) && _eql(k, *it))
                    ++it;
                last = it;
                break;
            }
        }
        return std::pair<iterator, iterator>(first, last);
    }

    std::pair<const_iterator, const_iterator>
    equal_range(const key_type& k) const {
        const_iterator first = end(), last = end();
        for(const_iterator it = begin(); it != end(); ++it) {
            if(_eql(k, *it)) {
                first = it;
                while((it != end()) && _eql(k, *it))
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

  template<class Key,
           class Hash = std::hash<Key>,
           class Pred = panzer::equal_to<Key>,
           class Allocator = panzer::allocator<Key> >
  class unordered_set
          : public _hashset_base<Key, Hash, Pred, Allocator, false> {
  private:
      typedef _hashset_base<Key, Hash, Pred, Allocator, false> _base;

  public:
      using _base::insert;
      // 构造（继承父类）
      unordered_set() {}

      explicit unordered_set(const Allocator& a)
          : _base(a)
      {}

      explicit unordered_set(typename _base::size_type n,
                             const Hash& hf = Hash(),
                             const Pred& eql = Pred(),
                             const Allocator& a = Allocator())
          : _base(n, hf, eql, a)
      {}

      template<class InputIt>
      unordered_set(InputIt first, InputIt last,
                    typename _base::size_type n = 0,
                    const Hash& hf = Hash(),
                    const Pred& eql = Pred(),
                    const Allocator& a = Allocator())
          : _base(first, last, n, hf, eql, a)
      {}

      unordered_set(typename _base::size_type n,
                    const Allocator& a)
          : _base(n, a)
      {}

      unordered_set(typename _base::size_type n,
                    const Hash& hf,
                    const Allocator& a)
          : _base(n, hf, a)
      {}

      unordered_set(const unordered_set& x,
                    const Allocator& a)
          : _base(x, a)
      {}

      unordered_set(unordered_set&& x,
                    const Allocator& a)
          : _base(static_cast<unordered_set&&>(x), a)
      {}

      unordered_set( std::initializer_list<typename _base::value_type> init,
                     typename _base::size_type bucket_count = 11,
                     const Hash& hash = Hash(),
                     const Pred& equal = Pred(),
                     const Allocator& alloc = Allocator() )
        : _base(init, bucket_count, hash, equal, alloc)
      {}

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

  template<class Key,
           class Hash = std::hash<Key>,
           class Pred = panzer::equal_to<Key>,
           class Allocator = panzer::allocator<Key>>
  class unordered_multiset
          : public _hashset_base<Key, Hash, Pred, Allocator, true> {
  private:
      typedef _hashset_base<Key, Hash, Pred, Allocator, true> _base;

  public:
      using _base::insert;
      // 构造（继承父类）
      unordered_multiset() {}

      explicit unordered_multiset(const Allocator& a)
          : _base(a)
      {}

      explicit unordered_multiset(typename _base::size_type n,
                             const Hash& hf = Hash(),
                             const Pred& eql = Pred(),
                             const Allocator& a = Allocator())
          : _base(n, hf, eql, a)
      {}

      template<class InputIt>
      unordered_multiset(InputIt first, InputIt last,
                    typename _base::size_type n = 0,
                    const Hash& hf = Hash(),
                    const Pred& eql = Pred(),
                    const Allocator& a = Allocator())
          : _base(first, last, n, hf, eql, a)
      {}

      unordered_multiset(typename _base::size_type n,
                         const Allocator& a)
          : _base(n, a)
      {}

      unordered_multiset(typename _base::size_type n,
                         const Hash& hf, const Allocator& a)
          : _base(n, hf, a)
      {}

      unordered_multiset(const unordered_multiset& x,
                         const Allocator& a)
          : _base(x, a)
      {}

      unordered_multiset(unordered_multiset&& x,
                         const Allocator& a)
          : _base(static_cast<unordered_multiset&&>(x), a)
      {}

      unordered_multiset( std::initializer_list<typename _base::value_type> init,
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
  void swap(unordered_set<Key, Hash, Pred, Alloc>& x,
            unordered_set<Key, Hash, Pred, Alloc>& y) {
      x.swap(y);
  }

  template< class Key, class Hash, class KeyEqual, class Alloc >
  void swap( unordered_multiset<Key,Hash,KeyEqual,Alloc>& x,
             unordered_multiset<Key,Hash,KeyEqual,Alloc>& y ) {
      x.swap(y);
  }

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
                   const unordered_set<Key,Hash,KeyEqual,Allocator>& rhs ) {
      return !(lhs == rhs);
  }

  template< class Key, class Hash, class KeyEqual, class Allocator >
  bool operator==( const unordered_multiset<Key,Hash,KeyEqual,Allocator>& lhs,
                   const unordered_multiset<Key,Hash,KeyEqual,Allocator>& rhs ) {
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
  bool operator!=( const unordered_multiset<Key,Hash,KeyEqual,Allocator>& lhs,
                   const unordered_multiset<Key,Hash,KeyEqual,Allocator>& rhs ) {
      return !(lhs == rhs);
  }
}

#endif // JR_UNORDERED_SET_H
