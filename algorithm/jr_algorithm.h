#ifndef JR_ALGORITHM_H
#define JR_ALGORITHM_H

#include <iostream>
#include <cstring>
#include <cstddef>
#include <type_traits>
#include "../iterator/jr_iterator.h"
#include "../container/utils/jr_utility.h"

namespace jr_std {
    /*不修改序列的操作*/
    template< class InputIt, class UnaryPredicate >
    bool all_of( InputIt first, InputIt last, UnaryPredicate p ) {
        while(first != last) {
            if(!p(*first))
                return false;
            ++first;
        }
        return true;
    }

    template< class InputIt, class UnaryPredicate >
    bool any_of( InputIt first, InputIt last, UnaryPredicate p ) {
        while(first != last) {
            if(p(*first))
                return true;
            ++first;
        }
        return false;
    }

    template< class InputIt, class UnaryPredicate >
    bool none_of( InputIt first, InputIt last, UnaryPredicate p ) {
        while(first != last) {
            if(p(*first))
                return false;
            ++first;
        }
        return true;
    }

    template< class InputIt, class UnaryFunction >
    UnaryFunction for_each( InputIt first, InputIt last, UnaryFunction f ) {
        while(first != last) {
            f(*first);
            ++first;
        }
        return static_cast<UnaryFunction&&>(f);
    }

    template< class InputIt, class UnaryPredicate >
    typename jr_std::iterator_traits<InputIt>::difference_type
    count_if( InputIt first, InputIt last, UnaryPredicate p ) {
        typename jr_std::iterator_traits<InputIt>::difference_type cnt = 0;
        while(first != last) {
            if(p(*first))
                ++cnt;
            ++first;
        }
        return cnt;
    }

    template< class InputIt, class T >
    typename jr_std::iterator_traits<InputIt>::difference_type
    count( InputIt first, InputIt last, const T &value )
    { return jr_std::count_if(first, last,
                              [&value](const T& m)->bool { return value == m; }); }

    template< class InputIt1, class InputIt2, class BinaryPredicate >
    jr_std::pair<InputIt1, InputIt2>
    mismatch( InputIt1 first1, InputIt1 last1, InputIt2 first2, BinaryPredicate p ) {
        while(first1 != last1) {
            if(!p(*first1, *first2))
                break;
            ++first1;
            ++first2;
        }
        return jr_std::pair<InputIt1, InputIt2>(first1, first2);
    }

    template< class InputIt1, class InputIt2 >
    pair<InputIt1, InputIt2>
    mismatch( InputIt1 first1, InputIt1 last1, InputIt2 first2 ) {
        typedef typename jr_std::iterator_traits<InputIt1>::value_type type1;
        typedef typename jr_std::iterator_traits<InputIt2>::value_type type2;
        return jr_std::mismatch(first1, last1, first2,
                        [](const type1& a, const type2& b)
                                ->bool { return a == b; });
    }

    template< class InputIt, class UnaryPredicate >
    InputIt find_if( InputIt first, InputIt last, UnaryPredicate p ) {
        while(first != last) {
             if(p(*first))  break;
             ++first;
        }
        return first;
    }

    template< class InputIt, class UnaryPredicate >
    InputIt find_if_not( InputIt first, InputIt last, UnaryPredicate q ) {
        while(first != last) {
             if(!q(*first))  break;
             ++first;
        }
        return first;
    }

    template< class InputIt, class T >
    InputIt find( InputIt first, InputIt last, const T& value ) {
        return jr_std::find_if(first, last,
                               [&value](const T& a)
                               ->bool { return value == a; });
    }

    template< class ForwardIt1, class ForwardIt2, class BinaryPredicate >
    ForwardIt1 find_end( ForwardIt1 first, ForwardIt1 last,
                         ForwardIt2 s_first, ForwardIt2 s_last, BinaryPredicate p ) {
        if(s_first == s_last)
            return last;
        auto rfirst = jr_std::reverse_iterator<ForwardIt1>(last);
        auto rlast = jr_std::reverse_iterator<ForwardIt1>(first);
        auto rs_first = jr_std::reverse_iterator<ForwardIt2>(s_last);
        auto rs_last = jr_std::reverse_iterator<ForwardIt2>(s_first);
        while(rfirst != rlast) {
            if(p(*rfirst, *rs_first)) {
                auto tmp2 = rs_first;
                while((rfirst != rlast) && (tmp2 != rs_last) && p(*rfirst, *tmp2)) {
                    ++rfirst;
                    ++tmp2;
                }
                if(tmp2 == rs_last)
                    break;
            } else{
                ++rfirst;
            }
        }
        return rfirst == rlast ? last : rfirst.base();
    }

    template< class ForwardIt1, class ForwardIt2 >
    ForwardIt1 find_end( ForwardIt1 first, ForwardIt1 last,
                         ForwardIt2 s_first, ForwardIt2 s_last ) {
        typedef typename jr_std::iterator_traits<ForwardIt1>::value_type type1;
        typedef typename jr_std::iterator_traits<ForwardIt2>::value_type type2;
        return jr_std::find_end(first, last, s_first, s_last,
                                [](const type1& a, const type2& b)
                                ->bool { return a == b; });
    }

    template< class InputIt, class ForwardIt, class BinaryPredicate >
    InputIt find_first_of( InputIt first, InputIt last,
                           ForwardIt s_first, ForwardIt s_last, BinaryPredicate p ) {
        typedef typename jr_std::iterator_traits<InputIt>::value_type type;
        while(first != last) {
            if(s_last != jr_std::find_if(s_first, s_last,
                                         [=](type a)
                                         ->bool { return p(a, *first); })) {
                return first;
            }
            ++first;
        }
        return last;
    }

    template< class InputIt, class ForwardIt >
    InputIt find_first_of( InputIt first, InputIt last,
                           ForwardIt s_first, ForwardIt s_last ) {
        typedef typename jr_std::iterator_traits<InputIt>::value_type type1;
        typedef typename jr_std::iterator_traits<ForwardIt>::value_type type2;
        return jr_std::find_first_of(first, last, s_first, s_last,
                                     [](const type1& a, const type2& b)
                                     ->bool { return a == b; });
    }

    template< class ForwardIt1, class ForwardIt2, class BinaryPredicate >
    ForwardIt1 search( ForwardIt1 first, ForwardIt1 last,
                       ForwardIt2 s_first, ForwardIt2 s_last, BinaryPredicate p ) {
        if(s_first == s_last)
            return first;
        while(first != last) {
            if(p(*first, *s_first)) {
                ForwardIt1 tmp1 = first;
                ForwardIt2 tmp2 = s_first;
                while((tmp1 != last) && (tmp2 != s_last) && p(*tmp1, *tmp2)) {
                    ++tmp1;
                    ++tmp2;
                }
                if(tmp2 == s_last)
                    break;
            }
            ++first;
        }
        return first;
    }

