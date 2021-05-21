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
    typename iterator_traits<InputIt>::difference_type
    count_if( InputIt first, InputIt last, UnaryPredicate p ) {
        typename iterator_traits<InputIt>::difference_type cnt = 0;
        while(first != last) {
            if(p(*first))   ++cnt;
            ++first;
        }
        return cnt;
    }

    template< class InputIt, class T >
    typename iterator_traits<InputIt>::difference_type
    count( InputIt first, InputIt last, const T &value )
    { return count_if(first, last, [&value](const T& m)->bool { return value == m; }); }

    template< class InputIt1, class InputIt2, class BinaryPredicate >
    pair<InputIt1, InputIt2>
    mismatch( InputIt1 first1, InputIt1 last1, InputIt2 first2, BinaryPredicate p ) {
        while(first1 != last1) {
            if(!p(*first1, *first2))
                break;
            ++first1;
            ++first2;
        }
        return pair<InputIt1, InputIt2>(first1, first2);
    }

    template< class InputIt1, class InputIt2 >
    pair<InputIt1, InputIt2>
    mismatch( InputIt1 first1, InputIt1 last1, InputIt2 first2 ) {
        typedef typename iterator_traits<InputIt1>::value_type type1;
        typedef typename iterator_traits<InputIt2>::value_type type2;
        return mismatch(first1, last1, first2,
                        [](const type1& a, const type2& b)->bool { return a == b; });
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
        return find_if(first, last, [&value](const T& a)->bool { return value == a; });
    }

    template< class ForwardIt1, class ForwardIt2, class BinaryPredicate >
    ForwardIt1 find_end( ForwardIt1 first, ForwardIt1 last,
                         ForwardIt2 s_first, ForwardIt2 s_last, BinaryPredicate p ) {
        ForwardIt1 rfirst = reverse_iterator<ForwardIt1>(last);
        ForwardIt1 rlast = reverse_iterator<ForwardIt1>(first);
        ForwardIt1 rs_first = reverse_iterator<ForwardIt2>(s_last);
        ForwardIt1 rs_last = reverse_iterator<ForwardIt2>(s_first);
        while(rfirst != rlast) {
            if(p(*rfirst, *rs_first)) {
                ForwardIt1 tmp1 = rfirst;
                ForwardIt2 tmp2 = rs_first;
                while((tmp1 != rlast) && (tmp2 != rs_last) && p(*tmp1, *tmp2)) {
                    ++tmp1;
                    ++tmp2;
                }
                if(tmp2 == rs_last)
                    break;
            } else{
                ++rfirst;
            }
        }
        return rfirst.base();
    }

    template< class ForwardIt1, class ForwardIt2 >
    ForwardIt1 find_end( ForwardIt1 first, ForwardIt1 last,
                         ForwardIt2 s_first, ForwardIt2 s_last ) {
        typedef typename iterator_traits<ForwardIt1>::value_type type1;
        typedef typename iterator_traits<ForwardIt2>::value_type type2;
        return find_end(first, last, s_first, s_last,
                        [](const type1& a, const type2& b)->bool { return a == b; });
    }

    template< class InputIt, class ForwardIt, class BinaryPredicate >
    InputIt find_first_of( InputIt first, InputIt last,
                           ForwardIt s_first, ForwardIt s_last, BinaryPredicate p ) {
        typedef typename jr_std::iterator_traits<InputIt>::value_type type;
        while(first != last) {
            if(s_last != find_if(s_first, s_last, [=](type a)->bool { return p(a, *first); })) {
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
        return find_first_of(first, last, s_first, s_last,
                             [](const type1& a, const type2& b)->bool { return a == b; });
    }

    template< class ForwardIt1, class ForwardIt2, class BinaryPredicate >
    ForwardIt1 search( ForwardIt1 first, ForwardIt1 last,
                       ForwardIt2 s_first, ForwardIt2 s_last, BinaryPredicate p ) {
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
            } else{
                ++first;
            }
        }
        return first;
    }

    template< class ForwardIt1, class ForwardIt2 >
    ForwardIt1 search( ForwardIt1 first, ForwardIt1 last,
                       ForwardIt2 s_first, ForwardIt2 s_last ) {
        typedef typename iterator_traits<ForwardIt1>::value_type type1;
        typedef typename iterator_traits<ForwardIt2>::value_type type2;
        return search(first, last, s_first, s_last,
                        [](const type1& a, const type2& b)->bool { return a == b; });
    }

    template< class ForwardIt, class Size, class T, class BinaryPredicate >
    ForwardIt search_n( ForwardIt first, ForwardIt last, Size count, const T& value,
                        BinaryPredicate p ) {
        if(!count)  return first;
        while(first != last) {
            if(p(*first, value)) {
                Size cnt = count;
                ForwardIt tmp = first;
                while((tmp != last) && cnt && p(*tmp, value)) {
                    ++tmp;
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
        return search(first, last, count, value,
                        [](const T& a, const T& b)->bool { return a == b; });
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
        return adjacent_find(first, last,
                             [](const type& a, const type& b)->bool { return a == b; });
    }

    /*修改序列的操作*/
    //逐个拷贝元素，循环结束条件为循环次数达到last-first次
    template< class InputIt, class OutputIt >
    OutputIt _copy_d( InputIt first, InputIt last, OutputIt d_first ) {
        typename iterator_traits<InputIt>::difference_type distance;
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
        return _copy_d(first, last, d_first);
    }

    // 分发器(包装成仿函数是因为函数不能特例化)
    template< class InputIt, class OutputIt >
    struct _copy_dispatch {
        OutputIt operator()( InputIt first, InputIt last, OutputIt d_first ) {
            return _copy(first, last, d_first, typename iterator_traits<InputIt>::iterator_category());
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
                return _copy_d(first, last, d_first);
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
                return _copy_d(first, last, d_first);
        }
    };

    // 完全泛化版本
    template< class InputIt, class OutputIt >
    OutputIt copy( InputIt first, InputIt last, OutputIt d_first ) {
        return _copy_dispatch<InputIt, OutputIt >()(first, last, d_first);
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
        return _copy_if(first, last, d_first, pred,
                        iterator_traits<InputIt>::iterator_category());
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
        auto res = copy(reverse_iterator<BidirIt1>(last), reverse_iterator<BidirIt1>(first), reverse_iterator<BidirIt2>(d_last));
        return res.base();
    }

    template< class InputIt, class OutputIt >
    OutputIt _move( InputIt first, InputIt last, OutputIt d_first, input_iterator_tag ) {
        while(first != last) {
            *d_first = std::move(*first);
            ++d_first;
            ++first;
        }
        return d_first;
    }

    template< class InputIt, class OutputIt >
    OutputIt _move( InputIt first, InputIt last, OutputIt d_first, random_access_iterator_tag ) {
        typename iterator_traits<InputIt>::difference_type distance;
        for(distance = last - first; distance; --distance) {
            *d_first = std::move(*first);
            ++first;
            ++d_first;
        }
        return d_first;
    }

    template< class InputIt, class OutputIt >
    OutputIt move( InputIt first, InputIt last, OutputIt d_first ) {
        return _move(first, last, d_first, iterator_traits<InputIt>::iterator_category());
    }

    template< class BidirIt1, class BidirIt2 >
    BidirIt2 move_backward( BidirIt1 first, BidirIt1 last, BidirIt2 d_last ) {
        return move(reverse_iterator<BidirIt1>(last), reverse_iterator<BidirIt1>(first), reverse_iterator<BidirIt2>(d_last));
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
        typename iterator_traits<ForwardIt>::difference_type distance;
        for(distance = last - first; distance; --distance) {
            *first = value;
            ++first;
        }
    }

    template< class ForwardIt, class T >
    void fill( ForwardIt first, ForwardIt last, const T& value ) {
        _fill(first, last, value, iterator_traits<ForwardIt>::iterator_category());
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
            *first = g(*first);
            ++first;
        }
    }

    template< class ForwardIt, class Generator >
    void _generate( ForwardIt first, ForwardIt last, Generator g, random_access_iterator_tag ) {
        typename iterator_traits<ForwardIt>::difference_type distance;
        for(distance = last - first; distance; --distance) {
            *first = g(*first);
            ++first;
        }
    }

    template< class ForwardIt, class Generator >
    void generate( ForwardIt first, ForwardIt last, Generator g ) {
        _generate(first, last, g, typename iterator_traits<ForwardIt>::iterator_category());
    }

    template< class OutputIt, class Size, class Generator >
    OutputIt generate_n( OutputIt first, Size count, Generator g ) {
        while(count--) {
            *first = g(*first);
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
        typename iterator_traits<InputIt>::difference_type distance;
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
        return _transform(first1, last1, d_first, unary_op);
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
        return remove_copy_if(first, last, d_first,
                              [&value](const T& a)->bool { return a == value; });
    }

    template< class ForwardIt, class UnaryPredicate >
    ForwardIt remove_if( ForwardIt first, ForwardIt last, UnaryPredicate p ) {
        ForwardIt next = first = find_if(first, last, p);
        return next == last ? last : remove_copy_if(++next, last, first, p);
    }

    template< class ForwardIt, class T >
    ForwardIt remove( ForwardIt first, ForwardIt last, const T& value ) {
        return remove_if(first, last, [&value](const T& a)->bool { return a == value; });
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
            typename iterator_traits<BidirIt>::value_type tmp = *first;
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
        BidirIt rfirst = reverse_iterator<BidirIt>(last);
        BidirIt rlast = reverse_iterator<BidirIt>(first);
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
            while(n_first != last) {
                ForwardIt tmp_it = n_first;
                while(first != tmp_it) {
                    typename iterator_traits<ForwardIt>::value_type tmp = *first;
                    *first = *n_first;
                    *n_first = tmp;
                    ++first;
                    ++n_first;
                }
            }
        }
        return first;
    }

    template< class ForwardIt >
    ForwardIt _rotate( ForwardIt first, ForwardIt n_first, ForwardIt last, bidectional_iterator_tag ) {
        if(first != n_first) {
            reverse(first, n_first);
            reverse(n_first, last);
            reverse(first, last);
        }
        return first;
    }

    template< class ForwardIt >
    ForwardIt rotate( ForwardIt first, ForwardIt n_first, ForwardIt last ) {
        return _rotate(first, n_first, last, typename iterator_traits<ForwardIt>::iterator_category());
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
        while(first != last) {
            ForwardIt next = first;
            ++next;
            if(next == last)
                break;
            if(p(*next, *first)) {
                while(p(*next, *first) && (next != last)) {
                    ++next;
                }
                if(next == last)
                    break;
                ++first;
                for(ForwardIt it = next; first != it; ++first) {
                    *first = *next;
                    ++next;
                }
            } else {
                ++first;
            }
        }
        return first;
    }

    template< class ForwardIt >
    ForwardIt unique( ForwardIt first, ForwardIt last ) {
        typedef typename iterator_traits<ForwardIt>::value_type type;
        return unique(first, last,
                      [](const type& a, const type& b)->bool { return a == b; });
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
        typedef typename iterator_traits<InputIt>::value_type type;
        return unique_copy(first, last, d_first,
                      [](const type& a, const type& b)->bool { return a == b; });
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

    template< class ForwardIt1, class ForwardIt2 >
    void iter_swap( ForwardIt1 a, ForwardIt2 b ) {
        typename iterator_traits<ForwardIt1>::value_type tmp = *a;
        *a = *b;
        *b = tmp;
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
    pair<const T&,const T&> minmax( const T& a, const T& b, Compare comp )
    { return pair<const T&,const T&>(min(a, b, comp), max(a, b, comp)); }

    template< class T >
    pair<const T&,const T&> minmax( const T& a, const T& b )
    { return pair<const T&,const T&>(min(a, b), max(a, b)); }

    template< class ForwardIt, class Compare >
    pair<ForwardIt,ForwardIt>
    minmax_element( ForwardIt first, ForwardIt last, Compare comp )
    { return pair<ForwardIt,ForwardIt>(min(first, last, comp), max(first, last, comp)); }

    template< class ForwardIt >
    pair<ForwardIt,ForwardIt>
    minmax_element( ForwardIt first, ForwardIt last )
    { return pair<ForwardIt,ForwardIt>(min(first, last), max(first, last)); }

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
        typedef typename iterator_traits<InputIt1>::value_type type1;
        typedef typename iterator_traits<InputIt2>::value_type type2;
        return merge(first1, last1, first2, last2, d_first,
                     [](const type1& a, const type2& b)->bool { return a < b; });
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
                rotate(first, tmp, it_mid);
                if(first != middle)
                    jr_std::advance(first, cnt);
            }
        }
    }

    template< class BidirIt >
    void inplace_merge( BidirIt first, BidirIt middle, BidirIt last ) {
        typedef typename iterator_traits<BidirIt>::value_type type;
        return inplace_merge(first, middle, last,
                     [](const type& a, const type& b)->bool { return a < b; });
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
        typedef typename iterator_traits<InputIt1>::value_type type1;
        typedef typename iterator_traits<InputIt2>::value_type type2;
        return equal(first1, last1, first2,
                     [](const type1& a, const type2& b)->bool { return a == b; });
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
                                   Compare comp, random_access_iterator_tag, random_access_iterator_tag ) {
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
        return _lexicographical_compare(first1, last1, first2, last2, comp,
                                        iterator_traits<InputIt1>::iterator_category(),
                                        iterator_traits<InputIt2>::iterator_category());
    }

    template< class InputIt1, class InputIt2 >
    bool lexicographical_compare( InputIt1 first1, InputIt1 last1,
                                  InputIt2 first2, InputIt2 last2 ) {
        typedef typename iterator_traits<InputIt1>::value_type type1;
        typedef typename iterator_traits<InputIt2>::value_type type2;
        return lexicographical_compare(first1, last1, first2, last2,
                                       [](const type1& a, const type2& b)->bool { return a < b; });
    }

    /*二分搜索操作*/
    template< class ForwardIt, class T, class Compare >
    ForwardIt lower_bound( ForwardIt first, ForwardIt last, const T& value, Compare comp ) {
        if(first == last)
            return first;
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
                return middle;
            }
        }
        return last;
    }

    template< class ForwardIt, class T >
    ForwardIt lower_bound( ForwardIt first, ForwardIt last, const T& value ) {
        return lower_bound(first, last, value, [](const T& a, const T& b)->bool { return a < b; });
    }

    template< class ForwardIt, class T, class Compare >
    ForwardIt upper_bound( ForwardIt first, ForwardIt last, const T& value, Compare comp ) {
        if(first == last || comp(value, *first))
            return first;
        ForwardIt middle, ret = last;
        typedef typename iterator_traits<ForwardIt>::difference_type dis_type;
        dis_type start = 0, end = jr_std::distance(first, last) - 1;
        while(start <= end) {
            dis_type mid = start + (end - start) / 2;
            ForwardIt middle = first;
            jr_std::advance(middle, mid);
            if(comp(*middle, value)) {
                start = mid + 1;
            } else {
                ret = middle;
                ++ret;
                end = mid - 1;
            }
        }
        return ret;
    }

    template< class ForwardIt, class T >
    ForwardIt upper_bound( ForwardIt first, ForwardIt last, const T& value ) {
        return upper_bound(first, last, value, [](const T& a, const T& b)->bool { return a < b; });
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
        return binary_search(first, last, value,
                             [](const T& a, const T& b)->bool { return a < b; });
    }

    template< class ForwardIt, class T, class Compare >
    pair<ForwardIt,ForwardIt>
    equal_range( ForwardIt first, ForwardIt last, const T& value, Compare comp ) {
        return pair<ForwardIt,ForwardIt>(lower_bound(first, last, value, comp),
                                         upper_bound(first, last, value, comp));
    }

    template< class ForwardIt, class T >
    pair<ForwardIt,ForwardIt>
    equal_range( ForwardIt first, ForwardIt last, const T& value ) {
        return equal_range(first, last, value,
                           [](const T& a, const T& b)->bool { return a < b; });
    }

    /*划分操作*/
    template< class InputIt, class UnaryPredicate >
    bool is_partitioned( InputIt first, InputIt last, UnaryPredicate p );

    template< class ForwardIt, class UnaryPredicate >
    ForwardIt partition_point( ForwardIt first, ForwardIt last, UnaryPredicate p );

    template< class ForwardIt, class UnaryPredicate >
    ForwardIt partition( ForwardIt first, ForwardIt last, UnaryPredicate p );

    template< class InputIt, class OutputIt1,
              class OutputIt2, class UnaryPredicate >
    pair<OutputIt1, OutputIt2>
    partition_copy( InputIt first, InputIt last,
                    OutputIt1 d_first_true, OutputIt2 d_first_false,
                    UnaryPredicate p );

    template< class BidirIt, class UnaryPredicate >
    BidirIt stable_partition( BidirIt first, BidirIt last, UnaryPredicate p );

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
        push_heap(first, last,
                  [=](auto x, auto y)->bool { return x < y; });
    }

    template< class RandomIt, class Compare >
    void make_heap( RandomIt first, RandomIt last, Compare comp ) {
        RandomIt tmp = first + 1;
        while(tmp != last)
            push_heap(first, tmp++, comp);
        push_heap(first, last, comp);
    }

    template< class RandomIt >
    void make_heap( RandomIt first, RandomIt last ) {
        make_heap(first, last,
                  [=](auto x, auto y)->bool { return x < y; });
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
        pop_heap(first, last,
                  [=](auto x, auto y)->bool { return x < y; });
    }

    template< class RandomIt, class Compare >
    void sort_heap( RandomIt first, RandomIt last, Compare comp ) {
        while(first != last)
            pop_heap(first, last--, comp);
    }

    template< class RandomIt >
    void sort_heap( RandomIt first, RandomIt last ) {
        sort_heap(first, last,
                  [=](auto x, auto y)->bool { return x < y; });
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
        return is_heap(first, last,
                  [=](auto x, auto y)->bool { return x < y; });
    }

    template< class RandomIt, class Compare >
    RandomIt is_heap_until( RandomIt first, RandomIt last, Compare comp ) {
        while((first != last) && is_heap(first, last--, comp));
        return last;
    }

    template< class RandomIt >
    RandomIt is_heap_until( RandomIt first, RandomIt last ) {
        return is_heap_until(first, last,
                  [=](auto x, auto y)->bool { return x < y; });
    }
}

#endif // JR_ALGORITHM_H
