#ifndef STACK_H
#define STACK_H

#include "../sequence/jr_deque.h"

namespace panzer {
    template< class T, class Container = panzer::deque<T> >
    class stack {
        public:
            typedef Container container_type;
            typedef typename Container::value_type value_type;
            typedef typename Container::size_type size_type;
            typedef typename Container::reference reference;
            typedef typename Container::const_reference const_reference;

        private:
            Container c;

        public:
            stack()
                : c()
            {}

            explicit stack( const Container& cont )
                : c(cont)
            {}

            explicit stack( Container&& cont )
                : c(static_cast<Container&&>(cont))
            {}

            stack( const stack& other )
                : c(other.c)
            {}

            stack( stack&& other )
                : c(static_cast<Container&&>(other.c))
            {}

            template< class Alloc >
            explicit stack( const Alloc& alloc )
                : c(alloc)
            {}

            template< class Alloc >
            stack( const Container& cont, const Alloc& alloc )
                : c(cont, alloc)
            {}

            template< class Alloc >
            stack( Container&& cont, const Alloc& alloc )
                : c(static_cast<Container&&>(cont), alloc)
            {}

            template< class Alloc >
            stack( const stack& other, const Alloc& alloc )
                : c(other.c, alloc)
            {}

            template< class Alloc >
            stack( stack&& other, const Alloc& alloc )
                : c(static_cast<Container&&>(other.c), alloc)
            {}

            ~stack() = default;

            stack& operator=( const stack& other ) {
                c = other.c;
                return *this;
            }

            stack& operator=( stack&& other ) {
                c = static_cast<Container&&>(other.c);
                return *this;
            }

            reference top() {
                return c.back();
            }

            const_reference top() const {
                return c.back();
            }

            bool empty() const {
                return c.empty();
            }

            size_type size() const {
                return c.size();
            }

            void push( const value_type& value ) {
                c.push_back(value);
            }

            void push( value_type&& value ) {
                c.push_back(static_cast<value_type&&>(value));
            }

            template< class... Args >
            void emplace( Args&&... args ) {
                c.emplace_back(static_cast<Args&&>(args)...);
            }

            void pop() {
                c.pop_back();
            }

            void swap( stack& other ) noexcept {
                panzer::swap(c, other.c);
            }

            // 友元声明
            template< class U, class Cont >
            friend bool operator==( const stack<U, Cont>& lhs,
                                    const stack<U, Cont>& rhs );

            template< class U, class Cont >
            friend bool operator!=( const stack<U, Cont>& lhs,
                                    const stack<U, Cont>& rhs );

            template< class U, class Cont >
            friend bool operator<=( const stack<U, Cont>& lhs,
                                    const stack<U, Cont>& rhs );

            template< class U, class Cont >
            friend bool operator>=( const stack<U, Cont>& lhs,
                                    const stack<U, Cont>& rhs );

            template< class U, class Cont >
            friend bool operator<( const stack<U, Cont>& lhs,
                                   const stack<U, Cont>& rhs );

            template< class U, class Cont >
            friend bool operator>( const stack<U, Cont>& lhs,
                                   const stack<U, Cont>& rhs );
    };

    template< class T, class Container >
    void swap( panzer::stack<T,Container>& lhs,
               panzer::stack<T,Container>& rhs ) {
        lhs.swap(rhs);
    }

    template< class T, class Container >
    bool operator==( const panzer::stack<T,Container>& lhs,
                     const panzer::stack<T,Container>& rhs ) {
        return lhs.c == rhs.c;
    }

    template< class T, class Container >
    bool operator!=( const panzer::stack<T,Container>& lhs,
                     const panzer::stack<T,Container>& rhs ) {
        return lhs.c != rhs.c;
    }

    template< class T, class Container >
    bool operator<( const panzer::stack<T,Container>& lhs,
                     const panzer::stack<T,Container>& rhs ) {
        return lhs.c < rhs.c;
    }

    template< class T, class Container >
    bool operator<=( const panzer::stack<T,Container>& lhs,
                     const panzer::stack<T,Container>& rhs ) {
        return lhs.c <= rhs.c;
    }

    template< class T, class Container >
    bool operator>( const panzer::stack<T,Container>& lhs,
                    const panzer::stack<T,Container>& rhs ) {
        return lhs.c > rhs.c;
    }

    template< class T, class Container >
    bool operator>=( const panzer::stack<T,Container>& lhs,
                     const panzer::stack<T,Container>& rhs ) {
        return lhs.c >= rhs.c;
    }
}

#endif // STACK_H
