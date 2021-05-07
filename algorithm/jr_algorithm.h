#ifndef JR_ALGORITHM_H
#define JR_ALGORITHM_H
#include <iostream>
#include <cstddef>
#include "../iterator/jr_iterator.h"

namespace jr_std {
    /*向上过滤*/
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

    /*向下过滤*/
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