    template< class ForwardIt1, class ForwardIt2 >
    ForwardIt1 search( ForwardIt1 first, ForwardIt1 last,
                       ForwardIt2 s_first, ForwardIt2 s_last ) {
        typedef typename jr_std::iterator_traits<ForwardIt1>::value_type type1;
        typedef typename jr_std::iterator_traits<ForwardIt2>::value_type type2;
        return jr_std::search(first, last, s_first, s_last,
                              [](const type1& a, const type2& b)
                              ->bool { return a == b; });
    }

    template< class ForwardIt, class Size, class T, class BinaryPredicate >
    ForwardIt search_n( ForwardIt first, ForwardIt last, Size count, const T& value,
                        BinaryPredicate p ) {
        if(!count)
            return first;
        while(first != last) {
            if(p(*first, value)) {
                Size cnt = count;
                while((first != last) && cnt && p(*first, value)) {
                    ++first;
                    --cnt;
                }
                if(!cnt)
                    break;
            } else{
                ++first;
            }
        }
        return first;
    }

    template< class ForwardIt, class Size, class T >
    ForwardIt search_n( ForwardIt first, ForwardIt last, Size count, const T& value ) {
        return jr_std::search_n(first, last, count, value,
                                [](const T& a, const T& b)
                                ->bool { return a == b; });
    }

    template< class ForwardIt, class BinaryPredicate>
    ForwardIt adjacent_find( ForwardIt first, ForwardIt last, BinaryPredicate p ) {
        while(first != last) {
            ForwardIt tmp = first;
            ++first;
            if(p(*tmp, *first))
                return tmp;
        }
        return last;
    }

    template< class ForwardIt >
    ForwardIt adjacent_find( ForwardIt first, ForwardIt last ) {
        typedef typename iterator_traits<ForwardIt>::value_type type;
        return jr_std::adjacent_find(first, last,
                                     [](const type& a, const type& b)
                                     ->bool { return a == b; });
    }

    /*修改序列的操作*/
    template< class ForwardIt1, class ForwardIt2 >
    void iter_swap( ForwardIt1 a, ForwardIt2 b ) {
        typename jr_std::iterator_traits<ForwardIt1>::value_type tmp = *a;
        *a = *b;
        *b = tmp;
    }

    //逐个拷贝元素，循环结束条件为循环次数达到last-first次
    template< class InputIt, class OutputIt >
    OutputIt _copy_d( InputIt first, InputIt last, OutputIt d_first ) {
        typename jr_std::iterator_traits<InputIt>::difference_type distance;
        for(distance = last - first; distance; --distance) {
            *d_first = *first;
            ++first;
            ++d_first;
        }
        return d_first;
    }

    // 非随机迭代器类型
    template< class InputIt, class OutputIt >
    OutputIt _copy( InputIt first, InputIt last, OutputIt d_first, input_iterator_tag ) {
        while(first != last) {
            *d_first = *first;
            ++first;
            ++d_first;
        }
        return d_first;
    }

    // 随机迭代器类型
    template< class InputIt, class OutputIt >
    OutputIt _copy( InputIt first, InputIt last, OutputIt d_first, random_access_iterator_tag ) {
        return jr_std::_copy_d(first, last, d_first);
    }

    // 分发器(包装成仿函数是因为函数不能特例化)
    template< class InputIt, class OutputIt >
    struct _copy_dispatch {
        OutputIt operator()( InputIt first, InputIt last, OutputIt d_first ) {
            return jr_std::_copy(first, last, d_first,
                                 typename jr_std::iterator_traits<InputIt>::iterator_category());
        }
    };

    // 原生指针类型（判断是否存在平凡特性）
    template< class T>
    struct _copy_dispatch<T*, T*>{
        T* operator()( T* first, T* last, T* d_first ) {
            if(std::is_trivial<T>::value) {
                std::memmove(d_first, first, static_cast<size_t>(last - first));
                return d_first + (last - first);
            }
            else
                return jr_std::_copy_d(first, last, d_first);
        }
    };

    template< class T>
    struct _copy_dispatch<const T*, T*>{
        T* operator()( const T* first, const T* last, T* d_first ) {
            if(std::is_trivial<T>::value) {
                std::memmove(d_first, first, static_cast<size_t>(last - first));
                return d_first + (last - first);
            }
            else
                return jr_std::_copy_d(first, last, d_first);
        }
    };

    // 完全泛化版本
    template< class InputIt, class OutputIt >
    OutputIt copy( InputIt first, InputIt last, OutputIt d_first ) {
        return jr_std::_copy_dispatch<InputIt, OutputIt >()(first, last, d_first);
    }

    // char/wchar_t具有平凡特性，可以直接操作内存，速度很快
    char* copy( const char* first, const char* last, char* d_first) {
        std::memmove(d_first, first, static_cast<size_t>(last - first));
        return d_first + (last - first);
    }

    wchar_t* copy( const wchar_t* first, const wchar_t* last, wchar_t* d_first) {
        std::memmove(d_first, first, static_cast<size_t>(last - first));
        return d_first + (last - first);
    }

    template< class InputIt, class OutputIt, class UnaryPredicate >
    OutputIt _copy_if( InputIt first, InputIt last, OutputIt d_first,
                       UnaryPredicate pred, input_iterator_tag ) {
        while(first != last) {
            if(pred(*first))
                *d_first++ = *first;
            ++first;
        }
        return d_first;
    }

    template< class InputIt, class OutputIt, class UnaryPredicate >
    OutputIt _copy_if( InputIt first, InputIt last, OutputIt d_first,
                       UnaryPredicate pred, random_access_iterator_tag ) {
        typename iterator_traits<InputIt>::difference_type distance;
        for(distance = last - first; distance; --distance) {
            if(pred(*first))
                *d_first++ = *first;
            ++first;
        }
        return d_first;
    }

    template< class InputIt, class OutputIt, class UnaryPredicate >
    OutputIt copy_if( InputIt first, InputIt last, OutputIt d_first,
                      UnaryPredicate pred ) {
        return jr_std::_copy_if(first, last, d_first, pred,
                                typename jr_std::iterator_traits<InputIt>::iterator_category());
    }

