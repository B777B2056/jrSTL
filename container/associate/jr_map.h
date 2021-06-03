#ifndef JR_MAP_H
#define JR_MAP_H

#include <cstddef>
#include <utility>
#include "../../functional/jr_functional.h"
#include "../../container/utils/jr_iterators.h"
#include "../../container/utils/jr_tree.h"

namespace jr_std {
    template<class Key, class T, class Compare,
             class Allocator, bool isMultiMap>
    class _map_base {
        public:
        // 类型
          typedef Key key_type;
          typedef Compare key_compare;
          typedef std::pair<const Key, T> value_type;
          typedef Allocator allocator_type;
          typedef std::pair<const Key, T>* pointer;
          typedef const std::pair<const Key, T>* const_pointer;
          typedef value_type& reference;
          typedef const value_type& const_reference;
          typedef size_t size_type;
          typedef T mapped_type;
          typedef ptrdiff_t difference_type;
          typedef _balance_bst_iterator<value_type, value_type&, value_type*> iterator;
          typedef _balance_bst_iterator<value_type, const value_type&, const value_type*> const_iterator;
          typedef jr_std::reverse_iterator<const_iterator> const_reverse_iterator;
          typedef jr_std::reverse_iterator<iterator> reverse_iterator;

        class value_compare {
          friend class _map_base;
        protected:
          Compare _comp;
          value_compare(Compare c) : _comp(c) { }
            public:
              bool operator()(const value_type& x, const value_type& y) const {
                return _comp(x.first, y.first);
              }
          };

         protected:
            typedef _tree_node<value_type> tnode;
            typedef _AVL_Tree<value_type, isMultiMap, value_compare, Allocator> tree;
            Compare comp;
            tree t;
            size_type _size;
            Allocator _alloc_data;

          public:
            // 构造/复制/销毁
            _map_base()
                : _size(0), comp(Compare()), t(comp)
            {}

            explicit _map_base(const Allocator& a)
                 : _size(0), comp(Compare()),
                   t(comp), _alloc_data(a)
            {}

            explicit _map_base(const Compare& c,
                               const Allocator& a = Allocator())
                 : _size(0), comp(c),
                   t(comp), _alloc_data(a)
            {}

            template<class InputIt>
            _map_base(InputIt first, InputIt last,
                      const Compare& c = Compare(),
                      const Allocator& a = Allocator())
                 : _size(0), comp(c), t(comp), _alloc_data(a) {
                insert(first, last);
            }

            _map_base(const _map_base& x, const Allocator& a)
                : _size(x.size()), comp(Compare()),
                  t(x.begin(), x.end(), comp), _alloc_data(a)
            {}

            _map_base(_map_base&& x, const Allocator& a)
                : _size(x._size), comp(x.key_comp()),
                t(static_cast<tree&&>(x.t), comp), _alloc_data(a) {
                x._size = 0;
            }

            _map_base(const _map_base& x)
                : _size(x.size()), comp(x.key_comp()),
                  t(x.begin(), x.end(), comp)
            {}

            _map_base(_map_base&& x)
                : _size(x._size), comp(x.key_comp()),
                t(static_cast<tree&&>(x.t), comp) {
                x._size = 0;
            }

            _map_base( std::initializer_list<value_type> init,
                       const Compare& comp = Compare(),
                       const Allocator& alloc = Allocator() )
                : _map_base(comp, alloc) {
                for(auto it = init.begin(); it != init.end(); ++it)
                    insert(*it);
            }

            virtual ~_map_base() = default;

            _map_base& operator=(const _map_base& x) {
                if(this != & x) {
                    _size = x.size();
                    clear();
                    insert(x.begin(), x.end());
                }
                return *this;
            }

            _map_base& operator=(_map_base&& x) {
                if(this != &x) {
                    _size = x._size;
                    x._size = 0;
                    t = static_cast<tree&&>(x.t);
                }
                return *this;
            }

            allocator_type get_allocator() const noexcept {
                return _alloc_data;
            }

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

            reverse_iterator rbegin() noexcept {
                return reverse_iterator(end());
            }

            const_reverse_iterator rbegin() const noexcept {
                return const_reverse_iterator(end());
            }

            const_reverse_iterator crbegin() const noexcept {
                return const_reverse_iterator(end());
            }

            reverse_iterator rend() noexcept {
                return reverse_iterator(begin());
            }

            const_reverse_iterator rend() const noexcept {
                return const_reverse_iterator(begin());
            }

            const_reverse_iterator crend() const noexcept {
                return const_reverse_iterator(begin());
            }

            // 容量
            bool empty() const noexcept {
                return _size == 0;
            }

            size_type size() const noexcept {
                return _size;
            }

