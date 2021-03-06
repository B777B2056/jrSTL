#ifndef JR_VECTOR_H
#define JR_VECTOR_H

#include <type_traits>
#include <cstddef>
#include <initializer_list>
#include "../../memory/jr_allocator.h"
#include "../../iterator/jr_iterator.h"

namespace jrSTL {
    template< class T, class Allocator = jrSTL::allocator<T> >
    class vector {
    public:
        typedef T* iterator;
        typedef const T* const_iterator;
        typedef jrSTL::reverse_iterator<const_iterator> const_reverse_iterator;
        typedef jrSTL::reverse_iterator<iterator> reverse_iterator;
        typedef T value_type;
        typedef Allocator allocator_type;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef T& reference;
        typedef const T& const_reference;
        typedef T* pointer;
        typedef const T* const_pointer;

    private:
        Allocator _alloc;
        size_type _size, _cap;
        iterator _head, _tail, _end_of_storage;

        void _move_elements(iterator destination) {
            iterator tmp = destination, old_head = _head;
            while(_head != _tail) {
                _alloc.construct(tmp, static_cast<T&&>(*_head));
                ++_head;
                ++tmp;
            }
            _head = destination;
            _tail = tmp;
            if(old_head) {
                _alloc.destroy(old_head);
                _alloc.deallocate(old_head, 1);
                old_head = nullptr;
            }
        }

        void _ctor(size_type count, const_reference value, std::true_type) {
            _size = count;
            _cap = 2 * _size;
            _head = _alloc.allocate(_cap);
            _tail = _head + _size;
            _end_of_storage = _head + _cap;
            for(iterator tmp = _head; tmp != _tail; ++tmp) {
                _alloc.construct(tmp, value);
            }
        }

        template<class InputIt>
        void _ctor(InputIt first, InputIt last, std::false_type) {
            _size = static_cast<size_type>(last - first);
            _cap = 2 * _size;
            _head = _alloc.allocate(_cap);
            _tail = _head + _size;
            _end_of_storage = _head + _cap;
            for(iterator tmp = _head; tmp != _tail; ++tmp) {
                _alloc.construct(tmp, *(jrSTL::next(first, tmp - _head)));
            }
        }

        void _assign(size_type count, const_reference value, std::true_type) {
            _size = count;
            if(count > _cap) {
                _cap = 2 * _size;
                _head = _alloc.allocate(_cap);
                _end_of_storage = _head + _cap;
            }
            _tail = _head;
            for(size_type i = 0; i < count; ++i) {
                _alloc.destroy(_tail);
                _alloc.construct(_tail, value);
                ++_tail;
            }
        }

        template<class InputIt>
        void _assign( InputIt first, InputIt last, std::false_type) {
            difference_type i, dis = last - first;
            _size = dis;
            if(dis > _cap) {
                _cap = 2 * _size;
                _head = _alloc.allocate(_cap);
                _end_of_storage = _head + _cap;
            }
            _tail = _head;
            for(i = 0; i < dis; ++i) {
                _alloc.construct(_tail, *(first + i));
                ++_tail;
            }
        }

        iterator _insert(const_iterator pos, size_type count,
                         const_reference value, std::true_type) {
            difference_type dis = pos - cbegin();
            if(_size + count > _cap) {
                _cap = 2 * (_size + count);
                _move_elements(_alloc.allocate(_cap));
                _end_of_storage = _head + _cap;
            }
            iterator pos_mutable = begin() + dis;
            for(iterator tmp = _tail + count - 1; tmp != pos_mutable; --tmp) {
                _alloc.construct(&(*tmp), *(tmp - count));
            }
            for(size_type i = 0; i < count; ++i) {
                _alloc.construct(pos_mutable + i, value);
            }
            _tail += count;
            _size += count;
            return begin() + dis;
        }

        template<class InputIt>
        iterator _insert(const_iterator pos,
                         InputIt first, InputIt last,
                         std::false_type) {
            difference_type dis = pos - cbegin();
            while(first != last) {
                pos = insert(pos, *first++);
                pos++;
            }
            return begin() + dis;
        }

    public:
        vector() : _size(0), _cap(4) {
            _head = _alloc.allocate(_cap);
            _tail = _head;
            _end_of_storage = _head + _cap;
        }

