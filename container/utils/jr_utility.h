#ifndef JR_UTILITY_H
#define JR_UTILITY_H

namespace jr_std {
template< class T1, class T2 >
struct pair {
    typedef T1 first_type;
    typedef T2 second_type;

    first_type first;
    second_type second;

    constexpr pair() {}

    pair( const T1& x, const T2& y ) : first(x), second(y)
    {}

    pair( T1&& x, T2&& y )
        : first(static_cast<T1&&>(x)), second(static_cast<T2&&>(y))
    {}

    pair( const pair& p ) : first(p.first), second(p.second)
    {}

    pair( pair&& p )
        : first(static_cast<T1&&>(p.first)),
        second(static_cast<T2&&>(p.second))
    {}

    pair& operator=( const pair& other ) = default;
    
    pair& operator=( pair&& other ) noexcept = default;

    void swap(pair& other) noexcept {
        pair tmp(first, second);
        *this = other;
        other = tmp;
    }

};

template< class T1, class T2 >
void swap( pair<T1,T2>& x, pair<T1,T2>& y ) noexcept { x.swap(y); }

template< class T1, class T2 >
bool operator==( const pair<T1,T2>& lhs, const pair<T1,T2>& rhs ) {
    return (lhs.first == rhs.first) && (lhs.second == rhs.second);
}

template< class T1, class T2 >
bool operator!=( const pair<T1,T2>& lhs, const pair<T1,T2>& rhs ) {
    return !(lhs == rhs);
}

template< class T1, class T2 >
bool operator<( const pair<T1,T2>& lhs, const pair<T1,T2>& rhs ) {
    if(lhs.first < rhs.first)
        return true;
    if(lhs.first > rhs.first)
        return false;
    return lhs.second < rhs.second;
}

template< class T1, class T2 >
bool operator>( const pair<T1,T2>& lhs, const pair<T1,T2>& rhs ) {
    if(lhs.first > rhs.first)
        return true;
    if(lhs.first < rhs.first)
        return false;
    return lhs.second > rhs.second;
}

template< class T1, class T2 >
bool operator<=( const pair<T1,T2>& lhs, const pair<T1,T2>& rhs ) {
    return !(lhs > rhs);
}

template< class T1, class T2 >
bool operator>=( const pair<T1,T2>& lhs, const pair<T1,T2>& rhs ) {
    return !(lhs < rhs);
}

template< class T1, class T2 >
pair<T1, T2> make_pair( T1&& t, T2&& u ) {
    return pair<T1, T2>(static_cast<T1&&>(t), static_cast<T2&&>(u));
}
}

#endif // JR_UTILITY_H