            size_type max_size() const noexcept {
                return UINT_MAX;
            }

        // 修改器
        protected:
            template<class... Args>
            std::pair<iterator, bool> emplace(Args&&... args) {
                bool flag;
                value_type *a;
                a = _alloc_data.allocate(1);
                _alloc_data.construct(a, static_cast<Args&&>(args)...);
                tnode *ret = t.insert(*a, flag);
                if(flag)
                    ++_size;
                return std::pair<iterator, bool>(iterator(ret, t.get_header()), flag);
            }

        public:
            template<class... Args>
            iterator emplace_hint(const_iterator hint, Args&&... args) {
                bool flag;
                value_type *a;
                a = _alloc_data.allocate(1);
                _alloc_data.construct(a, static_cast<Args&&>(args)...);
                tnode *ret = t.insert_hint(*a, hint._node, flag);
                if(flag)
                    ++_size;
                return iterator(ret, t.get_header());
            }

        protected:
            std::pair<iterator,bool> insert(const value_type& x) {
                bool flag;
                tnode *ret = t.insert(x, flag);
                if(flag) {
                    ++_size;
                }
                return std::pair<iterator, bool>(iterator(ret, t.get_header()), flag);
            }

            std::pair<iterator,bool> insert(value_type&& x) {
                bool flag;
                auto m = static_cast<value_type&&>(x);
                tnode *ret = t.insert(m, flag);
                if(flag) {
                    ++_size;
                }
                return std::pair<iterator, bool>(iterator(ret, t.get_header()), flag);
            }

        public:
            iterator insert(const_iterator hint, const value_type& x) {
                bool flag;
                tnode *ret = t.insert_hint(x, hint._node, flag);
                if(flag) {
                    ++_size;
                }
                return iterator(ret, t.get_header());
            }

            iterator insert(const_iterator hint, value_type&& x) {
                bool flag;
                auto m = static_cast<value_type&&>(x);
                tnode *ret = t.insert_hint(m, hint._node, flag);
                if(flag) {
                    ++_size;
                }
                return iterator(ret, t.get_header());
            }

            void insert( std::initializer_list<value_type> ilist ) {
                for(auto it = ilist.begin(); it != ilist.end(); ++it)
                    insert(*it);
            }

            template< class InputIt >
            void insert( InputIt first, InputIt last ) {
                while(first != last) {
                    bool flag;
                    t.insert(*first, flag);
                    if(flag)
                        ++_size;
                    ++first;
                }
            }

            size_type erase(const key_type& va) {
                size_type cnt = 0;
                key_type tmp(va);
                while(find(tmp) != end()) {
                    t.erase(value_type(tmp, T()));
                    --_size;
                    ++cnt;
                }
                return cnt;
            }

            iterator erase(const_iterator position) {
                difference_type dis = jr_std::distance(cbegin(), position);
                if(isMultiMap)
                    t.erase(*position, position._node);
                else
                    t.erase(*position);
                --_size;
                iterator result = begin();
                jr_std::advance(result, dis);
                return result;
            }

            iterator erase(const_iterator first, const_iterator last) {
                difference_type dis = jr_std::distance(cbegin(), first);
                while(first != last) {
                    iterator it = erase(first);
                    first = cbegin();
                    jr_std::advance(first, jr_std::distance(begin(), it));
                }
                iterator ret = begin();
                jr_std::advance(ret, dis);
                return ret;
            }

            virtual void swap(_map_base& other) {
                if(this == &other)
                    return;
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
            key_compare key_comp() const {
                return Compare();
            }

            value_compare value_comp() const {
                return comp;
            }

            // map 操作
            iterator find(const key_type& x) {
                return iterator(t.search(value_type(x, T())),
                                t.get_header());
            }

            const_iterator find(const key_type& x) const {
                return const_iterator(t.search(value_type(x, T())),
                                      t.get_header());
            }

            size_type count(const key_type& x) const {
                return t.count(value_type(x, T()));
            }

            iterator lower_bound(const key_type& x) {
                if(comp(x, t.get_min()->data.first))
                    return begin();
                if(comp(t.get_max()->data.first, x))
                    return end();
                iterator it = begin();
                while(comp(it->first, x) && (it != end()))
                    ++it;
                if(comp(x, it->first))
                    --it;
                return it;
            }

            const_iterator lower_bound(const key_type& x) const {
                if(comp(x, t.get_min()->data.first))
                    return cbegin();
                if(comp(t.get_max()->data.first, x))
                    return cend();
                const_iterator it = begin();
                while(comp(it->first, x) && (it != end()))
                    ++it;
                if(comp(x, it->first))
                    --it;
                return it;
            }

            iterator upper_bound(const key_type& x) {
                if(comp(x, t.get_min()->data.first))
                    return begin();
                if(comp(t.get_max()->data.first, x))
                    return end();
                iterator it = begin();
                while(!comp(x, it->first) && (it != end()))
                    ++it;
                return it;
            }

            const_iterator upper_bound(const key_type& x) const {
                if(comp(x, t.get_min()->data.first))
                    return cbegin();
                if(comp(t.get_max()->data.first, x))
                    return cend();
                const_iterator it = begin();
                while(!comp(x, it->first) && (it != end()))
                    ++it;
                return it;
            }

            std::pair<iterator, iterator>
            equal_range(const key_type& x) {
                return std::pair<iterator, iterator>(lower_bound(x),
                                                     upper_bound(x));
            }

            std::pair<const_iterator, const_iterator>
            equal_range(const key_type& x) const {
                return std::pair<iterator, iterator>(lower_bound(x),
                                                     upper_bound(x));
            }
    };

