#ifndef JR_SET_H
#define JR_SET_H

#include <cstddef>
#include <utility>
#include "../../functional/jr_functional.h"
#include "../../container/utils/jr_iterators.h"
#include "../../container/utils/jr_tree.h"

namespace panzer {
    template<class Key, class Compare, class Allocator, bool isMultiSet >
    class _set_base {
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
            typedef panzer::reverse_iterator<const_iterator> const_reverse_iterator;
            typedef panzer::reverse_iterator<iterator> reverse_iterator;

        protected:
            typedef _tree_node<Key> tnode;
            typedef _AVL_Tree<Key, isMultiSet, Compare, typename Allocator::template rebind<tnode>::other> tree;
            tree t;
            Compare comp;
            size_type _size;
            Allocator _alloc_data;

        public:
            // 构造/复制/销毁
            _set_base()
                : _size(0), t(), comp(Compare())
            {}

            explicit _set_base(const Allocator& a)
                : _size(0), t(), _alloc_data(a)
            {}

            explicit _set_base(const Compare& c,
                               const Allocator& a = Allocator())
                : _size(0), t(), comp(c), _alloc_data(a)
            {}

            template< class InputIt >
            _set_base( InputIt first, InputIt last,
                 const Compare& c = Compare(),
                 const Allocator& a = Allocator() )
                : _size(0), t(), comp(c), _alloc_data(a) {
                insert(first, last);
            }

            _set_base(const _set_base& x,
                      const Allocator& a)
                : _size(x.size()), t(x.begin(), x.end()), _alloc_data(a)
            {}

            _set_base(_set_base&& x,
                      const Allocator& a)
                : _size(x.size()),
                  t(static_cast<tree&&>(x.t)),
                  _alloc_data(a) {
                x._size = 0;
            }

            _set_base(const _set_base& x)
                : _size(x.size()),
                  t(x.begin(), x.end())
            {}

            _set_base(_set_base&& x)
                : _size(x.size()),
                  t(static_cast<tree&&>(x.t)) {
                x._size = 0;
            }

            _set_base( std::initializer_list<value_type> init,
                       const Compare& comp = Compare(),
                       const Allocator& alloc = Allocator() )
                : _set_base(comp, alloc) {
                for(auto it = init.begin(); it != init.end(); ++it)
                    insert(*it);
            }

            virtual ~_set_base()  = default;

            _set_base& operator=(const _set_base& x) {
                if(this != & x) {
                    _size = x.size();
                    clear();
                    insert(x.begin(), x.end());
                }
                return *this;
            }

            _set_base& operator=(_set_base&& x) {
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
            iterator erase(const_iterator position) {
                difference_type dis = panzer::distance(cbegin(), position);
                if(isMultiSet)
                    t.erase(*position, position._node);
                else
                    t.erase(*position);
                --_size;
                iterator result = begin();
                panzer::advance(result, dis);
                return result;
            }

            size_type erase(const key_type& va) {
                size_type cnt = 0;
                key_type tmp(va);
                while(find(tmp) != end()) {
                    t.erase(tmp);
                    --_size;
                    ++cnt;
                }
                return cnt;
            }

            iterator erase(const_iterator first, const_iterator last) {
                difference_type dis = panzer::distance(cbegin(), first);
                while(first != last)
                    first = erase(first);
                iterator ret = begin();
                panzer::advance(ret, dis);
                return ret;
            }

            void swap(_set_base& other) {
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
                return comp;
            }

            value_compare value_comp() const {
                return comp;
            }

            // set 操作
            iterator find(const key_type& x) {
                return iterator(t.search(x), t.get_header());
            }

            const_iterator find(const key_type& x) const {
                return const_iterator(t.search(x), t.get_header());
            }

            size_type count(const key_type& x) {
                return t.count(x);
            }

            iterator lower_bound(const key_type& x) {
                if(comp(x, t.get_min()->data))
                    return begin();
                if(comp(t.get_max()->data, x))
                    return end();
                iterator it = begin();
                while(comp(*it, x) && (it != end()))
                    ++it;
                if(comp(x, *it))
                    --it;
                return it;
            }

            const_iterator lower_bound(const key_type& x) const {
                if(comp(x, t.get_min()->data))
                    return cbegin();
                if(comp(t.get_max()->data, x))
                    return cend();
                const_iterator it = begin();
                while(comp(*it, x) && (it != end()))
                    ++it;
                if(comp(x, *it))
                    --it;
                return it;
            }

            iterator upper_bound(const key_type& x) {
                if(comp(x, t.get_min()->data))
                    return begin();
                if(comp(t.get_max()->data, x))
                    return end();
                iterator it = begin();
                while(!comp(x, *it) && (it != end()))
                    ++it;
                return it;
            }

            const_iterator upper_bound(const key_type& x) const {
                if(comp(x, t.get_min()->data))
                    return cbegin();
                if(comp(t.get_max()->data, x))
                    return cend();
                const_iterator it = begin();
                while(!comp(x, *it) && (it != end()))
                    ++it;
                return it;
            }

            std::pair<iterator, iterator> equal_range(const key_type& x) {
                return std::pair<iterator, iterator>(lower_bound(x),
                                                     upper_bound(x));
            }

            std::pair<const_iterator, const_iterator> equal_range(const key_type& x) const {
                return std::pair<iterator, iterator>(lower_bound(x),
                                                     upper_bound(x));
            }
    };