        vector( const vector& other )
            :_size(other._size), _cap(other._cap) {
            if(this == &other)
                return;
            _head = _alloc.allocate(_cap);
            _tail = _head + _size;
            _end_of_storage = _head + _cap;
            for(iterator tmp = _head; tmp != _tail; ++tmp) {
                _alloc.construct(tmp, other._head[tmp - _head]);
            }
        }

        vector( vector&& other )
             : _size(other._size), _cap(other._cap),
               _head(other._head), _tail(other._tail),
               _end_of_storage(other._end_of_storage) {
            if(this == &other)
                return;
            other._cap = 0;
            other._size = 0;
            other._head = nullptr;
            other._tail = nullptr;
            other._end_of_storage = nullptr;
        }

        vector( const vector& other, const Allocator& a )
            : _alloc(a), _size(other._size), _cap(other._cap) {
            if(this == &other)
                return;
            _head = _alloc.allocate(_cap);
            _tail = _head + _size;
            _end_of_storage = other._end_of_storage;
            for(iterator tmp = _head; tmp != _tail; ++tmp) {
                _alloc.construct(tmp, other._head[tmp - _head]);
            }
        }

        vector( vector&& other, const Allocator& a )
            : _alloc(a), _size(other._size), _cap(other._cap),
              _head(other._head), _tail(other._tail),
              _end_of_storage(other._end_of_storage) {
            if(this == &other)
                return;
            other._cap = 0;
            other._size = 0;
            other._head = nullptr;
            other._tail = nullptr;
            other._end_of_storage = nullptr;
        }

        vector( size_type count, const_reference value,
                const Allocator& a = Allocator() )
            : _alloc(a) {
            _ctor(count, value, std::true_type());
        }

        explicit vector( size_type count )
            : _size(count), _cap(2*count) {
            _head = _alloc.allocate(_cap);
            _tail = _head + _size;
            _end_of_storage = _head + _cap;
        }

        explicit vector( const Allocator& a)
            : _size(0), _cap(4), _alloc(a) {
            _head = _alloc.allocate(_cap);
            _tail = _head;
            _end_of_storage = _head + _cap;
        }

        template< class InputIt >
        vector( InputIt first, InputIt last,
                const Allocator& a = Allocator() )
            : _alloc(a) {
            typedef std::integral_constant<bool, std::is_integral<InputIt>::value> type;
            _ctor(first, last, type());
        }

        vector( std::initializer_list<T> init,
                const Allocator& a = Allocator() )
            : _alloc(a), _size(0), _cap(2*init.size())  {
            _head = _alloc.allocate(_cap);
            _tail = _head;
            _end_of_storage = _head + _cap;
            for(auto it = init.begin(); it != init.end(); ++it)
                push_back(*it);
        }

        ~vector() {
            for(iterator tmp = _head; tmp != _tail; ++tmp) {
                _alloc.destroy(&(*tmp));
            }
            _alloc.deallocate(_head, _cap);
        }

        vector& operator=( const vector& other ) {
            if(this == &other)
                return *this;
            if(capacity() > 0) {
                for(iterator tmp = _head; tmp != _tail; ++tmp) {
                    _alloc.destroy(&(*tmp));
                }
                _alloc.deallocate(_head, _cap);
            }
            _cap = other._cap;
            _size = other._size;
            _head = _alloc.allocate(_cap);
            _tail = _head + _size;
            _end_of_storage = _head + _cap;
            for(iterator tmp = _head; tmp != _tail; ++tmp) {
                _alloc.construct(tmp, other._head[tmp - _head]);
            }
            return *this;
        }

        vector& operator=( vector&& other ) {
            if(this == &other)
                return *this;
            if(capacity() > 0) {
                for(iterator tmp = _head; tmp != _tail; ++tmp) {
                    _alloc.destroy(&(*tmp));
                }
                _alloc.deallocate(_head, _cap);
            }
            _cap = other._cap;
            _size = other._size;
            _head = other._head;
            _tail = other._tail;
            _end_of_storage = other._end_of_storage;
            other._cap = 0;
            other._size = 0;
            other._head = nullptr;
            other._tail = nullptr;
            other._end_of_storage = nullptr;
            return *this;
        }

        void assign( size_type count, const_reference value ) {
            clear();
            _assign(count, value, std::true_type());
        }

        void assign( std::initializer_list<T> ilist ) {
            clear();
            for(auto it = ilist.begin(); it != ilist.end(); ++it)
                push_back(*it);
        }

