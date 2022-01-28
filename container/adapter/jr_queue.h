#ifndef QUEUE_H
#define QUEUE_H

#include "../sequence/jr_deque.h"

namespace jrSTL {
    template< class T, class Container = jrSTL::deque<T> >
    class queue {
    protected:
        Container c;
        typedef Container container_type;
        typedef typename Container::value_type value_type;
        typedef typename Container::size_type size_type;
        typedef typename Container::reference reference;
        typedef typename Container::const_reference const_reference;
    public:
        queue() : c(Container()) {}

        explicit queue( const Container& cont )
            : c(cont)
        {}

        explicit queue( Container&& cont )
            : c(static_cast<Container&&>(cont))
        {}

        queue( const queue& other )
            : c(other.c)
        {}

        queue( queue&& other )
            : c(static_cast<Container&&>(other.c))
        {}

        template< class Alloc >
        explicit queue( const Alloc& alloc )
            : c(alloc)
        {}

        template< class Alloc >
        queue( const Container& cont, const Alloc& alloc )
            : c(cont, alloc)
        {}

        template< class Alloc >
        queue( Container&& cont, const Alloc& alloc )
            : c(static_cast<Container&&>(cont), alloc)
        {}

        template< class Alloc >
        queue( const queue& other, const Alloc& alloc )
            : c(other.c, alloc)
        {}

        template< class Alloc >
        queue( queue&& other, const Alloc& alloc )
            : c(static_cast<Container&&>(other.c), alloc)
        {}

        ~queue() {}

        queue& operator=( const queue& other ) {
            c = other.c;
            return *this;
        }

        queue& operator=( queue&& other ) {
            c = static_cast<Container&&>(other.c);
            return *this;
        }

        reference front() { return c.front(); }

        const_reference front() const { return c.front(); }

        reference back() { return c.back(); }

        const_reference back() const { return c.back(); }

        bool empty() const { return c.empty(); }

        size_type size() const { return c.size(); }

        void push( const value_type& value ) { c.push_back(value); }

        void push( value_type&& value ) { c.push_back(static_cast<value_type&&>(value)); }

        template< class... Args >
        void emplace( Args&&... args ) {
            c.emplace_back(static_cast<Args&&>(args)...);
        }

        void pop() { c.pop_front(); }

        void swap( queue& other ) noexcept {
            jrSTL::swap(c, other.c);
        }
        // 友元声明
        template< class U, class Cont >
        friend bool operator==( const queue<U, Cont>& lhs, const queue<U, Cont>& rhs );

        template< class U, class Cont >
        friend bool operator!=( const queue<U, Cont>& lhs, const queue<U, Cont>& rhs );

        template< class U, class Cont >
        friend bool operator<=( const queue<U, Cont>& lhs, const queue<U, Cont>& rhs );

        template< class U, class Cont >
        friend bool operator>=( const queue<U, Cont>& lhs, const queue<U, Cont>& rhs );

        template< class U, class Cont >
        friend bool operator<( const queue<U, Cont>& lhs, const queue<U, Cont>& rhs );

        template< class U, class Cont >
        friend bool operator>( const queue<U, Cont>& lhs, const queue<U, Cont>& rhs );

    };

    template< class T, class Container >
    void swap( jrSTL::queue<T,Container>& lhs,
               jrSTL::queue<T,Container>& rhs )
    { lhs.swap(rhs); }

    template< class T, class Container >
    bool operator==( const jrSTL::queue<T,Container>& lhs,
                     const jrSTL::queue<T,Container>& rhs )
    { return lhs.c == rhs.c; }

    template< class T, class Container >
    bool operator!=( const jrSTL::queue<T,Container>& lhs,
                     const jrSTL::queue<T,Container>& rhs )
    { return lhs.c != rhs.c; }

    template< class T, class Container >
    bool operator<( const jrSTL::queue<T,Container>& lhs,
                    const jrSTL::queue<T,Container>& rhs )
    { return lhs.c < rhs.c; }

    template< class T, class Container >
    bool operator<=( const jrSTL::queue<T,Container>& lhs,
                     const jrSTL::queue<T,Container>& rhs )
    { return lhs.c <= rhs.c; }

    template< class T, class Container >
    bool operator>( const jrSTL::queue<T,Container>& lhs,
                    const jrSTL::queue<T,Container>& rhs )
    { return lhs.c > rhs.c; }

    template< class T, class Container >
    bool operator>=( const jrSTL::queue<T,Container>& lhs,
                     const jrSTL::queue<T,Container>& rhs )
    { return lhs.c >= rhs.c; }
}

#endif // QUEUE_H
