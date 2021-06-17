#ifndef JR_ALLOCATOR_H
#define JR_ALLOCATOR_H

#include <new>
#include <cstdlib>
#include <cstddef>
#include <climits>

namespace jr_std {
    template<class T>
    class allocator{
    public:
        typedef T value_type;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_reference;
        typedef size_t size_type;     // size_t
        typedef ptrdiff_t difference_type;  // ptrdiff_t

        /*default constructor*/
        allocator() noexcept {}
        /*copy constructor(sanme type)*/
        allocator(const allocator&) noexcept {}
        /*copy constructor(different type)*/
        template<class U>
        allocator(const allocator<U>&) noexcept {}
        /*destructor*/
        ~allocator() {}
        /*use current allocator's policy to build a different type allocator*/
        template<typename U>
        struct rebind{
            typedef allocator<U> other;
        };
        /* return a instance's memory address
         * @param: reference of instance x
         * @return: pointer of instance x's memory
         */
        pointer address(reference x) const noexcept{
            return static_cast<pointer>(&x);
        }

        const_pointer address(const_reference x) const noexcept{
            return static_cast<const_pointer>(&x);
        }
        /* alloca space to storage instance x
         * @param: n number of instance x
         *         hint
         * @return: pointer of instance x's memory
         */
        pointer allocate(size_type n, const void *hint = 0) const {
            pointer p = static_cast<pointer>(malloc(static_cast<size_type>(n * sizeof(T))));
            if(!p){
                throw "out of memory.";
            }
            return p;
        }
        /* free space which is storaging instance x
         * @param: p pointer of instance x's memory
         *         n number of instance x
         * @return: void
         */
        void deallocate(pointer p, size_type){
             free(p);
        }
        /* calculate the allocable capacity of this allocator
         * @param: void
         * @return: size_t, allocator's capacity
         */
        size_type max_size() const noexcept{
            return UINT_MAX / sizeof(T);
        }
        /* initialize the instance without allocate memory
         * @param: p the pointer of instance
         *         ...args	parament of p's constructor
         * @return: void
         * ATTENTION: "&&" 's goal is to get the Rvalue reference for Args  .
         */
        template<class U, class... Args>
        void construct(U *p, Args&&... args) const {
            new(static_cast<void *>(p)) U(static_cast<Args&&>(args)...);
        }
        /* free the instance's member's memory
         * @param: p the pointer of instance
         * @return: void
         */
        template<class U>
        void destroy(U *p){
            p->~U();
        }
    };

    /*void特化版*/
    template< >
    class allocator<void> {
        typedef void value_type;
        typedef void* pointer;
        typedef const void* const_pointer;

        template<typename U>
        struct rebind{
            typedef allocator<U> other;
        };
    };

    /* compare with two allocators(maybe they have different types)
     * @param: lhs allocator
     *         rhs allocator
     * @return: equal or not
     * ATTENTION: Cause the default allocator HAVE NO status, any two allocator are always equal.
     */
    template<class T1, class T2>
    bool operator==(const allocator<T1>&, const allocator<T2>&) noexcept{
        return true;
    }
    template<class T1, class T2>
    bool operator!=(const allocator<T1>&, const allocator<T2>&) noexcept{
        return false;
    }
}

#endif // JR_ALLOCATOR_H
