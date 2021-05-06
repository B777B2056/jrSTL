#ifndef QUEUE_H
#define QUEUE_H

#include "../sequence/jr_deque.h"

namespace jr_std {
    template< class T, class Container = jr_std::deque<T> >
    class queue {
    protected:
        Container c;
        typedef Container container_type;
        typedef typename Container::value_type value_type;
        typedef typename Container::size_type size_type;
        typedef typename Container::reference reference;
        typedef typename Container::const_reference const_reference;
    public:
        queue() {}

        explicit queue( const Container& cont ) {
            c = cont;
        }

        explicit queue( Container&& cont ) {
            c = static_cast<Container&&>(cont);
        }

        queue( const queue& other ) {
            c = other.cont;
        }

        queue( queue&& other ) {
            c = static_cast<Container&&>(other.cont);
        }

        ~queue() {}

        queue& operator=( const queue& other ) { c = other.c; }

        queue& operator=( queue&& other ) { c = static_cast<Container&&>(other.c); }

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
            jr_std::swap(c, other.c);
        }
    };

    template< class T, class Container >
    void swap( jr_std::queue<T,Container>& lhs,
               jr_std::queue<T,Container>& rhs ) {
        lhs.swap(rhs);
    }

    template< class T, class Container >
    bool operator==( const jr_std::queue<T,Container>& lhs,
                     const jr_std::queue<T,Container>& rhs ) {
        return lhs == rhs;
    }

    template< class T, class Container >
    bool operator!=( const jr_std::queue<T,Container>& lhs,
                     const jr_std::queue<T,Container>& rhs ) {
        return lhs != rhs;
    }

}

#endif // QUEUE_H