    template<class Key, class T, class Compare = jr_std::less<Key>,
             class Allocator = jr_std::allocator<std::pair<const Key, T> > >
    class map : public _map_base<Key, T, Compare, Allocator, false> {
      private:
          typedef _map_base<Key, T, Compare, Allocator, false> _base;

      public:
          using _base::insert;
          // 构造（继承父类）
          map() {}

          explicit map(const Allocator& a)
               : _base(a)
          {}

          explicit map(const Compare& c, const Allocator& a = Allocator())
               : _base(c, a)
          {}

          template<class InputIt>
          map(InputIt first, InputIt last,
              const Compare& c = Compare(), const Allocator& a = Allocator())
               : _base(first, last, c, a)
          {}

          map(const map& x, const Allocator& a)
              : _base(x, a)
          {}

          map(map&& x, const Allocator& a)
              : _base(static_cast<map&&>(x), a)
          {}

          map( std::initializer_list<typename _base::value_type> init,
               const Compare& comp = Compare(),
               const Allocator& alloc = Allocator() )
              : _base(init, comp, alloc)
          {}
          // 元素访问
          typename _base::mapped_type& operator[](const typename _base::key_type& x) {
              return (*((_base::insert(typename _base::value_type(x, T()))).first)).second;
          }

          typename _base::mapped_type& operator[](typename _base::key_type&& x) {
              return (*((_base::insert(typename _base::value_type(static_cast<typename _base::key_type&&>(x), T()))).first)).second;
          }

          typename _base::mapped_type& at(const typename _base::key_type& x)
          { return (*this)[x]; }

          const typename _base::mapped_type& at(const typename _base::key_type& x) const
          { return (*this)[x]; }

          template< class... Args >
          std::pair<typename _base::iterator, bool>
          emplace( Args&&... args )
          { return _base::emplace(static_cast<Args&&>(args)...); }

          std::pair<typename _base::iterator, bool>
          insert( const typename _base::value_type& value )
          { return _base::insert(value); }

          std::pair<typename _base::iterator, bool>
          insert( typename _base::value_type&& value )
          { return _base::insert(static_cast<typename _base::value_type&&>(value)); }
    };

    template<class Key, class T, class Compare = less<Key>,
             class Allocator = allocator<std::pair<const Key, T> > >
    class multimap : public _map_base<Key, T, Compare, Allocator, true> {
        private:
            typedef _map_base<Key, T, Compare, Allocator, true> _base;

        public:
            using _base::insert;
            // 构造（继承父类）
            multimap() {}

            explicit multimap(const Allocator& a)
                 : _base(a)
            {}

            explicit multimap(const Compare& c, const Allocator& a = Allocator())
                 : _base(c, a)
            {}

            template<class InputIt>
            multimap(InputIt first, InputIt last,
                const Compare& c = Compare(), const Allocator& a = Allocator())
                 : _base(first, last, c, a)
            {}

            multimap(const multimap& x, const Allocator& a)
                : _base(x, a)
            {}

            multimap(multimap&& x, const Allocator& a)
                : _base(static_cast<multimap&&>(x), a)
            {}

            multimap( std::initializer_list<typename _base::value_type> init,
                      const Compare& comp = Compare(),
                      const Allocator& alloc = Allocator() )
                : _base(init, comp, alloc)
            {}
            // 特化操作
            template< class... Args >
            typename _base::iterator
            emplace( Args&&... args )
            { return (_base::emplace(static_cast<Args&&>(args)...)).first; }

            typename _base::iterator
            insert( const typename _base::value_type& value )
            { return (_base::insert(value)).first; }

            typename _base::iterator
            insert( typename _base::value_type&& value )
            { return (_base::insert(static_cast<typename _base::value_type&&>(value))).first; }
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
