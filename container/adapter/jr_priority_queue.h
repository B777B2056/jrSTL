#ifndef JR_PRIORITY_QUEUE_H
#define JR_PRIORITY_QUEUE_H

#include <cstddef>
#include "../sequence/jr_vector.h"
#include "../utils/jr_heap.h"
#include "../../functional/jr_functional.h"

namespace jrSTL {
    template<class T,
             class Container = jrSTL::vector<T>,
             class Compare = jrSTL::less<typename Container::value_type> >
    class priority_queue {
        public:
            typedef typename Container::value_type value_type;
            typedef typename Container::reference reference;
            typedef typename Container::const_reference const_reference;
            typedef typename Container::size_type size_type;
            typedef Container container_type;
            typedef Compare value_compare;

        protected:
            Container c;
            Compare comp;

        public:
            priority_queue()
                : c(Container()), comp(Compare())
            {}

            explicit priority_queue(const Compare& x)
                : c(Container()), comp(x)
            {}

            priority_queue(const Compare& x, const Container& y)
                : c(y), comp(x)
            {}

            priority_queue(const Compare& x, Container&& y)
                : c(static_cast<Container&&>(y)), comp(x)
            {}

            priority_queue( const priority_queue& other )
                : c(other.c), comp(other.comp)
            {}

            priority_queue( priority_queue&& other )
                : c(static_cast<Container&&>(other.c)),
                  comp(static_cast<Compare&&>(other.comp))
            {}

            template< class Alloc >
            explicit priority_queue( const Alloc& alloc )
                : c(alloc)
            {}

            template< class Alloc >
            priority_queue( const Compare& compare, const Alloc& alloc )
                : c(alloc), comp(compare)
            {}

            template< class Alloc >
            priority_queue( const Compare& compare, const Container& cont,
                            const Alloc& alloc )
                : c(cont, alloc), comp(compare)
            {}

            template< class Alloc >
            priority_queue( const Compare& compare, Container&& cont,
                            const Alloc& alloc )
                : c(static_cast<Container&&>(cont), alloc), comp(compare)
            {}

            template< class Alloc >
            priority_queue( const priority_queue& other,
                            const Alloc& alloc )
                : c(other.c, alloc), comp(other.compare)
            {}

            template< class Alloc >
            priority_queue( priority_queue&& other, const Alloc& alloc )
                : c(static_cast<Container&&>(other.c), alloc),
                  comp(static_cast<Compare&&>(other.compare))
            {}

            template<class InputIt>
            priority_queue(InputIt first, InputIt last,
                           const Compare& x,
                           const Container& y)
                : c(y), comp(x) {
                c.insert(c.end(), first, last);
                jrSTL::make_heap(c.begin(), c.end(), comp);
            }

            template<class InputIt>
            priority_queue(InputIt first, InputIt last,
                           const Compare& x = Compare(),
                           Container&& y = Container())
                : c(static_cast<Container&&>(y)), comp(x) {
                c.insert(c.end(), first, last);
                jrSTL::make_heap(c.begin(), c.end(), comp);
            }

            ~priority_queue() = default;

            priority_queue& operator=( const priority_queue& other ) {
                c = other.c;
                comp = other.comp;
                return *this;
            }

            priority_queue& operator=( priority_queue&& other ) {
                c = static_cast<Container&&>(other.c);
                comp = static_cast<Compare&&>(other.comp);
                return *this;
            }

            bool empty() const { return c.empty(); }

            size_type size() const { return c.size(); }

            const_reference top() const { return c.front(); }

            void push(const value_type& x) {
                c.push_back(x);
                jrSTL::push_heap(c.begin(), c.end(), comp);
            }

            void push(value_type&& x) {
                c.push_back(static_cast<value_type&&>(x));
                jrSTL::push_heap(c.begin(), c.end(), comp);
            }

            template<class... Args>
            void emplace(Args&&... args) {
                c.push_back(static_cast<Args&&>(args)...);
                jrSTL::push_heap(c.begin(), c.end(), comp);
            }

            void pop() {
                jrSTL::pop_heap(c.begin(), c.end(), comp);
                c.pop_back();
            }

            void swap(priority_queue& q) noexcept {
                c.swap(q.c);
                Compare tc = comp;
                comp = q.comp;
                q.comp = tc;
            }
    };

    template<class T, class Container, class Compare>
    void swap(priority_queue<T, Container, Compare>& x,
              priority_queue<T, Container, Compare>& y)
              noexcept(noexcept(x.swap(y))) {
        x.swap(y);
    }
}

#endif // JR_PRIORITY_QUEUE_H