    template<class Key, class Compare = panzer::less<Key>,
             class Allocator = panzer::allocator<Key> >
    class set : public _set_base<Key, Compare, Allocator, false> {
        private:
            typedef _set_base<Key, Compare, Allocator, false> _base;

        public:
            using _base::insert;
            // 委托构造（继承父类）
            set() {}

            explicit set(const Allocator& a) : _base(a)
            {}

            explicit set(const Compare& c,
                         const Allocator& a = Allocator())
                : _base(c, a)
            {}

            template< class InputIt >
            set( InputIt first, InputIt last,
                 const Compare& c = Compare(),
                 const Allocator& a = Allocator() )
                : _base(first, last, c, a)
            {}

            set(const set& x, const Allocator& a)
                : _base(x,a)
            {}

            set(set&& x, const Allocator& a)
                : _base(static_cast<set&&>(x), a)
            {}

            set( std::initializer_list<typename _base::value_type> init,
                 const Compare& comp = Compare(),
                 const Allocator& alloc = Allocator() )
                : _base(init, comp, alloc)
            {}

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

    template<class Key, class Compare = panzer::less<Key>,
             class Allocator = panzer::allocator<Key>>
    class multiset : public _set_base<Key, Compare, Allocator, true> {
        private:
            typedef _set_base<Key, Compare, Allocator, true> _base;

        public:
            using _base::insert;
            // 构造（继承父类）
            multiset() {}

            explicit multiset(const Allocator& a) : _base(a)
            {}

            explicit multiset(const Compare& c, const Allocator& a = Allocator())
                : _base(c, a)
            {}

            template< class InputIt >
            multiset( InputIt first, InputIt last,
                 const Compare& c = Compare(),
                 const Allocator& a = Allocator() )
                : _base(first, last, c, a)
            {}

            multiset(const multiset& x, const Allocator& a)
                : _base(x,a)
            {}

            multiset(multiset&& x, const Allocator& a)
                : _base(static_cast<multiset&&>(x), a)
            {}

            multiset( std::initializer_list<typename _base::value_type> init,
                      const Compare& comp = Compare(),
                      const Allocator& alloc = Allocator() )
                : _base(init, comp, alloc)
            {}
            // 特化操作
            template< class... Args >
            typename _base::iterator
            emplace( Args&&... args )
            { return _base::emplace(static_cast<Args&&>(args)...).first; }

            typename _base::iterator
            insert( const typename _base::value_type& value )
            { return _base::insert(value).first; }

            typename _base::iterator
            insert( typename _base::value_type&& value )
            { return _base::insert(static_cast<typename _base::value_type&&>(value)).first; }
    };

    // 交换
    template<class Key, class Compare, class Allocator>
    void swap(set<Key, Compare, Allocator>& x,
            set<Key, Compare, Allocator>& y)
    { x.swap(y); }

    // 交换
    template<class Key, class Compare, class Allocator>
    void swap(multiset<Key, Compare, Allocator>& x,
              multiset<Key, Compare, Allocator>& y)
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
