#ifndef JR_DEQUE_H
#define JR_DEQUE_H

#include <cstddef>
#include <type_traits>
#include "../../memory/jr_allocator.h"
#include "../utils/jr_iterators.h"

namespace jr_std {
    template<class T, class Allocator = allocator<T>, size_t BufSize = 8>
    class deque {
        public:
            // 通用定义
            typedef T value_type;
            typedef Allocator allocator_type;
            typedef size_t size_type;
            typedef ptrdiff_t difference_type;
            typedef T& reference;
            typedef const T& const_reference;
            typedef T* pointer;
            typedef const T* const_pointer;
            typedef _deque_iterator<T, T&, T*, BufSize> iterator;
            typedef _deque_iterator<T, const T&, const T*, BufSize> const_iterator;
            typedef jr_std::reverse_iterator<const_iterator> const_reverse_iterator;
            typedef jr_std::reverse_iterator<iterator> reverse_iterator;

        protected:
            // 管理不同连续存储区的map定义
            typedef T** map_pointer;
            map_pointer _map;
            size_type _map_size;
            // 标识迭代器位置
            iterator _start, _finish;
            // 空间配置器实例化
            Allocator _alloc;
            // 将内存分配策略重绑定至map数组所分配的连续存储区域
            typename Allocator::template rebind<pointer>::other _alloc_map;

        protected:
            void _init(size_type map_size) {
                _map = _alloc_map.allocate(map_size + 1);
                for(size_type i = 0; i <= _map_size; i++)
                    _map[i] = _alloc.allocate(BufSize + 1);
                _start.jmp_node(&_map[0]);
                _finish.jmp_node(&_map[0]);
            }

            void _free_all() {
                for(size_type i = 0; i <= _map_size; i++) {
                    for(size_type j = 0; j <= BufSize; j++)
                        _alloc.destroy(&_map[i][j]);
                    _alloc.deallocate(_map[i], BufSize + 1);
                }
                _alloc_map.deallocate(_map, _map_size + 1);
            }

            void _ctor(size_type count, const T& value, std::true_type) {
                _map_size = count / BufSize + 1;
                _init(_map_size);
                size_type i, j, cnt = 0;
                for(i = 0; i <= _map_size; i++) {
                    for(j = 0; (j < BufSize) && (cnt < count); j++) {
                        _alloc.construct(&_map[i][j], value);
                        ++cnt;
                    }
                }
                _finish += count;
            }

            template<class InputIt>
            void _ctor(InputIt first, InputIt last, std::false_type) {
                difference_type count = last - first;
                _map_size = count / BufSize + 1;
                _init(_map_size);
                while(first != last) {
                    _alloc.construct(_finish.cur, *first);
                    ++first;
                    ++_finish;
                }
            }

            void _copy(const deque& other) {
                iterator first = other._start;
                iterator last = other._finish;
                _ctor(first, last, std::false_type());
            }

            void _move(deque&& other) {
                _map_size = other._map_size;
                _map = other._map;
                _start = std::move(other._start);
                _finish = std::move(other._finish);
                other._map_size = 1;
                other._init(other._map_size);
            }

            void _move_map(size_type n) {
                size_type tmp_sz = _map_size;
                _map_size = n / BufSize + 1;
                map_pointer _map_tmp = _map;
                iterator first = _start, last = _finish;
                _map = _alloc_map.allocate(_map_size + 1);
                for(size_type i = 0; i <= _map_size; i++) {
                    _map[i] = _alloc.allocate(BufSize + 1);
                }
                _start.jmp_node(&_map[0]);
                _finish.jmp_node(&_map[0]);
                while(first != last) {
                    _alloc.construct(_finish.cur, *first);
                    ++first;
                    ++_finish;
                }
                for(size_type i = 0; i <= tmp_sz; i++) {
                    for(size_type j = 0; j < BufSize; j++)
                        _alloc.destroy(&_map_tmp[i][j]);
                    _alloc.deallocate(_map_tmp[i], BufSize + 1);
                }
            }