    template< class InputIt, class Size, class OutputIt >
    OutputIt copy_n( InputIt first, Size count, OutputIt result ) {
        while(count--) {
            *result = *first;
            ++first;
            ++result;
        }
        return result;
    }

    template< class BidirIt1, class BidirIt2 >
    BidirIt2 copy_backward( BidirIt1 first, BidirIt1 last, BidirIt2 d_last ) {
        auto res = jr_std::copy(jr_std::reverse_iterator<BidirIt1>(last),
                                jr_std::reverse_iterator<BidirIt1>(first),
                                jr_std::reverse_iterator<BidirIt2>(d_last));
        return res.base();
    }

    template< class InputIt, class OutputIt >
    OutputIt _move( InputIt first, InputIt last, OutputIt d_first,
                    input_iterator_tag ) {
        while(first != last) {
            auto m = std::move(*first);
            *d_first = m;
            ++d_first;
            ++first;
        }
        return d_first;
    }

    template< class InputIt, class OutputIt >
    OutputIt _move( InputIt first, InputIt last, OutputIt d_first,
                    random_access_iterator_tag ) {
        typename jr_std::iterator_traits<InputIt>::difference_type distance;
        for(distance = last - first; distance; --distance) {
            // 以下move与move赋值操作需分开，否则会导致memcpy错误
            auto m = std::move(*first);
            *d_first = m;
            ++first;
            ++d_first;
        }
        return d_first;
    }

    template< class InputIt, class OutputIt >
    OutputIt move( InputIt first, InputIt last, OutputIt d_first ) {
        return jr_std::_move(first, last, d_first,
                     typename jr_std::iterator_traits<InputIt>::iterator_category());
    }

    template< class BidirIt1, class BidirIt2 >
    BidirIt2 move_backward( BidirIt1 first, BidirIt1 last, BidirIt2 d_last ) {
        auto ret = jr_std::move(jr_std::reverse_iterator<BidirIt1>(last),
                                jr_std::reverse_iterator<BidirIt1>(first),
                                jr_std::reverse_iterator<BidirIt2>(d_last));
        return ret.base();
    }

    template< class ForwardIt, class T >
    void _fill( ForwardIt first, ForwardIt last, const T& value, input_iterator_tag ) {
        while(first != last) {
            *first = value;
            ++first;
        }
    }

    template< class ForwardIt, class T >
    void _fill( ForwardIt first, ForwardIt last, const T& value, random_access_iterator_tag ) {
        typename jr_std::iterator_traits<ForwardIt>::difference_type distance;
        for(distance = last - first; distance; --distance) {
            *first = value;
            ++first;
        }
    }

    template< class ForwardIt, class T >
    void fill( ForwardIt first, ForwardIt last, const T& value ) {
        jr_std::_fill(first, last, value,
                typename jr_std::iterator_traits<ForwardIt>::iterator_category());
    }

    template< class OutputIt, class Size, class T >
    OutputIt fill_n( OutputIt first, Size count, const T& value ) {
        while(count--) {
            *first = value;
            ++first;
        }
        return first;
    }

    template< class ForwardIt, class Generator >
    void _generate( ForwardIt first, ForwardIt last, Generator g, input_iterator_tag ) {
        while(first != last) {
            *first = g();
            ++first;
        }
    }

    template< class ForwardIt, class Generator >
    void _generate( ForwardIt first, ForwardIt last, Generator g, random_access_iterator_tag ) {
        typename jr_std::iterator_traits<ForwardIt>::difference_type distance;
        for(distance = last - first; distance; --distance) {
            *first = g();
            ++first;
        }
    }

    template< class ForwardIt, class Generator >
    void generate( ForwardIt first, ForwardIt last, Generator g ) {
        jr_std::_generate(first, last, g,
                          typename jr_std::iterator_traits<ForwardIt>::iterator_category());
    }

    template< class OutputIt, class Size, class Generator >
    OutputIt generate_n( OutputIt first, Size count, Generator g ) {
        while(count--) {
            *first = g();
            ++first;
        }
        return first;
    }

    template< class InputIt, class OutputIt, class UnaryOperation >
    OutputIt _transform( InputIt first1, InputIt last1, OutputIt d_first,
                         UnaryOperation unary_op, input_iterator_tag ) {
        while(first1 != last1) {
            *d_first = unary_op(*first1);
            ++first1;
            ++d_first;
        }
    }

    template< class InputIt, class OutputIt, class UnaryOperation >
    OutputIt _transform( InputIt first1, InputIt last1, OutputIt d_first,
                         UnaryOperation unary_op, random_access_iterator_tag ) {
        typename jr_std::iterator_traits<InputIt>::difference_type distance;
        for(distance = last1 - first1; distance; --distance) {
            *d_first = unary_op(*first1);
            ++first1;
            ++d_first;
        }
        return d_first;
    }

    template< class InputIt, class OutputIt, class UnaryOperation >
    OutputIt transform( InputIt first1, InputIt last1, OutputIt d_first,
                        UnaryOperation unary_op ) {
        return _transform(first1, last1, d_first, unary_op,
                          typename jr_std::iterator_traits<InputIt>::iterator_category());
    }

    template< class InputIt1, class InputIt2, class OutputIt, class BinaryOperation >
    OutputIt _transform( InputIt1 first1, InputIt1 last1, InputIt2 first2,
                         OutputIt d_first, BinaryOperation binary_op, input_iterator_tag ) {
        while(first1 != last1) {
            *d_first = binary_op(*first1, *first2);
            ++first1;
            ++first2;
            ++d_first;
        }
    }

    template< class InputIt1, class InputIt2, class OutputIt, class BinaryOperation >
    OutputIt _transform( InputIt1 first1, InputIt1 last1, InputIt2 first2,
                         OutputIt d_first, BinaryOperation binary_op, random_access_iterator_tag ) {
        typename jr_std::iterator_traits<InputIt1>::difference_type distance;
        for(distance = last1 - first1; distance; --distance) {
            *d_first = binary_op(*first1, *first2);
            ++first1;
            ++first2;
            ++d_first;
        }
        return d_first;
    }


    template< class InputIt1, class InputIt2, class OutputIt, class BinaryOperation >
    OutputIt transform( InputIt1 first1, InputIt1 last1, InputIt2 first2,
                        OutputIt d_first, BinaryOperation binary_op ) {
        return _transform(first1, last1, first2, d_first, binary_op,
                          typename jr_std::iterator_traits<InputIt1>::iterator_category());
    }