        template<class InputIt>
        void assign( InputIt first, InputIt last) {
            clear();
            typedef std::integral_constant<bool, std::is_integral<InputIt>::value> type;
            _assign(first, last, type());
        }

        allocator_type get_allocator() const noexcept {
            return _alloc;
        }

        reference operator[]( size_type pos ) {
            return *(_head + pos);
        }

        const_reference operator[]( size_type pos ) const {
            return *(_head + pos);
        }

        reference at( size_type pos ) {
            return *(_head + pos);
        }

        const_reference at( size_type pos ) const {
            return *(_head + pos);
        }

        pointer data() noexcept {
            return _head;
        }

        const_pointer data() const noexcept {
            return _head;
        }

        reference front() {
            return *_head;
        }

        const_reference front() const {
            return *_head;
        }

        reference back() {
            return *(_tail - 1);
        }

        const_reference back() const {
            return *(_tail - 1);
        }

        iterator begin() noexcept {
            return _head;
        }

        const_iterator begin() const noexcept {
            return _head;
        }

        const_iterator cbegin() const noexcept {
            return _head;
        }

        iterator end() noexcept {
            return _tail;
        }

        const_iterator end() const noexcept {
            return _tail;
        }

        const_iterator cend() const noexcept {
            return _tail;
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

        bool empty() const noexcept {
            return _size == 0;
        }

        size_type size() const noexcept {
            return _size;
        }

        size_type max_size() const noexcept {
            return _cap;
        }

        size_type capacity() const noexcept {
            return _cap;
        }

        void reserve( size_type new_cap ) {
            if (new_cap > _cap) {
                _cap = new_cap;
                iterator tmp, new_head = _alloc.allocate(_cap);
                for(tmp = new_head; _head != _tail; ++_head) {
                    *tmp = *_head;
                    ++tmp;
                }
                _head = new_head;
                _tail = tmp;
                _end_of_storage = _head + _size;
            }
        }

        void shrink_to_fit() {
            if(_size == _cap)
                return;
            _cap = _size;
            _move_elements(_alloc.allocate(_cap));
            _end_of_storage = _tail;
        }

        void swap( vector& other ) {
            if(this == &other)
                return;
            {
                size_type s = _size;
                _size = other._size;
                other._size = s;
            }
            {
                size_type c = _cap;
                _cap = other._cap;
                other._cap = c;
            }
            {
                iterator h = _head;
                _head = other._head;
                other._head = h;
            }
            {
                iterator t = _tail;
                _tail = other._tail;
                other._tail = t;
            }
            {
                iterator e = _end_of_storage;
                _end_of_storage = other._end_of_storage;
                other._end_of_storage = e;
            }
        }

        void resize( size_type count, const_reference value ) {
            if(count == size())
                return;
            _size = count;
            if(_cap < count) {
                _cap = 2 * _size;
                size_type m = _tail - _head;
                _move_elements(_alloc.allocate(_cap));
                _end_of_storage = _head + _cap;
                for(size_type i = 0; i < count - m; i++) {
                    _alloc.construct(_tail, value);
                    ++_tail;
                }
            } else if(_cap > count) {
                _tail = _head + count;
            }
        }

        void resize( size_type count ) {
            resize(count, T());
        }

        iterator erase( iterator pos ) {
            if(pos == end())
                return end();
            iterator ret = pos;
            while(pos != end()) {
                _alloc.destroy(&(*pos));
                if((pos + 1) == end())
                    break;
                _alloc.construct(&(*pos), *(pos + 1));
                ++pos;
            }
            --_size;
            --_tail;
            return ret;
        }

        iterator erase( iterator first, iterator last ) {
            if(first == last)
                return last;
            while(first != last) {
                first = erase(first);
                --last;
            }
            return first;
        }

        iterator insert( const_iterator pos, size_type count, const_reference value) {
            return _insert(pos, count, value, std::true_type());
        }

        iterator insert( const_iterator pos, const_reference value ) {
            return insert(pos, 1, value);
        }

        iterator insert( const_iterator pos, T&& value ) {
            difference_type dis = pos - _head;
            if(_size == _cap) {
                _cap = 2 * (_size + 1);
                _move_elements(_alloc.allocate(_cap));
                _end_of_storage = _head + _cap;
            }
            iterator pos_mutable = begin();
            jrSTL::advance(pos_mutable, dis);
            for(iterator tmp = _tail; tmp != pos_mutable; --tmp) {
                _alloc.construct(&(*tmp), std::move(*(tmp - 1)));
            }
            _alloc.construct(pos_mutable,
                             std::move(value));
            ++_tail;
            ++_size;
            return pos_mutable;
        }

        iterator insert( const_iterator pos, std::initializer_list<T> ilist ) {
            difference_type len = jrSTL::distance(cbegin(), pos);
            for(auto it = ilist.begin(); it != ilist.end(); ++it) {
                pos = insert(pos, *it);
                ++pos;
            }
            return begin() + len;
        }

        template< class InputIt >
        iterator insert( const_iterator pos, InputIt first, InputIt last) {
            typedef std::integral_constant<bool, std::is_integral<InputIt>::value> type;
            return _insert(pos, first, last, type());
        }

        void push_back( const_reference value ) {
            insert(end(), value);
        }

        void push_back( T&& value ) {
            insert(end(), std::forward<T&&>(value));
        }

        void pop_back() {
            erase(end() - 1);
        }

        void clear() noexcept {
            erase(begin(), end());
        }

        template< class... Args >
        iterator emplace( const_iterator pos, Args&&... args ) {
            difference_type dis = pos - _head;
            if(_size == _cap) {
                _cap = 2 * (_size + 1);
                _move_elements(_alloc.allocate(_cap));
                _end_of_storage = _head + _cap;
            }
            iterator pos_mutable = begin();
            jrSTL::advance(pos_mutable, dis);
            for(iterator tmp = _tail; tmp != pos_mutable; --tmp) {
                *tmp = *(tmp - 1);
            }
            _alloc.construct(pos_mutable,
                             static_cast<Args&&>(args)...);
            ++_tail;
            ++_size;
            return pos_mutable;
        }

        template< class... Args >
        void emplace_back( Args&&... args ) {
            if(_size == _cap) {
                _cap = 2 * (_size + 1);
                _move_elements(_alloc.allocate(_cap));
                _end_of_storage = _head + _cap;
            }
            _alloc.construct(_tail,
                             static_cast<Args&&>(args)...);
            ++_tail;
            ++_size;
        }
    };

