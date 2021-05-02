#ifndef JR_DEQUE_H
#define JR_DEQUE_H

#include <cstddef>
#include <type_traits>
#include "../../memory/jr_allocator.h"
#include "../iterators.h"

namespace jr_std {

template<class T, class Allocator = allocator<T>, size_t BufSize = 8>
class deque{
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

protected:
    // 管理不同连续存储区的map定义
    typedef T** map_pointer;
    map_pointer _map;
    size_type _map_size;
    // 标识迭代器位置
    iterator _start, _finish;
//    const_iterator cstart, cfinish;
    // 空间配置器实例化
    Allocator _alloc;
    // 将内存分配策略重绑定至map数组所分配的连续存储区域
    typename Allocator::template rebind<_node<T> >::other _alloc_map;

protected:
    void _ctor(size_type count, const T& value, std::true_type) {
        _map_size = count / BufSize + 1;
        _map = new T*[_map_size + 1];
        size_type i, j, cnt = 0;
        for(i = 0; i <= _map_size; i++) {
            _map[i] = _alloc_map.allocate(BufSize + 1);
            for(j = 0; (j < BufSize) && (cnt < count); j++) {
                _alloc.construct(_map[i][j], value);
                ++cnt;
            }
        }
        _start(_map[0]);
        _finish(_map[count / BufSize]);
    }

    template<class InputIt>
    void _ctor(InputIt first, InputIt last, std::false_type) {
        difference_type count = last - first;
        _map_size = count / BufSize + 1;
        _map = new T*[_map_size + 1];
        for(size_type i = 0; i <= _map_size; i++) {
            _map[i] = _alloc_map.allocate(BufSize + 1);
        }
        start(_map[0]);
        finish(_map[0]);
        while(first != last) {
            _alloc.construct(_finish.cur, *first);
            ++first;
            ++_finish;
        }
    }

public:
    // 构造函数
    deque() {
        _map_size = 4;
        _map = new T*[_map_size + 1];
        for(size_type i = 0; i <= _map_size; i++)
            _map[i] = _alloc_map.allocate(BufSize + 1);
        _start(_map[0]);
        _finish(_map[0]);
    }

    explicit deque( const Allocator&) {
        _map_size = 4;
        _map = new T*[_map_size + 1];
        for(size_type i = 0; i <= _map_size; i++)
            _map[i] = _alloc_map.allocate(BufSize + 1);
        _start(_map[0]);
        _finish(_map[0]);
    }

    explicit deque( size_type count ) {
        _ctor(count, T(), std::true_type());
    }

    deque( size_type count, const T& value, const Allocator&) {
        _ctor(count, value, std::true_type());
    }

    template< class InputIt >
    deque( InputIt first, InputIt last, const Allocator&) {
        typedef std::integral_constant<bool, std::is_integral<InputIt>::value> type;
        _ctor(first, last, type());
    }

    deque( const deque& other ) {
        iterator first = other.begin(), last = other.end();
        _map_size = other._map_size;
        _map = new T*[_map_size + 1];
        for(size_type i = 0; i <= _map_size; i++) {
            _map[i] = _alloc_map.allocate(BufSize + 1);
        }
        _start(_map[0]);
        _finish(_map[0]);
        while(first != last) {
            _alloc.construct(_finish.cur, *first);
            ++first;
            ++_finish;
        }
    }

    deque( deque&& other ) {
        _map_size = other._map_size;
        _map = other._map;
        _start = other.start;
        _finish = other.finish;
    }

    // 析构函数
    ~deque() {
        for(size_type i = 0; i <= _map_size; i++) {
            for(size_type j = 0; j < BufSize; j++)
                _alloc.destroy(_map[i][j]);
            _alloc.deallocate(_map[i], BufSize + 1);
        }
    }

    deque& operator=( const deque& other ) {
        iterator first = other.begin(), last = other.end();
        _map_size = other._map_size;
        _map = new T*[_map_size + 1];
        for(size_type i = 0; i <= _map_size; i++) {
            _map[i] = _alloc_map.allocate(BufSize + 1);
        }
        _start(_map[0]);
        _finish(_map[0]);
        while(first != last) {
            _alloc.construct(_finish.cur, *first);
            ++first;
            ++_finish;
        }
        return *this;
    }

    deque& operator=( deque&& other ) {
        _map_size = other._map_size;
        _map = other._map;
        _start = other.start;
        _finish = other.finish;
        return *this;
    }

    // 迭代器
    iterator begin() noexcept { return _start; }
    const_iterator begin() const noexcept { return const_iterator(_start.control_node); }
    iterator end() noexcept { return _finish; }
    const_iterator end() const noexcept { return const_iterator(_finish.control_node); }
    const_iterator cbegin() const noexcept { return const_iterator(_start.control_node); }
    const_iterator cend() const noexcept { return const_iterator(_finish.control_node); }
};

}

#endif // JR_DEQUE_H