    template< class InputIt, class OutputIt, class UnaryPredicate >
    OutputIt remove_copy_if( InputIt first, InputIt last, OutputIt d_first,
                             UnaryPredicate p ) {
        while(first != last) {
            if(!p(*first)) {
                *d_first = *first;
                ++d_first;
            }
            ++first;
        }
        return d_first;
    }

    template< class InputIt, class OutputIt, class T >
    OutputIt remove_copy( InputIt first, InputIt last, OutputIt d_first,
                          const T& value ) {
        return jr_std::remove_copy_if(first, last, d_first,
                                      [&value](const T& a)
                                      ->bool { return a == value; });
    }

    template< class ForwardIt, class UnaryPredicate >
    ForwardIt remove_if( ForwardIt first, ForwardIt last, UnaryPredicate p ) {
        ForwardIt next = first = jr_std::find_if(first, last, p);
        return next == last ? last : jr_std::remove_copy_if(++next, last, first, p);
    }

    template< class ForwardIt, class T >
    ForwardIt remove( ForwardIt first, ForwardIt last, const T& value ) {
        return jr_std::remove_if(first, last,
                                 [&value](const T& a)
                                 ->bool { return a == value; });
    }

    template< class ForwardIt, class UnaryPredicate, class T >
    void replace_if( ForwardIt first, ForwardIt last,
                     UnaryPredicate p, const T& new_value ) {
        while(first != last) {
            if(p(*first))
                *first = new_value;
            ++first;
        }
    }

    template< class ForwardIt, class T >
    void replace( ForwardIt first, ForwardIt last,
                  const T& old_value, const T& new_value ) {
        while(first != last) {
            if(*first == old_value)
                *first = new_value;
            ++first;
        }
    }

    template< class InputIt, class OutputIt, class UnaryPredicate, class T >
    OutputIt replace_copy_if( InputIt first, InputIt last, OutputIt d_first,
                              UnaryPredicate p, const T& new_value ) {
        while(first != last) {
            if(p(*first))
                *d_first = new_value;
            else
                *d_first = *first;
            ++first;
            ++d_first;
        }
        return d_first;
    }

    template< class InputIt, class OutputIt, class T >
    OutputIt replace_copy( InputIt first, InputIt last, OutputIt d_first,
                           const T& old_value, const T& new_value ) {
        while(first != last) {
            if(*first == old_value)
                *d_first = new_value;
            else
                *d_first = *first;
            ++first;
            ++d_first;
        }
        return d_first;
    }

    template< class BidirIt >
    void reverse( BidirIt first, BidirIt last ) {
        if(first == last)
            return;
        --last;
        while(first != last) {
            typename jr_std::iterator_traits<BidirIt>::value_type tmp = *first;
            *first = *last;
            *last = tmp;
            ++first;
            if(first == last)
                break;
            --last;
        }
    }

    template< class BidirIt, class OutputIt >
    OutputIt reverse_copy( BidirIt first, BidirIt last, OutputIt d_first ) {
        BidirIt rfirst = jr_std::reverse_iterator<BidirIt>(last);
        BidirIt rlast = jr_std::reverse_iterator<BidirIt>(first);
        while(rfirst != rlast) {
            *d_first = *rfirst;
            ++rfirst;
            ++d_first;
        }
        return d_first;
    }

    template< class ForwardIt >
    ForwardIt _rotate( ForwardIt first, ForwardIt n_first, ForwardIt last, input_iterator_tag ) {
        if(first != n_first){
            ForwardIt tmp_it = n_first, ret = first;
            jr_std::advance(ret, jr_std::distance(n_first, last));
            while(tmp_it != last) {
                while((first != n_first) && (tmp_it != last)) {
                    iter_swap(first, tmp_it);
                    ++first;
                    ++tmp_it;
                }
                // 左长右短
                if(first != n_first) {
                    tmp_it = n_first;
                }
                // 左短右长
                if(tmp_it != last) {
                    n_first = tmp_it;
                }
            }
            return ret;
        } else
            return last;
    }

    template< class ForwardIt >
    ForwardIt _rotate( ForwardIt first, ForwardIt n_first, ForwardIt last, bidectional_iterator_tag ) {
        if(first != n_first) {
            jr_std::reverse(first, n_first);
            jr_std::reverse(n_first, last);
            jr_std::reverse(first, last);
        }
        jr_std::advance(first, jr_std::distance(n_first, last));
        return first;
    }

    template< class ForwardIt >
    ForwardIt rotate( ForwardIt first, ForwardIt n_first, ForwardIt last ) {
        return jr_std::_rotate(first, n_first, last,
                               typename jr_std::iterator_traits<ForwardIt>::iterator_category());
    }

    template< class ForwardIt, class OutputIt >
    OutputIt rotate_copy( ForwardIt first, ForwardIt n_first,
                          ForwardIt last, OutputIt d_first ) {
        ForwardIt tmp_it = n_first;
        while(n_first != last) {
            *d_first = *n_first;
            ++n_first;
            ++d_first;
        }
        while(first != tmp_it) {
            *d_first = *tmp_it;
            ++first;
            ++d_first;
        }
        return d_first;
    }

    template< class ForwardIt, class BinaryPredicate >
    ForwardIt unique( ForwardIt first, ForwardIt last, BinaryPredicate p ) {
        if(first == last)
            return last;
        ForwardIt ret = first;
        while(first != last) {
            ForwardIt next = first;
            ++next;
            if(next == last)
                break;
            if(p(*next, *first)) {
                while(p(*next, *first) && (next != last)) {
                    ++next;
                }
                rotate(++first, next, last);
            } else {
                ++first;
                ++ret;
            }
        }
        return ++ret;
    }

    template< class ForwardIt >
    ForwardIt unique( ForwardIt first, ForwardIt last ) {
        typedef typename jr_std::iterator_traits<ForwardIt>::value_type type;
        return unique(first, last,
                      [](const type& a, const type& b)
                      ->bool { return a == b; });
    }

    template< class InputIt, class OutputIt, class BinaryPredicate >
    OutputIt unique_copy( InputIt first, InputIt last,
                          OutputIt d_first, BinaryPredicate p ) {
        while(first != last) {
            InputIt next = first;
            ++next;
            if(next == last)
                break;
            *d_first = *next;
            ++d_first;
            if(p(*next, *first)) {
                while(p(*next, *first) && (next != last)) {
                    ++next;
                }
                first = next;
            } else {
                ++first;
            }
        }
        return d_first;
    }

    template< class InputIt, class OutputIt >
    OutputIt unique_copy( InputIt first, InputIt last, OutputIt d_first ) {
        typedef typename jr_std::iterator_traits<InputIt>::value_type type;
        return unique_copy(first, last, d_first,
                      [](const type& a, const type& b)
                           ->bool { return a == b; });
    }

