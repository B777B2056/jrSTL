#ifndef JR_ITERATOR_H
#define JR_ITERATOR_H

/* C++11 Standard
 * Cause tags of iterator_category is different with STL, this iterator cannot adapt to STL
 */

#include <cstddef>

namespace jr_std {
    /*Tags of different type iterator_category, for function overload*/
    struct input_iterator_tag {};
    struct outut_iterator_tag {};
    struct forward_iterator_tag : public input_iterator_tag {};
    struct bidectional_iterator_tag : public forward_iterator_tag {};
    struct random_access_iterator_tag : public bidectional_iterator_tag {};

    /*A base class, convenient for other classes to inherit*/
    template<
        class Category,
        class T,
        class Distance = ptrdiff_t,
        class Pointer = T*,
        class Reference = T&
    > struct iterator {
        typedef Category iterator_category;
        typedef T value_type;
        typedef Distance difference_type;
        typedef Pointer pointer;
        typedef Reference reference;
    };

    /*Traits(General version)*/
    template< class T >
    struct iterator_traits {
        typedef typename T::iterator_category iterator_category;
        typedef typename T::value_type value_type;
        typedef typename T::difference_type difference_type;
        typedef typename T::pointer pointer;
        typedef typename T::reference reference;
    };

    /*Traits(Native pointer specialization version)*/
    template< class T>
    struct iterator_traits<T*> {
        typedef random_access_iterator_tag iterator_category;
        typedef T value_type;
        typedef ptrdiff_t difference_type;
        typedef T* pointer;
        typedef T& reference ;
    };

    /*Traits(Const native pointer specialization version)*/
    template< class T >
    struct iterator_traits<const T*> {
        typedef random_access_iterator_tag iterator_category;
        typedef T value_type;
        typedef ptrdiff_t difference_type;
        typedef const T* pointer;
        typedef const T& reference ;
    };

    /*Reverse Iterator Adapter*/
    template< class Iter >
    class reverse_iterator
            : public iterator< typename iterator_traits<Iter>::iterator_category,
                               typename iterator_traits<Iter>::value_type,
                               typename iterator_traits<Iter>::difference_type,
                               typename iterator_traits<Iter>::pointer,
                               typename iterator_traits<Iter>::reference > {
      public:
        typedef Iter iterator_type;

      private:
        Iter current;
        typedef iterator< typename iterator_traits<Iter>::iterator_category,
                          typename iterator_traits<Iter>::value_type,
                          typename iterator_traits<Iter>::difference_type,
                          typename iterator_traits<Iter>::pointer,
                          typename iterator_traits<Iter>::reference > _base;

      public:
        reverse_iterator() {}

        explicit reverse_iterator( iterator_type x ) : current(x) {}

//        template< class U >
//        reverse_iterator( const reverse_iterator<U>& other ) {}

//        template< class U >
//        reverse_iterator& operator=( const reverse_iterator<U>& other )

        iterator_type base() const { return current; }

        reverse_iterator& operator++() {
            --current;
            return *this;
        }

        reverse_iterator& operator--() {
            ++current;
            return *this;
        }

        reverse_iterator operator++( int ) {
            iterator_type tmp = current;
            --current;
            return tmp;
        }

        reverse_iterator operator--( int ) {
            iterator_type tmp = current;
            ++current;
            return tmp;
        }

        reverse_iterator operator+( typename _base::difference_type n ) const {
            return reverse_iterator(current - n);
        }

        reverse_iterator operator-( typename _base::difference_type n ) const {
            return reverse_iterator(current + n);
        }

        reverse_iterator& operator+=( typename _base::difference_type n ) {
            current -= n;
            return *this;
        }

        reverse_iterator& operator-=( typename _base::difference_type n ) {
            current -= n;
            return *this;
        }

        typename _base::reference operator*() const {
            iterator_type tmp = current;
            return *(--tmp);
        }

        typename _base::pointer operator->() const {
            return &(*(*this));
        }

        typename _base::value_type operator[]( typename _base::difference_type n ) const {
            return *(*this + n);
        }
    };

    template< class Iterator1, class Iterator2 >
    bool operator==( const reverse_iterator<Iterator1>& lhs,
                     const reverse_iterator<Iterator2>& rhs )
    { return lhs.base() == rhs.base(); }

    template< class Iterator1, class Iterator2 >
    bool operator!=( const reverse_iterator<Iterator1>& lhs,
                     const reverse_iterator<Iterator2>& rhs )
    { return lhs.base() != rhs.base(); }

    template< class Iterator1, class Iterator2 >
    bool operator<( const reverse_iterator<Iterator1>& lhs,
                    const reverse_iterator<Iterator2>& rhs )
    { return lhs.base() > rhs.base(); }

    template< class Iterator1, class Iterator2 >
    bool operator<=( const reverse_iterator<Iterator1>& lhs,
                     const reverse_iterator<Iterator2>& rhs )
    { return lhs.base() >= rhs.base(); }

    template< class Iterator1, class Iterator2 >
    bool operator>( const reverse_iterator<Iterator1>& lhs,
                    const reverse_iterator<Iterator2>& rhs )
    { return lhs.base() < rhs.base(); }

    template< class Iterator1, class Iterator2 >
    bool operator>=( const reverse_iterator<Iterator1>& lhs,
                     const reverse_iterator<Iterator2>& rhs )
    { return lhs.base() <= rhs.base(); }


    /*Operating of iterator*/
    template< class InputIt, class Distance >
    void _advance( InputIt& it, Distance n, input_iterator_tag ) {
        if(n) {
            while(n--)
                ++it;
        }
    }

    template< class InputIt, class Distance >
    void _advance( InputIt& it, Distance n, bidectional_iterator_tag ) {
        if(n) {
            while(n--)
                ++it;
        } else {
            while(n++)
                --it;
        }
    }

    template< class InputIt, class Distance >
    void _advance( InputIt& it, Distance n, random_access_iterator_tag ) {
        it += n;
    }

    template< class InputIt >
    typename iterator_traits<InputIt>::difference_type
    _distance( InputIt first, InputIt last, input_iterator_tag ) {
        typename iterator_traits<InputIt>::difference_type n = 0;
        InputIt tmp = first;
        while(tmp != last) {
            ++tmp;
            ++n;
        }
        return n;
    }

    template< class InputIt >
    typename iterator_traits<InputIt>::difference_type
    _distance( InputIt first, InputIt last, random_access_iterator_tag ) {
        return last - first;
    }

    template< class InputIt, class Distance >
    void advance( InputIt& it, Distance n ) {
        _advance(it, n, typename iterator_traits<InputIt>::iterator_category());
    }

    template< class InputIt >
    typename iterator_traits<InputIt>::difference_type
    distance( InputIt first, InputIt last ) {
        return _distance(first, last, typename iterator_traits<InputIt>::iterator_category());
    }

    template< class InputIt >
    InputIt
    next( InputIt it, typename iterator_traits<InputIt>::difference_type n = 1 ) {
        advance(it, n);
        return it;
    }

    template< class BidirIt >
    BidirIt prev( BidirIt it, typename iterator_traits<BidirIt>::difference_type n = 1 ) {
        advance(it, -n);
        return it;
    }
}

#endif // JR_ITERATOR_H