            iterator _move_elements_forward_n(const_iterator pos, difference_type n) {
                iterator it = iterator(pos.control_node, pos.cur);
                if(it >= _finish) {
                    _finish += n;
                    return _finish;
                }
                iterator tmp = _finish - 1;
                _finish += n;
                for(; tmp != it - 1; tmp--) {
                    _alloc.construct(&(*(tmp + n)), *tmp);
                }
                it += n;
                return it;
            }

            iterator _insert(const_iterator pos, size_type count,
                             const T& value, std::true_type) {
                size_type osz = size();
                if(osz + count > BufSize * _map_size) {
                    difference_type pdis = jr_std::distance(cbegin(), pos);
                    _move_map(osz + count);
                    pos = cbegin();
                    jr_std::advance(pos, pdis);
                }
                iterator i_pos = _move_elements_forward_n(pos,
                                                          static_cast<difference_type>(count));
                iterator before_pos = i_pos - count, tmp = before_pos;
                while(tmp != i_pos) {
                    _alloc.construct(&(*tmp), value);
                    ++tmp;
                }
                return before_pos;
            }

            template<class InputIt>
            iterator _insert(const_iterator pos,
                             InputIt first, InputIt last,
                             std::false_type){
                difference_type dis = pos - cbegin();
                while(first != last) {
                    iterator it = insert(pos, *first);
                    pos = cbegin() + (it - begin());
                    ++pos;
                    ++first;
                }
                return begin() + dis;
            }

        public:
            // 构造函数
            deque() : _map_size(1) {
                _init(_map_size);
            }

            explicit deque( const Allocator& a)
                : _map_size(1), _alloc(a) {
                _init(_map_size);
            }

            explicit deque( size_type count ) {
                _ctor(count, T(), std::true_type());
            }

            deque( size_type count, const T& value,
                   const Allocator& a = Allocator())
                : _alloc(a) {
                _ctor(count, value, std::true_type());
            }

            template< class InputIt >
            deque( InputIt first, InputIt last,
                   const Allocator& a = Allocator())
                : _alloc(a) {
                typedef std::integral_constant<bool, std::is_integral<InputIt>::value> type;
                _ctor(first, last, type());
            }

            deque( const deque& other )
                : _map_size(0){
                if(this == &other)
                    return;
                _copy(other);
            }

            deque( deque&& other ) {
                if(this == &other)
                    return;
                _move(static_cast<deque&&>(other));
            }

            deque( std::initializer_list<T> init,
                   const Allocator& a = Allocator() )
                : deque(a) {
                for(auto it = init.begin(); it != init.end(); ++it)
                    push_back(*it);
            }

            // 析构函数
            ~deque() {
                _free_all();
            }

            // 拷贝重载
            deque& operator=( const deque& other ) {
                if(this == &other)
                    return *this;
                clear();
                _copy(other);
                return *this;
            }
            // 移动重载
            deque& operator=( deque&& other ) {
                if(this == &other)
                    return *this;
                _free_all();
                _move(static_cast<deque&&>(other));
                return *this;
            }
            // 赋值操作
            void assign( size_type count, const T& value ) {
                clear();
                _ctor(count, value, std::true_type());
            }

            void assign( std::initializer_list<T> ilist ) {
                clear();
                for(auto it = ilist.begin(); it != ilist.end(); ++it)
                    push_back(*it);
            }

            template< class InputIt >
            void assign( InputIt first, InputIt last ) {
                clear();
                typedef std::integral_constant<bool, std::is_integral<InputIt>::value> type;
                _ctor(first, last, type());
            }

            allocator_type get_allocator() const noexcept {
                return _alloc;
            }

            // 迭代器
            iterator begin() noexcept {
                return _start;
            }

            const_iterator begin() const noexcept {
                return const_iterator(_start.control_node);
            }

            iterator end() noexcept {
                return _finish;
            }

            const_iterator end() const noexcept {
                return const_iterator(_finish.control_node, _finish.cur);
            }