    template< class ForwardIt1, class ForwardIt2 >
    ForwardIt2 swap_ranges( ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2 ) {
        while(first1 != last1) {
            iter_swap(first1, first2);
            ++first1;
            ++first2;
        }
        return first2;
    }

    template< class T >
    void swap( T& a, T& b ) noexcept {
        T tmp = a;
        a = b;
        b = tmp;
    }

    template< class T2, size_t N >
    void swap( T2 (&a)[N], T2 (&b)[N]) noexcept {
        swap_ranges(a, a + N, b);
    }

    template< class RandomIt, class URBG >
    void shuffle( RandomIt first, RandomIt last, URBG&& g ) {
        if(first != last) {
            for(RandomIt it = first + 1; it != last; ++it)
                iter_swap(it, first + g((it - first) + 1)); // 在范围内随机交换元素即可
        }
    }

    /*最值操作*/
    template< class T >
    const T& max( const T& a, const T& b ) { return a < b ? b : a; }

    template< class T, class Compare >
    const T& max( const T& a, const T& b, Compare comp ) { return comp(a, b) ? b : a; }

    template< class ForwardIt, class Compare >
    ForwardIt max_element(ForwardIt first, ForwardIt last, Compare comp ) {
        ForwardIt max_it = first;
        while(first != last) {
            if(comp(*max_it, *first))
                max_it = first;
            ++first;
        }
        return max_it;
    }

    template< class ForwardIt >
    ForwardIt max_element(ForwardIt first, ForwardIt last ) {
        ForwardIt max_it = first;
        while(first != last) {
            if(*max_it < *first)
                max_it = first;
            ++first;
        }
        return max_it;
    }

    template< class T >
    const T& min( const T& a, const T& b ) { return a < b ? a : b; }

    template< class T, class Compare >
    const T& min( const T& a, const T& b, Compare comp ) { return comp(a, b) ? a : b; }

    template< class ForwardIt, class Compare >
    ForwardIt min_element(ForwardIt first, ForwardIt last, Compare comp ) {
        ForwardIt min_it = first;
        while(first != last) {
            if(comp(*first, *min_it))
                min_it = first;
            ++first;
        }
        return min_it;
    }

    template< class ForwardIt >
    ForwardIt min_element(ForwardIt first, ForwardIt last ) {
        ForwardIt min_it = first;
        while(first != last) {
            if(*first < *min_it)
                min_it = first;
            ++first;
        }
        return min_it;
    }

    template< class T, class Compare >
    jr_std::pair<const T&,const T&> minmax( const T& a, const T& b, Compare comp )
    { return pair<const T&,const T&>(jr_std::min(a, b, comp), jr_std::max(a, b, comp)); }

    template< class T >
    jr_std::pair<const T&,const T&> minmax( const T& a, const T& b )
    { return pair<const T&,const T&>(jr_std::min(a, b), jr_std::max(a, b)); }

    template< class ForwardIt, class Compare >
    jr_std::pair<ForwardIt,ForwardIt>
    minmax_element( ForwardIt first, ForwardIt last, Compare comp )
    { return jr_std::pair<ForwardIt,ForwardIt>(jr_std::min(first, last, comp),
                                               jr_std::max(first, last, comp)); }

    template< class ForwardIt >
    jr_std::pair<ForwardIt,ForwardIt>
    minmax_element( ForwardIt first, ForwardIt last )
    { return jr_std::pair<ForwardIt,ForwardIt>(jr_std::min(first, last),
                                               jr_std::max(first, last)); }

    /*有序序列的归并操作*/
    // 非原地归并
    template< class InputIt1, class InputIt2, class OutputIt, class Compare>
    OutputIt merge( InputIt1 first1, InputIt1 last1,
                    InputIt2 first2, InputIt2 last2,
                    OutputIt d_first, Compare comp ) {
        if(first1 == last1)
            d_first = copy(first2, last2, d_first);
        else if(first2 == last2)
            d_first = copy(first1, last1, d_first);
        else {
            if(comp(*first1, *first2)) {
                while((first1 != last1) && (first2 != last2)) {
                    if(comp(*first1, *first2)) {
                        *d_first = *first1;
                        ++first1;
                        ++d_first;
                    }
                    else {
                        *d_first = *first2;
                        ++first2;
                        ++d_first;
                    }
                }
            } else {
                while((first1 != last1) && (first2 != last2)) {
                    if(comp(*first2, *first1)) {
                        *d_first = *first2;
                        ++first2;
                        ++d_first;
                    }
                    else {
                        *d_first = *first1;
                        ++first1;
                        ++d_first;
                    }
                }
            }
            while(first1 != last1) {
                *d_first = *first1;
                ++first1;
                ++d_first;
            }
            while(first2 != last2) {
                *d_first = *first2;
                ++first2;
                ++d_first;
            }
        }
        return d_first;
    }

    template< class InputIt1, class InputIt2, class OutputIt >
    OutputIt merge( InputIt1 first1, InputIt1 last1,
                    InputIt2 first2, InputIt2 last2,
                    OutputIt d_first ) {
        typedef typename jr_std::iterator_traits<InputIt1>::value_type type1;
        typedef typename jr_std::iterator_traits<InputIt2>::value_type type2;
        return jr_std::merge(first1, last1, first2, last2, d_first,
                             [](const type1& a, const type2& b)
                             ->bool { return a < b; });
    }

    // 原地归并(完全不同于普通归并！！！！)
    template< class BidirIt, class Compare>
    void inplace_merge( BidirIt first, BidirIt middle, BidirIt last, Compare comp ) {
        if(first == last)
            return;
        else {
            BidirIt it_mid = middle;
            while((first != middle) && (it_mid != last)) {
                while((first != middle) && !comp(*it_mid, *first)) {
                    ++first;
                }
                BidirIt tmp = it_mid;
                ptrdiff_t cnt = 0; // 旋转计数，对于第二序列需要旋转多少个元素进行计数
                while((it_mid != last) && comp(*it_mid, *first)) {
                    ++cnt;
                    ++it_mid;
                }
                jr_std::rotate(first, tmp, it_mid);
                if(first != middle)
                    jr_std::advance(first, cnt);
            }
        }
    }

    template< class BidirIt >
    void inplace_merge( BidirIt first, BidirIt middle, BidirIt last ) {
        typedef typename iterator_traits<BidirIt>::value_type type;
        jr_std::inplace_merge(first, middle, last,
                              [](const type& a, const type& b)
                              ->bool { return a < b; });
    }

