#ifndef JR_ITERATOR_H
#define JR_ITERATOR_H

#include <cstddef>
#include <utility>

namespace panzer {
    /*Tags of different type iterator_category, for function overload*/
    struct input_iterator_tag {};
    struct output_iterator_tag {};
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
            : public iterator< typename panzer::iterator_traits<Iter>::iterator_category,
                               typename iterator_traits<Iter>::value_type,
                               typename iterator_traits<Iter>::difference_type,
                               typename iterator_traits<Iter>::pointer,
                               typename iterator_traits<Iter>::reference > {
      public:
        typedef Iter iterator_type;

      private:
        Iter current;
        typedef iterator< typename panzer::iterator_traits<Iter>::iterator_category,
                          typename iterator_traits<Iter>::value_type,
                          typename iterator_traits<Iter>::difference_type,
                          typename iterator_traits<Iter>::pointer,
                          typename iterator_traits<Iter>::reference > _base;

      public:
        reverse_iterator()  : current() {}

        explicit reverse_iterator( iterator_type x ) : current(x) {}

        template< class U >
        reverse_iterator( const reverse_iterator<U>& other ) : current(other.current) {}

        iterator_type base() const { return current; }

        template< class U >
        reverse_iterator& operator=( const reverse_iterator<U>& other ) {
            current = other.base();
            return *this;
        }

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
            current += n;
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

    template< class Iter >
    reverse_iterator<Iter> operator+( typename reverse_iterator<Iter>::difference_type n,
                                      const reverse_iterator<Iter>& it ) {
        return reverse_iterator<Iter>(it.base() - n);
    }

    template< class Iterator1, class Iterator2 >
    auto operator-( const reverse_iterator<Iterator1>& lhs,
                    const reverse_iterator<Iterator2>& rhs
                    ) -> decltype(rhs.base() - lhs.base()) {
        return rhs.base() - lhs.base();
    }

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

    /*Move Iterator Adapter*/
    template< class Iter >
    class move_iterator
            : public iterator< typename iterator_traits<Iter>::iterator_category,
                               typename iterator_traits<Iter>::value_type,
                               typename iterator_traits<Iter>::difference_type,
                               typename iterator_traits<Iter>::pointer,
                               typename iterator_traits<Iter>::value_type&& > {
    public:
      typedef Iter iterator_type;

    private:
      Iter current;
      typedef iterator< typename iterator_traits<Iter>::iterator_category,
                        typename iterator_traits<Iter>::value_type,
                        typename iterator_traits<Iter>::difference_type,
                        typename iterator_traits<Iter>::pointer,
                        typename iterator_traits<Iter>::value_type&& > _base;

    public:
      move_iterator() : current() {}

      explicit move_iterator( iterator_type x ) : current(x) {}

      template< class U >
      move_iterator( const move_iterator<U>& other ) : current(other.current) {}

      iterator_type base() const { return current; }

      template< class U >
      move_iterator& operator=( const move_iterator<U>& other ) {
          current = other.base();
          return *this;
      }

      move_iterator& operator++() {
          ++current;
          return *this;
      }

      move_iterator& operator--() {
          --current;
          return *this;
      }

      move_iterator operator++( int ) {
          iterator_type tmp = current;
          ++current;
          return tmp;
      }

      move_iterator operator--( int ) {
          iterator_type tmp = current;
          --current;
          return tmp;
      }

      move_iterator operator+( typename _base::difference_type n ) const {
          return move_iterator(current + n);
      }

      move_iterator operator-( typename _base::difference_type n ) const {
          return move_iterator(current - n);
      }

      move_iterator& operator+=( typename _base::difference_type n ) {
          current += n;
          return *this;
      }

      move_iterator& operator-=( typename _base::difference_type n ) {
          current -= n;
          return *this;
      }

      // 均返回右值引用
      typename _base::reference operator*() const {
          return static_cast<typename _base::reference>(*current);
      }

      typename _base::pointer operator->() const {
          return &(*(*this));
      }

      typename _base::value_type operator[]( typename _base::difference_type n ) const {
          return *(*this + n);
      }
    };

    template< class Iter >
    move_iterator<Iter> operator+( typename move_iterator<Iter>::difference_type n,
                   const move_iterator<Iter>& it ) {
        return move_iterator<Iter>(it.base() + n);
    }

    template< class Iterator1, class Iterator2 >
    auto operator-( const move_iterator<Iterator1>& lhs,
                    const move_iterator<Iterator2>& rhs
                    ) -> decltype(lhs.base() - rhs.base()) {
        return lhs.base() - rhs.base();
    }

    template< class Iterator1, class Iterator2 >
    bool operator==( const move_iterator<Iterator1>& lhs,
                     const move_iterator<Iterator2>& rhs )
    { return lhs.base() == rhs.base(); }

    template< class Iterator1, class Iterator2 >
    bool operator!=( const move_iterator<Iterator1>& lhs,
                     const move_iterator<Iterator2>& rhs )
    { return lhs.base() != rhs.base(); }

    template< class Iterator1, class Iterator2 >
    bool operator<( const move_iterator<Iterator1>& lhs,
                    const move_iterator<Iterator2>& rhs )
    { return lhs.base() < rhs.base(); }

    template< class Iterator1, class Iterator2 >
    bool operator<=( const move_iterator<Iterator1>& lhs,
                     const move_iterator<Iterator2>& rhs )
    { return lhs.base() <= rhs.base(); }

    template< class Iterator1, class Iterator2 >
    bool operator>( const move_iterator<Iterator1>& lhs,
                    const move_iterator<Iterator2>& rhs )
    { return lhs.base() > rhs.base(); }

    template< class Iterator1, class Iterator2 >
    bool operator>=( const move_iterator<Iterator1>& lhs,
                     const move_iterator<Iterator2>& rhs )
    { return lhs.base() >= rhs.base(); }

    /*Insert Iterator Adapter*/
    template< class Container >
    class insert_iterator
            : public iterator< output_iterator_tag, void,void,void,void > {
        public:
            typedef Container container_type;

        protected:
            container_type *cont;
            typename Container::iterator iter;

        public:
            insert_iterator( Container& c, typename Container::iterator i ) : cont(&c), iter(i) {}

            // 因为通常insert都是将value插在iter前面，且返回插入后的value所在迭代器位置，因此更新iter后还需要将其前进一格以回到原来的位置
            insert_iterator<Container>& operator=( const typename Container::value_type& value ) {
                iter = cont->insert(iter, value);
                ++iter;
                return *this;
            }

            insert_iterator<Container>& operator=( typename Container::value_type&& value ) {
                iter = cont->insert(iter, static_cast<typename Container::value_type&&>(value));
                ++iter;
                return *this;
            }

            // 以下三种操作对于Insert Iterator来说毫无意义，但为满足output_iterator操作要求又必须提供，因此进行空操作
            insert_iterator& operator*() { return *this; }

            insert_iterator& operator++() { return *this; }

            insert_iterator& operator++( int ) { return *this; }
    };

    template< class Container >
    insert_iterator<Container> inserter( Container& c, typename Container::iterator i )
    { return insert_iterator<Container>(c, i); }

    /*Front Insert Iterator Adapter*/
    template< class Container >
    class front_insert_iterator
            : public iterator< output_iterator_tag, void,void,void,void > {
        public:
            typedef Container container_type;

        protected:
            container_type *cont;

        public:
            front_insert_iterator( Container& c ) : cont(&c) {}

            front_insert_iterator<Container>& operator=( const typename Container::value_type& value ) {
                cont->push_front(value);
                return *this;
            }

            front_insert_iterator<Container>& operator=( typename Container::value_type&& value ) {
                cont->push_front(static_cast<typename Container::value_type&&>(value));
                return *this;
            }

            // 以下三种操作对于Front Insert Iterator来说毫无意义，但为满足output_iterator操作要求又必须提供，因此进行空操作
            front_insert_iterator& operator*() { return *this; }

            front_insert_iterator& operator++() { return *this; }

            front_insert_iterator& operator++( int ) { return *this; }
    };

    template< class Container >
    front_insert_iterator<Container> front_inserter( Container& c )
    { return front_insert_iterator<Container>(c); }

    /*Back Insert Iterator Adapter*/
    template< class Container >
    class back_insert_iterator
            : public iterator< output_iterator_tag, void,void,void,void > {
        public:
            typedef Container container_type;

        protected:
            container_type *cont;

        public:
            back_insert_iterator( Container& c ) : cont(&c) {}

            back_insert_iterator<Container>& operator=( const typename Container::value_type& value ) {
                cont->push_back(value);
                return *this;
            }

            back_insert_iterator<Container>& operator=( typename Container::value_type&& value ) {
                cont->push_back(std::move(value));
                return *this;
            }

            // 以下三种操作对于Back Insert Iterator来说毫无意义，但为满足output_iterator操作要求又必须提供，因此进行空操作
            back_insert_iterator& operator*() { return *this; }

            back_insert_iterator& operator++() { return *this; }

            back_insert_iterator& operator++( int ) { return *this; }
    };

    template< class Container >
    back_insert_iterator<Container> back_inserter( Container& c )
    { return back_insert_iterator<Container>(c); }

    /*Stream Iterator*/
    template< class T,
              class CharT = char,
              class Traits = std::char_traits<CharT>,
              class Distance = ptrdiff_t >
    class istream_iterator
            : public iterator<input_iterator_tag, T, Distance, const T*, const T&> {
        template< class T1, class T2, class T3, class T4 >
        friend bool operator==( const istream_iterator<T1,T2,T3,T4>& lhs,
                                const istream_iterator<T1,T2,T3,T4>& rhs );

    public:
        typedef CharT char_type;
        typedef Traits traits_type;
        typedef std::basic_istream<CharT, Traits> istream_type;

    protected:
        istream_type *is;
        T value;
        bool end_flag;

    public:
        // m默认构造函数用于创建EOF标识的流迭代器，即标志流的结束
        constexpr istream_iterator() : is(&(std::cin)), end_flag(true) {}

        istream_iterator( istream_type& stream ) : is(&stream), end_flag(false) {}

        istream_iterator( const istream_iterator& other ) = default;

        const T& operator*() const { return value; }

        const T* operator->() const { return &(*(*this)); }

        istream_iterator& operator++() {
            // 输入数据前查询流状态
            // （流对象一般都重载过运算符，当未到达流结束位置时，流对象==true，否则为false）
            end_flag = (*is) ? false : true;
            // 输入单个数据
            if(!end_flag)
                *is >> value;
            // 输入数据后更新流状态
            end_flag = (*is) ? false : true;
            return *this;
        }

        istream_iterator operator++(int) {
            istream_iterator tmp(*this);
            ++(*this);
            return tmp;
        }
    };

    template< class T, class CharT, class Traits, class Dist >
    bool operator==( const istream_iterator<T,CharT,Traits,Dist>& lhs,
                     const istream_iterator<T,CharT,Traits,Dist>& rhs )
    { return (lhs.is == rhs.is) && (lhs.value == rhs.value); }

    template< class T, class CharT, class Traits, class Dist >
    bool operator!=( const istream_iterator<T,CharT,Traits,Dist>& lhs,
                     const istream_iterator<T,CharT,Traits,Dist>& rhs )
    { return !(lhs == rhs); }

    template< class T,
              class CharT = char,
              class Traits = std::char_traits<CharT> >
    class ostream_iterator
            : public iterator<output_iterator_tag, void, void, void, void> {
    public:
        typedef CharT char_type;
        typedef Traits traits_type;
        typedef std::basic_ostream<CharT, Traits> ostream_type;

    protected:
        ostream_type *os;
        const char *str;

    public:
        ostream_iterator(ostream_type& stream) : os(&stream), str(nullptr) {}

        ostream_iterator(ostream_type& stream, const CharT* delim) : os(&stream), str(delim) {}

        ostream_iterator& operator=( const T& value ) {
            *os << value;
            if(str)
                *os << str;
            return *this;
        }

        ostream_iterator& operator*() { return *this; }

        ostream_iterator& operator++() { return *this; }

        ostream_iterator& operator++( int ) { return *this; }
    };

    /*Operating of iterator*/
    template< class InputIt, class Distance >
    void _advance( InputIt& it, Distance n, input_iterator_tag ) {
        if(n > 0) {
            while(n--)
                ++it;
        }
    }

    template< class InputIt, class Distance >
    void _advance( InputIt& it, Distance n, bidectional_iterator_tag ) {
        if(n > 0) {
            while(n--)
                ++it;
        } else if(n < 0) {
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

    /*头尾迭代器(有原生数组的特化版本)*/
    template< class C >
    auto begin( C& c ) -> decltype(c.begin()) { return c.begin(); }

    template< class C >
    auto begin( const C& c ) -> decltype(c.begin()) { return c.begin(); }

    template< class T, std::size_t N >
    T* begin( T (&array)[N] ) { return array; }  // 返回指向 array 起始的指针(此处array为C/C++原生array)

    template< class C >
    auto end( C& c ) -> decltype(c.end()) { return c.end(); }

    template< class C >
    auto end( const C& c ) -> decltype(c.end()) { return c.end(); }

    template< class T, std::size_t N >
    T* end( T (&array)[N] ) { return array + N; }
}

#endif // JR_ITERATOR_H