    template< class T, class Alloc >
    void swap( jrSTL::vector<T,Alloc>& lhs,
               jrSTL::vector<T,Alloc>& rhs ) {
        lhs.swap(rhs);
    }

    template< class T, class Alloc >
    bool operator==( const jrSTL::vector<T, Alloc>& lhs,
                     const jrSTL::vector<T, Alloc>& rhs ) {
        if (lhs.size() != rhs.size())
            return false;
        for(size_t i = 0; i < lhs.size(); i++) {
            if(lhs.at(i) != rhs.at(i))
                return false;
        }
        return true;
    }

    template< class T, class Alloc >
    bool operator!=( const jrSTL::vector<T, Alloc>& lhs,
                     const jrSTL::vector<T, Alloc>& rhs ) {
        return !(lhs == rhs);
    }

    template< class T, class Alloc >
    bool operator<( const jrSTL::vector<T, Alloc>& lhs,
                    const jrSTL::vector<T, Alloc>& rhs ) {
        if (lhs.size() < rhs.size())
            return true;
        if (lhs.size() > rhs.size() || operator==(lhs, rhs))
            return false;
        for(size_t i = 0; i < lhs.size(); i++) {
            if(lhs.at(i) > rhs.at(i))
                return false;
        }
        return true;
    }

    template< class T, class Alloc >
    bool operator>( const jrSTL::vector<T, Alloc>& lhs,
                    const jrSTL::vector<T, Alloc>& rhs ) {
        if (lhs.size() > rhs.size())
            return true;
        if (lhs.size() < rhs.size() || operator==(lhs, rhs))
            return false;
        for(size_t i = 0; i < lhs.size(); i++) {
            if(lhs.at(i) < rhs.at(i))
                return false;
        }
        return true;
    }

    template< class T, class Alloc >
    bool operator>=( const jrSTL::vector<T, Alloc>& lhs,
                     const jrSTL::vector<T, Alloc>& rhs ) {
        return !(lhs < rhs);
    }

    template< class T, class Alloc >
    bool operator<=( const jrSTL::vector<T, Alloc>& lhs,
                     const jrSTL::vector<T, Alloc>& rhs ) {
        return !(lhs > rhs);
    }

}

#endif // JR_VECTOR_H