    /*比较操作*/
    template< class InputIt1, class InputIt2, class BinaryPredicate >
    bool equal( InputIt1 first1, InputIt1 last1,
                InputIt2 first2, BinaryPredicate p ) {
        while(first1 != last1) {
            if(!p(*first1, *first2))
                return false;
            ++first1;
            ++first2;
        }
        return true;
    }

    template< class InputIt1, class InputIt2 >
    bool equal( InputIt1 first1, InputIt1 last1, InputIt2 first2 ) {
        typedef typename jr_std::iterator_traits<InputIt1>::value_type type1;
        typedef typename jr_std::iterator_traits<InputIt2>::value_type type2;
        return equal(first1, last1, first2,
                     [](const type1& a, const type2& b)
                     ->bool { return a == b; });
    }

    template< class InputIt1, class InputIt2, class Compare >
    bool _lexicographical_compare( InputIt1 first1, InputIt1 last1,
                                   InputIt2 first2, InputIt2 last2,
                                   Compare comp, input_iterator_tag, input_iterator_tag ) {
        while((first1 != last1) && (first2 != last2)) {
            if(!comp(*first1, *first2))
                return false;
            ++first1;
            ++first2;
        }
        return first1 == last1;
    }

    template< class InputIt1, class InputIt2, class Compare >
    bool _lexicographical_compare( InputIt1 first1, InputIt1 last1,
                                   InputIt2 first2, InputIt2 last2,
                                   Compare comp,
                                   random_access_iterator_tag,
                                   random_access_iterator_tag ) {
        if(comp(last1-first1, last2-first2))
            return true;
        if(comp(last2-first2, last1-first1))
            return false;
        while(first1 != last1) {
            if(!comp(*first1, *first2))
                return false;
            ++first1;
            ++first2;
        }
        return true;
    }

    template< class InputIt1, class InputIt2, class Compare >
    bool lexicographical_compare( InputIt1 first1, InputIt1 last1,
                                  InputIt2 first2, InputIt2 last2,
                                  Compare comp ) {
        return jr_std::_lexicographical_compare(first1, last1, first2, last2, comp,
                                        jr_std::iterator_traits<InputIt1>::iterator_category(),
                                        jr_std::iterator_traits<InputIt2>::iterator_category());
    }

    template< class InputIt1, class InputIt2 >
    bool lexicographical_compare( InputIt1 first1, InputIt1 last1,
                                  InputIt2 first2, InputIt2 last2 ) {
        typedef typename jr_std::iterator_traits<InputIt1>::value_type type1;
        typedef typename jr_std::iterator_traits<InputIt2>::value_type type2;
        return jr_std::lexicographical_compare(first1, last1, first2, last2,
                                               [](const type1& a, const type2& b)
                                               ->bool { return a < b; });
    }

    /*二分搜索操作*/
    template< class ForwardIt, class T, class Compare >
    ForwardIt lower_bound( ForwardIt first, ForwardIt last, const T& value, Compare comp ) {
        if(first == last)
            return first;
        typedef typename jr_std::iterator_traits<ForwardIt>::difference_type dis_type;
        dis_type start = 0, end = jr_std::distance(first, last);
        while(start < end) {
            dis_type mid = start + (end - start) / 2;
            ForwardIt middle = first;
            jr_std::advance(middle, mid);
            if(comp(*middle, value)) {
                start = mid + 1;
            } else {
                end = mid;

            }
        }
        jr_std::advance(first, start);
        return first;
    }

    template< class ForwardIt, class T >
    ForwardIt lower_bound( ForwardIt first, ForwardIt last, const T& value ) {
        return jr_std::lower_bound(first, last, value,
                                   [](const T& a, const T& b)
                                   ->bool { return a < b; });
    }

    template< class ForwardIt, class T, class Compare >
    ForwardIt upper_bound( ForwardIt first, ForwardIt last, const T& value, Compare comp ) {
        if(first == last || comp(value, *first))
            return first;
        ForwardIt middle, ret;
        typedef typename jr_std::iterator_traits<ForwardIt>::difference_type dis_type;
        dis_type start = 0, end = jr_std::distance(first, last);
        while(start < end) {
            dis_type mid = start + (end - start) / 2;
            ForwardIt middle = first;
            jr_std::advance(middle, mid);
            if(comp(value, *middle)) {
                end = mid;
            } else {
                start = mid + 1;
                ret = middle;
                ++ret;
            }
        }
        return ret;
    }

    template< class ForwardIt, class T >
    ForwardIt upper_bound( ForwardIt first, ForwardIt last, const T& value ) {
        return jr_std::upper_bound(first, last, value,
                                   [](const T& a, const T& b)
                                   ->bool { return a < b; });
    }

    template< class ForwardIt, class T, class Compare >
    bool binary_search( ForwardIt first, ForwardIt last, const T& value, Compare comp ) {
        if(first == last)
            return false;
        typedef typename iterator_traits<ForwardIt>::difference_type dis_type;
        dis_type start = 0, end = jr_std::distance(first, last) - 1;
        while(start <= end) {
            dis_type mid = start + (end - start) / 2;
            ForwardIt middle = first;
            jr_std::advance(middle, mid);
            if(comp(*middle, value)) {
                start = mid + 1;
            } else if(comp(value, *middle)) {
                end = mid - 1;
            } else {
                return true;
            }
        }
        return false;
    }

    template< class ForwardIt, class T >
    bool binary_search( ForwardIt first, ForwardIt last, const T& value ) {
        return jr_std::binary_search(first, last, value,
                                     [](const T& a, const T& b)
                                     ->bool { return a < b; });
    }

    template< class ForwardIt, class T, class Compare >
    jr_std::pair<ForwardIt,ForwardIt>
    equal_range( ForwardIt first, ForwardIt last, const T& value, Compare comp ) {
        return jr_std::pair<ForwardIt,ForwardIt>(jr_std::lower_bound(first, last, value, comp),
                                                 jr_std::upper_bound(first, last, value, comp));
    }

    template< class ForwardIt, class T >
    jr_std::pair<ForwardIt,ForwardIt>
    equal_range( ForwardIt first, ForwardIt last, const T& value ) {
        return jr_std::equal_range(first, last, value,
                                   [](const T& a, const T& b)
                                   ->bool { return a < b; });
    }

    /*划分操作*/
    template< class InputIt, class UnaryPredicate >
    bool is_partitioned( InputIt first, InputIt last, UnaryPredicate p ) {
        for(; (first != last) && p(*first); ++first);
        while(first != last) {
            if(p(*first))
                return false;
            ++first;
        }
        return true;
    }

