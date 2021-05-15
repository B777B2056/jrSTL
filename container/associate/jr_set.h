#ifndef JR_SET_H
#define JR_SET_H

#include <cstddef>
#include "../../functional/jr_functional.h"
#include "../../container/utils/se_iterators.h"
#include "../../container/utils/jr_tree.h"
#include "../../container/utils/jr_utility.h"

namespace jr_std {
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

        protected:
            typedef _tree_node<Key> tnode;
            typedef _Balance_BST<Key, isMultiSet, Compare, typename Allocator::template rebind<tnode>::other> tree;
            tree t;
            Compare comp;
            size_type _size;
            Allocator _alloc_data;

        public:
            // 构造/复制/销毁
            _set_base() : _size(0), t(), comp(Compare())
            {}

            explicit _set_base(const Allocator&) : _size(0), t()
            {}

            explicit _set_base(const Compare& c, const Allocator& a = Allocator())
                : _size(0), t(), comp(c), _alloc_data(a)
            {}

            template< class InputIt >
            _set_base( InputIt first, InputIt last,
                 const Compare& c = Compare(),
                 const Allocator& a = Allocator() )
                : _size(static_cast<size_type>(jr_std::distance(first, last))),
                  t(first, last), comp(c), _alloc_data(a)
            {}

            _set_base(const _set_base& x)
                : _size(x.size()), t(x.begin(), x.end())
            {}

            _set_base(_set_base&& x) {
                _size = x._size;
                x._size = 0;
                t = static_cast<tree&&>(x.t);
            }

            _set_base(const _set_base& x, const Allocator& a)
                : _size(x.size()), t(x.begin(), x.end()), _alloc_data(a)
            {}

            _set_base(_set_base&& x, const Allocator& a) : _alloc_data(a) {
                _size = x._size;
                x._size = 0;
                t = static_cast<tree&&>(x.t);
            }

            ~_set_base()  = default;

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
                return pair<iterator, bool>(find(*a), isInsert);
            }

            template<class... Args>
            iterator emplace_hint(const_iterator hint, Args&&... args) {
                value_type *a;
                a = _alloc_data.allocate(1);
                _alloc_data.construct(a, static_cast<Args&&>(args)...);
                bool isInsert = t.insert_hint(*a, hint._node);
                if(isInsert) ++_size;
                return find(*a);
            }

            pair<iterator,bool> insert(const value_type& x) {
                bool isInsert = t.insert(x);
                if(isInsert) ++_size;
                return pair<iterator, bool>(find(x), isInsert);
            }

            pair<iterator,bool> insert(value_type&& x) {
                auto m = static_cast<value_type&&>(x);
                bool isInsert = t.insert(m);
                if(isInsert) ++_size;
                return pair<iterator, bool>(find(x), isInsert);
            }

            iterator insert(const_iterator hint, const value_type& x) {
                bool isInsert = t.insert_hint(x, hint._node);
                if(isInsert) ++_size;
                return find(x);
            }

            iterator insert(const_iterator hint, value_type&& x) {
                auto m = static_cast<value_type&&>(x);
                bool isInsert = t.insert_hint(m, hint._node);
                if(isInsert) ++_size;
                return find(m);
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
                while(find(va) != end()) {
                    t.erase(va);
                    --_size;
                }
            }

            iterator erase(const_iterator first, const_iterator last) {
                iterator result = iterator(last._node, last._header);
                while(first != last) {
                    erase(iterator(first._node, first._header));
                    ++first;
                }
                return result;
            }

            void swap(_set_base& other) {
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


    template<class Key, class Compare = jr_std::less<Key>,
             class Allocator = jr_std::allocator<Key> >
    class set : public _set_base<Key, Compare, Allocator, false> {};

    template<class Key, class Compare = jr_std::less<Key>,
             class Allocator = jr_std::allocator<Key>>
    class multiset : public _set_base<Key, Compare, Allocator, true> {
        private:
            typedef _set_base<Key, Compare, Allocator, true> _base;

        public:
            template< class... Args >
            typename _base::iterator emplace( Args&&... args )
            { return _base::emplace(static_cast<Args&&>(args)...).first; }

            typename _base::iterator insert( const typename _base::value_type& value )
            { return _base::insert(value).first; }

            typename _base::iterator insert( typename _base::value_type&& value )
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
