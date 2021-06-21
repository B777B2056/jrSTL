#ifndef JR_HEAP_H
#define JR_HEAP_H

#include "../../iterator/jr_iterator.h"

/*堆操作*/
namespace panzer {
    // 向上过滤
    template< class RandomIt, class Compare >
    void push_heap( RandomIt first, RandomIt last,
                    Compare comp ) {
        if(first == last - 1 || first == last)
            return;
        typedef typename panzer::iterator_traits<RandomIt>::value_type type;
        typedef typename panzer::iterator_traits<RandomIt>::difference_type dis_type;
        RandomIt target = last - 1;
        type target_var = *target;
        dis_type index = panzer::distance(first, target);
        dis_type parent = (index - 1) / 2;
        while(index && comp(*(first+parent), target_var)) {
            *(first+index) = *(first+parent);
            index = parent;
            parent = (index - 1) / 2;
        }
        *(first+index) = target_var;
    }

    template< class RandomIt >
    void push_heap( RandomIt first, RandomIt last ) {
        typedef typename iterator_traits<RandomIt>::value_type type;
        panzer::push_heap(first, last,
                          [](const type& x, const type& y)
                          ->bool { return x < y; });
    }

    template< class RandomIt, class Compare >
    void make_heap( RandomIt first, RandomIt last, Compare comp ) {
        RandomIt tmp = first;
        while(tmp != last)
            panzer::push_heap(first, ++tmp, comp);
    }

    template< class RandomIt >
    void make_heap( RandomIt first, RandomIt last ) {
        typedef typename panzer::iterator_traits<RandomIt>::value_type type;
        panzer::make_heap(first, last,
                          [](const type& x, const type& y)
                          ->bool { return x < y; });
    }

    // 向下过滤
    template< class RandomIt, class Compare >
    void pop_heap( RandomIt first, RandomIt last, Compare comp ) {
        if(first == last - 1)
            return;
        typedef typename panzer::iterator_traits<RandomIt>::value_type type;
        typedef typename panzer::iterator_traits<RandomIt>::difference_type dis_type;
        RandomIt target = last - 1;
        // 交换first与last-1位置的元素, 同时保存交换前的堆尾元素
        // 即交换堆顶与堆尾的元素
        type tmp = *target;
        *target = *first;
        *first = tmp;
        // 上滤节点，将其调整为最大堆
        dis_type len = panzer::distance(first, target);
        dis_type parent = 0, child = parent * 2 + 1;
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
        typedef typename panzer::iterator_traits<RandomIt>::value_type type;
        panzer::pop_heap(first, last,
                         [](const type& x, const type& y)
                         ->bool { return x < y; });
    }

    template< class RandomIt, class Compare >
    void sort_heap( RandomIt first, RandomIt last, Compare comp ) {
        while(first != last)
            panzer::pop_heap(first, last--, comp);
    }

    template< class RandomIt >
    void sort_heap( RandomIt first, RandomIt last ) {
        typedef typename panzer::iterator_traits<RandomIt>::value_type type;
        panzer::sort_heap(first, last,
                          [](const type& x, const type& y)
                          ->bool { return x < y; });
    }

    template< class RandomIt, class Compare >
    bool is_heap( RandomIt first, RandomIt last, Compare comp ) {
        if(first == last || first == last - 1)
            return true;
        typedef typename panzer::iterator_traits<RandomIt>::difference_type dis_type;
        dis_type len = panzer::distance(first, last - 1);
        dis_type parent = 0, child = parent * 2 + 1;
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
        typedef typename panzer::iterator_traits<RandomIt>::value_type type;
        return panzer::is_heap(first, last,
                               [](const type& x, const type& y)
                               ->bool { return x < y; });
    }

    template< class RandomIt, class Compare >
    RandomIt is_heap_until( RandomIt first, RandomIt last, Compare comp ) {
        while((first != last) && panzer::is_heap(first, last--, comp));
        return last;
    }

    template< class RandomIt >
    RandomIt is_heap_until( RandomIt first, RandomIt last ) {
        typedef typename panzer::iterator_traits<RandomIt>::value_type type;
        return panzer::is_heap_until(first, last,
                                     [](const type& x, const type& y)
                                     ->bool { return x < y; });
    }
}

#endif // JR_HEAP_H