    template< class ForwardIt, class UnaryPredicate >
    ForwardIt partition_point( ForwardIt first, ForwardIt last, UnaryPredicate p ) {
        for(; (first != last) && p(*first); ++first);
        return first;
    }

    template< class InputIt, class OutputIt1,
              class OutputIt2, class UnaryPredicate >
    jr_std::pair<OutputIt1, OutputIt2>
    partition_copy( InputIt first, InputIt last,
                    OutputIt1 d_first_true, OutputIt2 d_first_false,
                    UnaryPredicate p ) {
        while(first != last) {
            if(p(*first)) {
                *d_first_true = *first;
                ++d_first_true;
            } else{
                *d_first_false = *first;
                ++d_first_false;
            }
            ++first;
        }
        return jr_std::pair<OutputIt1, OutputIt2>(d_first_true, d_first_false);
    }

    // 非稳定划分（！！！）
    template< class ForwardIt, class UnaryPredicate >
    ForwardIt partition( ForwardIt first, ForwardIt last, UnaryPredicate p ) {
        ForwardIt tmp = first;
        while(first != last) {
            for(; (first != last) && p(*first); ++first);
            if(first == last)
                break;
            tmp = first;
            for(; (first != last) && !p(*first); ++first);
            if(first == last)
                break;
            iter_swap(tmp, first);
            first = tmp;
        }
        return tmp;
    }

    // 稳定划分（！！！）
    template< class BidirIt, class RandomIt, class UnaryPredicate >
    BidirIt _stable_partition_with_buffer( BidirIt first, BidirIt last,
                                           RandomIt buffer,
                                           UnaryPredicate p ) {
        BidirIt it, ret;
        RandomIt bit = buffer;
        for(it = first; it != last; ++it) {
            if(p(*it)) {
                *buffer = *it;
                ret = it;
                ++buffer;
            }
        }
        for(it = first; it != last; ++it) {
            if(!p(*it)) {
                *buffer = *it;
                ++buffer;
            }
        }
        for(; bit != buffer; ++bit) {
            *first = *bit;
            if(p(*first))
                ret = first;
            ++first;
        }
        return ++ret;
    }

    // TODO: 不使用jr_std::partition_point
    template< class BidirIt, class UnaryPredicate >
    BidirIt _stable_partition_without_buffer( BidirIt first, BidirIt last,
                                             UnaryPredicate p ) {
        BidirIt ret = first;
        while(first != last) {
            if(!p(*first)) {
                BidirIt tmp = first;
                for(; (first != last) && !p(*first); ++first);
                jr_std::rotate(tmp, first, last);
            } else {
                ++first;
            }
        }
        return jr_std::partition_point(ret, last, p);
    }

    template< class BidirIt, class UnaryPredicate >
    BidirIt stable_partition( BidirIt first, BidirIt last, UnaryPredicate p ) {
        typedef typename iterator_traits<BidirIt>::value_type type;
        typename iterator_traits<BidirIt>::difference_type len;
        len = jr_std::distance(first, last);
        type *buffer = nullptr;
        buffer = new type[len];
        if(buffer) {
            // 内存充足，可分配缓冲区
            auto ret = _stable_partition_with_buffer(first, last, buffer, p);
            delete []buffer;
            return ret;
        } else {
            // 内存不足，不可分配缓冲区
            return _stable_partition_without_buffer(first, last, p);
        }
    }

    /*集合操作*/
    template< class InputIt1, class InputIt2, class Compare >
    bool includes( InputIt1 first1, InputIt1 last1,
                   InputIt2 first2, InputIt2 last2, Compare comp );

    template< class InputIt1, class InputIt2 >
    bool includes( InputIt1 first1, InputIt1 last1,
                   InputIt2 first2, InputIt2 last2 );

    template< class InputIt1, class InputIt2,
              class OutputIt, class Compare >
    OutputIt set_difference( InputIt1 first1, InputIt1 last1,
                             InputIt2 first2, InputIt2 last2,
                             OutputIt d_first, Compare comp );

    template< class InputIt1, class InputIt2, class OutputIt >
    OutputIt set_difference( InputIt1 first1, InputIt1 last1,
                             InputIt2 first2, InputIt2 last2,
                             OutputIt d_first );

    template< class InputIt1, class InputIt2,
              class OutputIt, class Compare >
    OutputIt set_intersection( InputIt1 first1, InputIt1 last1,
                               InputIt2 first2, InputIt2 last2,
                               OutputIt d_first, Compare comp );

    template< class InputIt1, class InputIt2, class OutputIt >
    OutputIt set_intersection( InputIt1 first1, InputIt1 last1,
                               InputIt2 first2, InputIt2 last2,
                               OutputIt d_first );

    template< class InputIt1, class InputIt2,
              class OutputIt, class Compare >
    OutputIt set_symmetric_difference( InputIt1 first1, InputIt1 last1,
                                       InputIt2 first2, InputIt2 last2,
                                       OutputIt d_first, Compare comp );

    template< class InputIt1, class InputIt2, class OutputIt >
    OutputIt set_symmetric_difference( InputIt1 first1, InputIt1 last1,
                                       InputIt2 first2, InputIt2 last2,
                                       OutputIt d_first );

    template< class InputIt1, class InputIt2,
              class OutputIt, class Compare >
    OutputIt set_union( InputIt1 first1, InputIt1 last1,
                        InputIt2 first2, InputIt2 last2,
                        OutputIt d_first, Compare comp );

    template< class InputIt1, class InputIt2, class OutputIt >
    OutputIt set_union( InputIt1 first1, InputIt1 last1,
                        InputIt2 first2, InputIt2 last2,
                        OutputIt d_first );

    /*堆操作*/
    // 向上过滤
    template< class RandomIt, class Compare >
    void push_heap( RandomIt first, RandomIt last,
                    Compare comp ) {
        if(first == last - 1)   return;
        RandomIt target = last - 1;
        ptrdiff_t index = jr_std::distance(first, target - 1);
        typename jr_std::iterator_traits<RandomIt>::value_type tmp;
        while((index >= 0) && comp(first[index], *target)) {
            tmp = first[index];
            first[index] = *target;
            *target = tmp;
            target = first + index;
            index = (index - 1) / 2;
        }
    }

    template< class RandomIt >
    void push_heap( RandomIt first, RandomIt last ) {
        typedef typename iterator_traits<RandomIt>::value_type type;
        jr_std::push_heap(first, last,
                          [](const type& x, const type& y)
                          ->bool { return x < y; });
    }

