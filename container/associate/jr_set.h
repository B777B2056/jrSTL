#ifndef JR_SET_H
#define JR_SET_H

#include <cstddef>
#include <type_traits>
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
    using key_type               = Key;
    using key_compare            = Compare;
    using value_type             = Key;
    using value_compare          = Compare;
    using allocator_type         = Allocator;
    using pointer                = Key*;
    using const_pointer          = const Key*;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using size_type              = size_t;
    using difference_type        = ptrdiff_t;
    using iterator               = _balance_bst_iterator<Key, const Key&, const Key*>;
    using const_iterator         = _balance_bst_iterator<Key, const Key&, const Key*>;

  private:
    typedef _tree_node<Key> tnode;
    typedef _Balance_BST<Key, false, Compare, typename Allocator::template rebind<tnode>::other> tree;
    int _size;
    tree t;
    Compare comp;
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

    set(const set& x) = default;

    set(set&& x) = default;

    set(const set& x, const Allocator& a) : _alloc_data(a) {
        *this(x);
    }

    set(set&& x, const Allocator& a) : _alloc_data(a) {
        *this(static_cast<set&&>(x));
    }

    ~set()  = default;

    set& operator=(const set& x) = default;

    set& operator=(set&& x) = default;

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
        bool isInsert = t.insert(static_cast<Args&&>(args)...);
        if(isInsert) ++_size;
        return pair<iterator, bool>(iterator(t.search(static_cast<Args&&>(args)...), t.get_header()), isInsert);
    }

    template<class... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args) {
        bool isInsert = t.insert_hint(static_cast<Args&&>(args)..., hint._node);
        if(isInsert) ++_size;
        return iterator(t.search(static_cast<Args&&>(args)...));
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
        if(t.search(va)) {
            t.erase();
            --_size;
        }
        return result;
    }

    void erase(const key_type& va) {
        if(t.search(va)) {
            t.erase();
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
            tree m = t;
            t = other.t;
            other.t = m;
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

    size_type count(const key_type& x) const { return t.search(x) ? 1 : 0; }

    iterator lower_bound(const key_type& x) {
        iterator it = begin();
        while(comp(*it, x))
            ++it;
        return it;
    }

    const_iterator lower_bound(const key_type& x) const {
        const_iterator it = begin();
        while(comp(*it, x))
            ++it;
        return it;
    }

    iterator upper_bound(const key_type& x) {
        iterator it = begin();
        while(!comp(x, *it))
            ++it;
        return it;
    }

    const_iterator upper_bound(const key_type& x) const {
        const_iterator it = begin();
        while(!comp(x, *it))
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

//    template<class Key, class Compare = jr_std::less<Key>,
//               class Allocator = jr_std::allocator<Key>>
//      class multiset {
//      public:
//        // 类型
//        using key_type               = Key;
//        using key_compare            = Compare;
//        using value_type             = Key;
//        using value_compare          = Compare;
//        using allocator_type         = Allocator;
//        using pointer                = typename allocator_traits<Allocator>::pointer;
//        using const_pointer          = typename allocator_traits<Allocator>::const_pointer;
//        using reference              = value_type&;
//        using const_reference        = const value_type&;
//        using size_type              = /* 由实现定义 */;
//        using difference_type        = /* 由实现定义 */;
//        using iterator               = /* 由实现定义 */;
//        using const_iterator         = /* 由实现定义 */;
//        using reverse_iterator       = std::reverse_iterator<iterator>;
//        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
//        using node_type              = /* 未指明 */;

//        // 构造/复制/销毁
//        multiset() : multiset(Compare()) { }
//        explicit multiset(const Compare& comp, const Allocator& = Allocator());
//        template<class InputIt>
//          multiset(InputIt first, InputIt last,
//                   const Compare& comp = Compare(), const Allocator& = Allocator());
//        multiset(const multiset& x);
//        multiset(multiset&& x);
//        explicit multiset(const Allocator&);
//        multiset(const multiset&, const Allocator&);
//        multiset(multiset&&, const Allocator&);
//        multiset(initializer_list<value_type>, const Compare& = Compare(),
//                 const Allocator& = Allocator());
//        template<class InputIt>
//          multiset(InputIt first, InputIt last, const Allocator& a)
//            : multiset(first, last, Compare(), a) { }
//        multiset(initializer_list<value_type> il, const Allocator& a)
//          : multiset(il, Compare(), a) { }
//        ~multiset();
//        multiset& operator=(const multiset& x);
//        multiset& operator=(multiset&& x)
//          noexcept(allocator_traits<Allocator>::is_always_equal::value &&
//                   is_nothrow_move_assignable_v<Compare>);
//        multiset& operator=(initializer_list<value_type>);
//        allocator_type get_allocator() const noexcept;

//        // 迭代器
//        iterator               begin() noexcept;
//        const_iterator         begin() const noexcept;
//        iterator               end() noexcept;
//        const_iterator         end() const noexcept;

//        reverse_iterator       rbegin() noexcept;
//        const_reverse_iterator rbegin() const noexcept;
//        reverse_iterator       rend() noexcept;
//        const_reverse_iterator rend() const noexcept;

//        const_iterator         cbegin() const noexcept;
//        const_iterator         cend() const noexcept;
//        const_reverse_iterator crbegin() const noexcept;
//        const_reverse_iterator crend() const noexcept;

//        // 容量
//        [[nodiscard]] bool empty() const noexcept;
//        size_type size() const noexcept;
//        size_type max_size() const noexcept;

//        // 修改器
//        template<class... Args> iterator emplace(Args&&... args);
//        template<class... Args> iterator emplace_hint(const_iterator position, Args&&... args);
//        iterator insert(const value_type& x);
//        iterator insert(value_type&& x);
//        iterator insert(const_iterator position, const value_type& x);
//        iterator insert(const_iterator position, value_type&& x);
//        template<class InputIt>
//          void insert(InputIt first, InputIt last);
//        void insert(initializer_list<value_type>);

//        node_type extract(const_iterator position);
//        node_type extract(const key_type& x);
//        iterator insert(node_type&& nh);
//        iterator insert(const_iterator hint, node_type&& nh);

//        iterator  erase(iterator position);
//        iterator  erase(const_iterator position);
//        size_type erase(const key_type& x);
//        iterator  erase(const_iterator first, const_iterator last);
//        void      swap(multiset&)
//          noexcept(allocator_traits<Allocator>::is_always_equal::value &&
//                   is_nothrow_swappable_v<Compare>);
//        void      clear() noexcept;

//        template<class C2>
//          void merge(multiset<Key, C2, Allocator>& source);
//        template<class C2>
//          void merge(multiset<Key, C2, Allocator>&& source);
//        template<class C2>
//          void merge(set<Key, C2, Allocator>& source);
//        template<class C2>
//          void merge(set<Key, C2, Allocator>&& source);

//        // 观察器
//        key_compare key_comp() const;
//        value_compare value_comp() const;

//        // set 操作
//        iterator       find(const key_type& x);
//        const_iterator find(const key_type& x) const;
//        template<class K> iterator       find(const K& x);
//        template<class K> const_iterator find(const K& x) const;

//        size_type      count(const key_type& x) const;
//        template<class K> size_type count(const K& x) const;

//        bool           contains(const key_type& x) const;
//        template<class K> bool contains(const K& x) const;

//        iterator       lower_bound(const key_type& x);
//        const_iterator lower_bound(const key_type& x) const;
//        template<class K> iterator       lower_bound(const K& x);
//        template<class K> const_iterator lower_bound(const K& x) const;

//        iterator       upper_bound(const key_type& x);
//        const_iterator upper_bound(const key_type& x) const;
//        template<class K> iterator       upper_bound(const K& x);
//        template<class K> const_iterator upper_bound(const K& x) const;

//        pair<iterator, iterator>               equal_range(const key_type& x);
//        pair<const_iterator, const_iterator>   equal_range(const key_type& x) const;
//        template<class K>
//          pair<iterator, iterator>             equal_range(const K& x);
//        template<class K>
//          pair<const_iterator, const_iterator> equal_range(const K& x) const;
//      };

//      template<class InputIt,
//               class Compare = less</*iter-value-type*/<InputIt>>,
//               class Allocator = allocator</*iter-value-type*/<InputIt>>>
//        multiset(InputIt, InputIt,
//                 Compare = Compare(), Allocator = Allocator())
//          -> multiset</*iter-value-type*/<InputIt>, Compare, Allocator>;

//      template<class Key, class Compare = less<Key>, class Allocator = allocator<Key>>
//        multiset(initializer_list<Key>, Compare = Compare(), Allocator = Allocator())
//          -> multiset<Key, Compare, Allocator>;

//      template<class InputIt, class Allocator>
//        multiset(InputIt, InputIt, Allocator)
//          -> multiset</*iter-value-type*/<InputIt>,
//                      less<iter-value-type<InputIt>>, Allocator>;

//      template<class Key, class Allocator>
//        multiset(initializer_list<Key>, Allocator) -> multiset<Key, less<Key>, Allocator>;

//      // 交换
//      template<class Key, class Compare, class Allocator>
//        void swap(multiset<Key, Compare, Allocator>& x,
//                  multiset<Key, Compare, Allocator>& y)
//          noexcept(noexcept(x.swap(y)));
}

#endif // JR_SET_H
