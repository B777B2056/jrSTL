#ifndef JR_ALGORITHM_H
#define JR_ALGORITHM_H

#include <cstddef>
#include "../iterator/jr_iterator.h"

namespace jr_std {
    template< class RandomIt, class Compare >
    void _filter_up( RandomIt first, RandomIt last,
                     Compare comp ) {
        RandomIt target = last - 1;
        ptrdiff_t index = jr_std::distance(first, target - 1);
        while(index && !comp(*target, *(first[index]))) {
            *(first[index]) = *target;
            target = first[index];
            index = (index - 1) / 2;
        }
    }

    template< class RandomIt, class Compare >
    void _filter_down( RandomIt first, RandomIt last,
                     Compare comp ) {
        RandomIt target = last - 1;
        // 交换first与last-1位置的元素, 同时保存交换前的堆尾元素
        // 即交换堆顶与堆尾的元素
        typename RandomIt::value_type tmp;
        tmp = *target;
        *target = *first;
        *first = tmp;
        // 上滤节点，将其调整为最大堆
        ptrdiff_t len = jr_std::distance(first, target);
        ptrdiff_t parent = 0, child;
        while(parent * 2 + 1 < len) {
            // 在左右儿子里找最大的那个
            child = parent * 2 + 1;
            if((parent * 2 + 1 < len)
             && comp(*(first[child]), *(first[child + 1])))
                ++child;
            // 原堆尾元素<最大的儿子，用更大的元素覆盖原堆尾元素
            if(comp(tmp, first[child]))
                first[parent] = first[child];
            // 反之则说明堆已经调整好了
            else
                break;
            parent = child;
        }
        first[parent] = tmp;
    }

    template< class RandomIt, class Compare >
    void push_heap( RandomIt first, RandomIt last,
                    Compare comp ) {
        _filter_up(first, last, comp);
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
    }

    template< class RandomIt >
    void make_heap( RandomIt first, RandomIt last ) {
        make_heap(first, last,
                  [=](auto x, auto y)->bool { return x < y; });
    }

    template< class RandomIt, class Compare >
    void pop_heap( RandomIt first, RandomIt last, Compare comp ) {
        _filter_down(first, last, comp);
    }

    template< class RandomIt >
    void pop_heap( RandomIt first, RandomIt last ) {
        pop_heap(first, last,
                  [=](auto x, auto y)->bool { return x < y; });
    }

    template< class RandomIt, class Compare >
    void sort_heap( RandomIt first, RandomIt last, Compare comp ) {
        while(last - first > 1)
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
        ptrdiff_t len = jr_std::distance(first, last - 1);
        ptrdiff_t parent = 0, child;
        while(parent < len) {
            child = parent * 2 + 1;
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
        RandomIt it = first;
        while((it != last) && is_heap(first, it++, comp));
        return it;
    }

    template< class RandomIt >
    RandomIt is_heap_until( RandomIt first, RandomIt last ) {
        return is_heap_until(first, last,
                  [=](auto x, auto y)->bool { return x < y; });
    }
}

#endif // JR_ALGORITHM_H