            const_iterator cbegin() const noexcept {
                return const_iterator(_start.control_node);
            }

            const_iterator cend() const noexcept {
                return const_iterator(_finish.control_node, _finish.cur);
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

            // 元素访问
            reference operator[](size_type n) {
                return *(_start + n);
            }

            const_reference operator[](size_type n) const  {
                return *(cbegin() + n);
            }

            reference at(size_type n) {
                return (*this)[n];
            }

            const_reference at(size_type n) const {
                return (*this)[n];
            }

            reference front() {
                return *_start;
            }

            const_reference front() const {
                return *_start;
            }

            reference back() {
                return *(_finish - 1);
            }

            const_reference back() const {
                return *(_finish - 1);
            }

            // 容量
            bool empty() const noexcept {
                return _start == _finish;
            }

            size_type size() const noexcept {
                return _finish - _start;
            }

            size_type max_size() const noexcept {
                return UINT_MAX;
            }

            void resize(size_type sz, const T& c) {
                size_type dis, osz = size();
                if(sz < osz) {
                    dis = osz - sz;
                    while(dis--) {
                        _alloc.destroy(_finish.cur);
                        --_finish;
                    }
                } else if(sz > osz) {
                    dis = sz - osz;
                    if(sz > _map_size * BufSize) {
                        _move_map(sz);
                    }
                    while(dis--) {
                        _alloc.construct(_finish.cur, c);
                        ++_finish;
                    }
                }
            }

            void resize(size_type sz) {
                resize(sz, T());
            }

            void shrink_to_fit() {
                _move_map(size());
            }

            void clear() noexcept {
                if(!_map_size)
                    return;
                for(size_type i = 0; i <= _map_size; i++) {
                    for(size_type j = 0; j < BufSize; j++)
                        _alloc.destroy(&_map[i][j]);
                }
                _start.jmp_node(&_map[0]);
                _finish.jmp_node(&_map[0]);
            }

            iterator insert( const_iterator pos, size_type count, const T& value ) {
                return _insert(pos, count, value, std::true_type());
            }

            iterator insert( const_iterator pos, const T& value ) {
                return insert(pos, 1, value);
            }

            iterator insert( const_iterator pos, T&& value ) {
                size_type osz = size();
                if(osz + 1 > BufSize * _map_size) {
                    difference_type pdis = pos - cbegin();
                    _move_map(osz + 1);
                    pos = cbegin();
                    pos += pdis;
                }
                iterator i_pos = _move_elements_forward_n(pos, 1);
                iterator before_pos = i_pos - 1, tmp = before_pos;
                while(tmp != i_pos) {
                    _alloc.construct(&(*tmp), static_cast<T&&>(value));
                    ++tmp;
                }
                return before_pos;
            }

            iterator insert( const_iterator pos, std::initializer_list<T> ilist ) {
                difference_type dis = pos - cbegin();
                for(auto it = ilist.begin(); it != ilist.end(); ++it) {
                    iterator t = insert(pos, *it);
                    pos = cbegin() + (t - begin());
                    ++pos;
                }
                return begin() + dis;
            }

            template< class InputIt >
            iterator insert( const_iterator pos, InputIt first, InputIt last ) {
                typedef std::integral_constant<bool, std::is_integral<InputIt>::value> type;
                return _insert(pos, first, last, type());
            }

            iterator erase( const_iterator first, const_iterator last ) {
                difference_type n = last - first;
                iterator it = iterator(last.control_node, last.cur);
                if(first == last)
                    return it;
                iterator tmp = _finish;
                _finish -= n;
                for(; it != tmp; it++) {
                    *(it - n) = *it;
                }
                it -= n;
                return it;
            }

            iterator erase( const_iterator pos ) {
                if(pos == cend())
                    return end();
                return erase(pos, pos + 1);
            }

            void push_back( const T& value ) {
                insert(cend(), value);
            }

            void push_front( const T& value ) {
                insert(cbegin(), value);
            }

            void pop_front() {
                erase(cbegin());
            }

            void pop_back() {
                erase(cend() - 1);
            }

            template< class... Args >
            iterator emplace( const_iterator pos, Args&&... args ) {
                size_type osz = size();
                if(osz + 1 > BufSize * _map_size) {
                    difference_type pdis = pos - cbegin();
                    _move_map(osz + 1);
                    pos = cbegin() + pdis;
                }
                iterator i_pos = _move_elements_forward_n(pos, 1);
                iterator before_pos = i_pos - 1, tmp = before_pos;
                while(tmp != i_pos) {
                    _alloc.construct(&(*tmp), static_cast<Args&&>(args)...);
                    ++tmp;
                }
                return before_pos;
            }

            template< class... Args >
            void emplace_front( Args&&... args ) {
                emplace(cbegin(), static_cast<Args&&>(args)...);
            }

            template< class... Args >
            void emplace_back( Args&&... args ) {
                emplace(cend(), static_cast<Args&&>(args)...);
            }

            void push_front( T&& value ) {
                insert(cbegin(), static_cast<T&&>(value));
            }

            void push_back( T&& value ) {
                insert(cend(), static_cast<T&&>(value));
            }

            void swap( deque& other ) {
                if(this == &other)
                    return;
                {
                    size_type st = _map_size;
                    _map_size = other._map_size;
                    other._map_size = st;
                }
                {
                    map_pointer mt = _map;
                    _map = other._map;
                    other._map = mt;
                }
                {
                    iterator ist = _start;
                    _start = other._start;
                    other._start = ist;
                }
                {
                    iterator ift = _finish;
                    _finish = other._finish;
                    other._finish = ift;
                }
            }
    };

