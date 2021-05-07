#ifndef JR_FUNCTIONAL_H
#define JR_FUNCTIONAL_H

namespace jr_std {
    template< class T >
    struct less {
        bool operator()( const T& lhs, const T& rhs ) const {
            return lhs < rhs;
        }
    };

    template< class T >
    struct greater {
        bool operator()( const T& lhs, const T& rhs ) const {
            return lhs > rhs;
        }
    };
}

#endif // JR_FUNCTIONAL_H