    template< class RandomIt, class Compare >
    void make_heap( RandomIt first, RandomIt last, Compare comp ) {
        RandomIt tmp = first + 1;
        while(tmp != last)
            jr_std::push_heap(first, tmp++, comp);
        jr_std::push_heap(first, last, comp);
    }

    template< class RandomIt >
    void make_heap( RandomIt first, RandomIt last ) {
        typedef typename jr_std::iterator_traits<RandomIt>::value_type type;
        jr_std::make_heap(first, last,
                          [](const type& x, const type& y)
                          ->bool { return x < y; });
    }

    // 向下过滤
    template< class RandomIt, class Compare >
    void pop_heap( RandomIt first, RandomIt last, Compare comp ) {
        if(first == last - 1)   return;
        RandomIt target = last - 1;
        // 交换first与last-1位置的元素, 同时保存交换前的堆尾元素
        // 即交换堆顶与堆尾的元素
        typename jr_std::iterator_traits<RandomIt>::value_type tmp;
        tmp = *target;
        *target = *first;
        *first = tmp;
        // 上滤节点，将其调整为最大堆
        ptrdiff_t len = jr_std::distance(first, target - 1);
        ptrdiff_t parent = 0, child = parent * 2 + 1;
        while(child < len) {
            // 在左右儿子里找最大的那个
            if((child < len)
             &&(child + 1 < len)
             && comp(first[child], first[child + 1]))
                ++child;
            //  父节点 < 最大的儿子
            if(comp(first[parent], first[child])) {
                tmp = first[parent];
                first[parent] = first[child];
                first[child] = tmp;
            }
            // 反之则说明堆已经调整好了
            else
                break;
            parent = child;
            child = parent * 2 + 1;
        }
        first[parent] = tmp;
    }

    template< class RandomIt >
    void pop_heap( RandomIt first, RandomIt last ) {
        typedef typename jr_std::iterator_traits<RandomIt>::value_type type;
        jr_std::pop_heap(first, last,
                         [](const type& x, const type& y)
                         ->bool { return x < y; });
    }

    template< class RandomIt, class Compare >
    void sort_heap( RandomIt first, RandomIt last, Compare comp ) {
        while(first != last)
            jr_std::pop_heap(first, last--, comp);
    }

    template< class RandomIt >
    void sort_heap( RandomIt first, RandomIt last ) {
        typedef typename jr_std::iterator_traits<RandomIt>::value_type type;
        jr_std::sort_heap(first, last,
                          [](const type& x, const type& y)
                          ->bool { return x < y; });
    }
    /*C++11新增*/
    template< class RandomIt, class Compare >
    bool is_heap( RandomIt first, RandomIt last, Compare comp ) {
        if(first == last || first == last - 1)   return true;
        ptrdiff_t len = jr_std::distance(first, last - 1);
        ptrdiff_t parent = 0, child = parent * 2 + 1;
        while(child < len) {
            if(child < len) {
                if(comp(first[parent], first[child]))
                    return false;
                else {
                    ++child;
                    if((child < len)
                     && comp(first[parent], first[child]))
                        return false;
                }
            } else {
                break;
            }
            parent++;
            child = parent * 2 + 1;
        }
        return true;
    }

    template< class RandomIt >
    bool is_heap( RandomIt first, RandomIt last ) {
        typedef typename jr_std::iterator_traits<RandomIt>::value_type type;
        return jr_std::is_heap(first, last,
                               [](const type& x, const type& y)
                               ->bool { return x < y; });
    }

    template< class RandomIt, class Compare >
    RandomIt is_heap_until( RandomIt first, RandomIt last, Compare comp ) {
        while((first != last) && is_heap(first, last--, comp));
        return last;
    }

    template< class RandomIt >
    RandomIt is_heap_until( RandomIt first, RandomIt last ) {
        typedef typename jr_std::iterator_traits<RandomIt>::value_type type;
        return jr_std::is_heap_until(first, last,
                                     [](const type& x, const type& y)
                                     ->bool { return x < y; });
    }

    /*排列操作*/
    template< class ForwardIt1, class ForwardIt2, class BinaryPredicate >
    bool is_permutation( ForwardIt1 first1, ForwardIt1 last1,
                         ForwardIt2 first2, BinaryPredicate p );

    template< class ForwardIt1, class ForwardIt2 >
    bool is_permutation( ForwardIt1 first1, ForwardIt1 last1,
                         ForwardIt2 first2 );

    template< class BidirIt, class Compare >
    bool next_permutation( BidirIt first, BidirIt last, Compare comp );

    template< class BidirIt >
    bool next_permutation( BidirIt first, BidirIt last );

    template< class BidirIt, class Compare >
    bool prev_permutation( BidirIt first, BidirIt last, Compare comp);

    template< class BidirIt >
    bool prev_permutation( BidirIt first, BidirIt last);

    /*排序操作*/
    template< class ForwardIt, class Compare >
    bool is_sorted( ForwardIt first, ForwardIt last, Compare comp );

    template< class ForwardIt >
    bool is_sorted( ForwardIt first, ForwardIt last );

    template< class ForwardIt, class Compare >
    ForwardIt is_sorted_until( ForwardIt first, ForwardIt last,
                               Compare comp );

    template< class ForwardIt >
    ForwardIt is_sorted_until( ForwardIt first, ForwardIt last );

    template< class RandomIt, class Compare >
    void sort( RandomIt first, RandomIt last, Compare comp );

    template< class RandomIt >
    void sort( RandomIt first, RandomIt last );

    template< class RandomIt, class Compare >
    void partial_sort( RandomIt first, RandomIt middle, RandomIt last,
                       Compare comp );

    template< class RandomIt >
    void partial_sort( RandomIt first, RandomIt middle, RandomIt last );

    template< class InputIt, class RandomIt, class Compare >
    RandomIt partial_sort_copy( InputIt first, InputIt last,
                                RandomIt d_first, RandomIt d_last,
                                Compare comp );

    template< class InputIt, class RandomIt >
    RandomIt partial_sort_copy( InputIt first, InputIt last,
                                RandomIt d_first, RandomIt d_last );

    template< class RandomIt, class Compare >
    void stable_sort( RandomIt first, RandomIt last, Compare comp );

    template< class RandomIt >
    void stable_sort( RandomIt first, RandomIt last );

    template< class RandomIt, class Compare >
    void nth_element( RandomIt first, RandomIt nth, RandomIt last,
                      Compare comp );

    template< class RandomIt >
    void nth_element( RandomIt first, RandomIt nth, RandomIt last );
}

#endif // JR_ALGORITHM_H