    template< class T, class Alloc >
    void swap( jr_std::deque<T,Alloc>& lhs,
               jr_std::deque<T,Alloc>& rhs ) {
        lhs.swap(rhs);
    }

    template< class T, class Alloc >
    bool operator==( const jr_std::deque<T,Alloc>& lhs,
                     const jr_std::deque<T,Alloc>& rhs ) {
        size_t lsz = lhs.size(), rsz = rhs.size();
        if(lsz != rsz)
            return false;
        for(size_t i = 0; i < lsz; i++) {
            if(lhs[i] != rhs[i])
                return false;
        }
        return true;
    }

    template< class T, class Alloc >
    bool operator!=( const jr_std::deque<T,Alloc>& lhs,
                     const jr_std::deque<T,Alloc>& rhs ) {
        return !(lhs == rhs);
    }

    template< class T, class Alloc >
    bool operator<( const jr_std::deque<T,Alloc>& lhs,
                    const jr_std::deque<T,Alloc>& rhs ) {
        size_t lsz = lhs.size(), rsz = rhs.size();
        if(lsz < rsz)
            return true;
        if(lsz > rsz)
            return false;
        for(size_t i = 0; i < lsz; i++) {
            if(lhs[i] >= rhs[i])
                return false;
        }
        return true;
    }

    template< class T, class Alloc >
    bool operator>( const jr_std::deque<T,Alloc>& lhs,
                    const jr_std::deque<T,Alloc>& rhs ) {
        size_t lsz = lhs.size(), rsz = rhs.size();
        if(lsz < rsz)
            return false;
        if(lsz > rsz)
            return true;
        for(size_t i = 0; i < lsz; i++) {
            if(lhs[i] <= rhs[i])
                return false;
        }
        return true;
    }

    template< class T, class Alloc >
    bool operator<=( const jr_std::deque<T,Alloc>& lhs,
                     const jr_std::deque<T,Alloc>& rhs ) {
        return !(lhs > rhs);
    }

    template< class T, class Alloc >
    bool operator>=( const jr_std::deque<T,Alloc>& lhs,
                     const jr_std::deque<T,Alloc>& rhs ) {
        return !(lhs < rhs);
    }
}

#endif // JR_DEQUE_H
