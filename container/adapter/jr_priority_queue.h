#ifndef JR_PRIORITY_QUEUE_H
#define JR_PRIORITY_QUEUE_H

#include <cstddef>
#include "../sequence/jr_vector.h"
#include "../../algorithm/jr_algorithm.h"
#include "../../functional/jr_functional.h"

namespace jr_std {
    template<class T, class Container = jr_std::vector<T>,
         class Compare = jr_std::less<typename Container::value_type> >
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
            priority_queue() : c(Container()), comp(Compare()) {}

            explicit priority_queue(const Compare& x)
                : c(Container()), comp(x) {}

            priority_queue(const Compare& x, const Container& y)
                : c(y), comp(x) {}

            priority_queue(const Compare& x, Container&& y)
                : c(static_cast<Container&&>(y)), comp(x) {}

            priority_queue( const priority_queue& other ) {
                c = other.c;
                comp = other.comp;
            }

            priority_queue( priority_queue&& other ) {
                c = static_cast<Container&&>(other.c);
                comp = static_cast<Compare&&>(other.comp);
            }

            template<class InputIt>
            priority_queue(InputIt first, InputIt last,
                           const Compare& x,
                           const Container& y)
                : c(y), comp(x) {
                c.insert(c.end(), first, last);
                jr_std::make_heap(c.begin(), c.end(), comp);
            }

            template<class InputIt>
            priority_queue(InputIt first, InputIt last,
                           const Compare& x = Compare(),
                           Container&& y = Container())
                : c(static_cast<Container&&>(y)), comp(x) {
                c.insert(c.end(), first, last);
                jr_std::make_heap(c.begin(), c.end(), comp);
            }

            ~priority_queue() = default;

            priority_queue& operator=( const priority_queue& other ) {
                c = other.c;
                comp = other.comp;
            }

            priority_queue& operator=( priority_queue&& other ) {
                c = static_cast<Container&&>(other.c);
                comp = static_cast<Compare&&>(other.comp);
            }

            bool empty() const { return c.empty(); }

            size_type size() const { return c.size(); }

            const_reference top() const { return c.front(); }

            void push(const value_type& x) {
                c.push_back(x);
                jr_std::push_heap(c.begin(), c.end(), comp);
            }

            void push(value_type&& x) {
                c.push_back(static_cast<value_type&&>(x));
                jr_std::push_heap(c.begin(), c.end(), comp);
            }

            template<class... Args>
            void emplace(Args&&... args) {
                c.push_back(static_cast<Args&&>(args)...);
                jr_std::push_heap(c.begin(), c.end(), comp);
            }

            void pop() {
                jr_std::pop_heap(c.begin(), c.end(), comp);
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
              noexcept(noexcept(x.swap(y)))
    { x.swap(y); }
}

#endif // JR_PRIORITY_QUEUE_H
