#ifndef JR_NUMERIC_H
#define JR_NUMERIC_H

#include "../iterator/jr_iterator.h"

namespace panzer {
    template< class InputIt, class T, class BinaryOperation >
    T accumulate( InputIt first, InputIt last, T init,
                  BinaryOperation op ) {
        while(first != last) {
            init = op(init, *first);
            ++first;
        }
        return init;
    }

    template< class InputIt, class T >
    T accumulate( InputIt first, InputIt last, T init ) {
        return accumulate(first, last, init,
                          [](const T& a, const T& b)->T { return a + b; });
    }

    template< class InputIt, class OutputIt, class BinaryOperation >
    OutputIt partial_sum( InputIt first, InputIt last, OutputIt d_first,
                          BinaryOperation op ) {
        if(first == last)
            return d_first;
        *d_first = *first;
        ++first;
        ++d_first;
        while(first != last) {
            *d_first = op(*d_first, *first);
            ++first;
            ++d_first;
        }
        return d_first;
    }

    template< class InputIt, class OutputIt >
    OutputIt partial_sum( InputIt first, InputIt last, OutputIt d_first ) {
        typedef typename iterator_traits<OutputIt>::value_type T;
        return partial_sum(first, last, d_first,
                          [](const T& a, const T& b)->T { return a + b; });
    }

    template<class InputIt1, class InputIt2, class T,
             class BinaryOperation1, class BinaryOperation2>
    T inner_product( InputIt1 first1, InputIt1 last1,
                     InputIt2 first2, T init,
                     BinaryOperation1 op1,
                     BinaryOperation2 op2 ) {
        while(first1 != last1) {
            init = op1(init, op2(*first1, *first2));
            ++first1;
            ++first2;
        }
        return init;
    }

    template< class InputIt1, class InputIt2, class T >
    T inner_product( InputIt1 first1, InputIt1 last1,
                     InputIt2 first2, T init ) {
        return inner_product(first1, first2, last1, init,
                             [](const T& a, const T& b)->T { return a + b; },
                             [](const T& a, const T& b)->T { return a * b; });
    }

    template< class InputIt, class OutputIt >
    OutputIt adjacent_difference( InputIt first, InputIt last,
                                  OutputIt d_first ) {
        typedef typename iterator_traits<OutputIt>::value_type type;
        while(first != last) {
            type acc = *first;
            ++first;
            *d_first = *first - acc;
            ++d_first;
        }
        return d_first;
    }

    template< class ForwardIt, class T >
    void iota( ForwardIt first, ForwardIt last, T value ) {
        while(first != last) {
            *first = (value++);
            ++first;
        }
    }
}

#endif // JR_NUMERIC_H
