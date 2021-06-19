#ifndef JR_SMART_PTR_H
#define JR_SMART_PTR_H

#include <cstddef>
#include <utility>
#include <fstream>
#include <type_traits>
#include "jr_allocator.h"

namespace jr_std {
    template< class T >
    struct default_delete {
        constexpr default_delete() noexcept = default;

        template<class U>
        default_delete( const default_delete<U>& ) noexcept {}

        void operator()(T* ptr) const {
            delete ptr;
            ptr = nullptr;
        }
    };

    template< class T >
    struct default_delete<T[]> {
        constexpr default_delete() noexcept = default;

        template<class U>
        default_delete( const default_delete<U[]>& ) noexcept {}

        template <class U>
        void operator()(U* ptr) const {
            delete []static_cast<T*>(ptr);
            ptr = nullptr;
        }
    };

    /*==============unique ptr==============*/

    template< class T,
              class Deleter = jr_std::default_delete<T> >
    class unique_ptr {
    public:
        typedef T* pointer;
        typedef T element_type;
        typedef Deleter deleter_type;

    private:
        pointer _source;
        Deleter _delete_fun;

        template< class U, class E >
        void _ctor(unique_ptr<U, E>&& u, std::true_type) {
            _source = static_cast<pointer>(u.release());
            _delete_fun = u.get_deleter();
        }

        template< class U, class E >
        void _ctor(unique_ptr<U, E>&& u, std::false_type) {
            _source = static_cast<pointer>(u.release());
            _delete_fun = std::move(u.get_deleter());
        }

    public:
        constexpr unique_ptr() noexcept
            : _source(nullptr), _delete_fun(Deleter())
        {}

        constexpr unique_ptr( std::nullptr_t) noexcept
            : _source(nullptr), _delete_fun(Deleter())
        {}

        explicit unique_ptr( pointer p ) noexcept
            : _source(p), _delete_fun(Deleter())
        {}

        unique_ptr( pointer p, const deleter_type& d1 ) noexcept
            : _source(p), _delete_fun(d1)
        {}

        unique_ptr( pointer p, deleter_type&& d2 ) noexcept
            : _source(p), _delete_fun(std::move(d2))
        {}

        unique_ptr( const unique_ptr& u ) noexcept = delete;  // 不可拷贝

        unique_ptr( unique_ptr&& u ) noexcept
            : _source(u.get()),
              _delete_fun(std::move(u.get_deleter())) {
            u._source = nullptr;
        }

        template< class U, class E >
        unique_ptr( unique_ptr<U, E>&& u ) noexcept {
            _ctor(std::forward<unique_ptr<U, E>&&>(u),
                  std::is_reference<E>());
        }

        ~unique_ptr() {
            if(_source)
                _delete_fun(_source);
        }

        unique_ptr& operator=(const unique_ptr&) = delete;

        unique_ptr& operator=( unique_ptr&& r ) noexcept {
            if(this != &r) {
                _source = r._source;
                _delete_fun = std::move(r._delete_fun);
                r._source = nullptr;
            }
            return *this;
        }

        template< class U, class E >
        unique_ptr& operator=( unique_ptr<U,E>&& u ) noexcept {
            if(this != &u) {
                _ctor(std::forward<unique_ptr<U, E>&&>(u),
                      std::is_reference<E>());
            }
            return *this;
        }

        unique_ptr& operator=( std::nullptr_t ) noexcept {
            _source = nullptr;
            return *this;
        }

        pointer get() const noexcept {
            return _source;
        }

        Deleter& get_deleter() noexcept {
            return _delete_fun;
        }

        const Deleter& get_deleter() const noexcept {
            return _delete_fun;
        }

        explicit operator bool() const noexcept {
            return _source != nullptr;
        }

        pointer release() noexcept {
            pointer tmp = _source;
            _source = nullptr;
            return tmp;
        }

        void reset( pointer ptr = pointer() ) noexcept {
            pointer tmp = _source;
            _source = ptr;
            if(tmp)
                _delete_fun(tmp);
        }

        void swap(unique_ptr& other) noexcept {
            {
                pointer tmp = _source;
                _source = other._source;
                other._source = tmp;
            }
            {
                Deleter t = _delete_fun;
                _delete_fun = other._delete_fun;
                other._delete_fun = t;
            }
        }

        typename std::add_lvalue_reference<T>::type operator*() const {
            return *_source;
        }

        pointer operator->() const noexcept {
            return &operator*();
        }
    };

    template < class T, class Deleter >
    class unique_ptr<T[], Deleter> {
    public:
        typedef T* pointer;
        typedef T element_type;
        typedef Deleter deleter_type;

    private:
        pointer _source;
        Deleter _delete_fun;

        template< class U, class E >
        void _ctor(unique_ptr<U, E>&& u, std::true_type) {
            _source = static_cast<pointer>(u.release());
            _delete_fun = u.get_deleter();
        }

        template< class U, class E >
        void _ctor(unique_ptr<U, E>&& u, std::false_type) {
            _source = static_cast<pointer>(u.release());
            _delete_fun = std::move(u.get_deleter());
        }

    public:
        constexpr  unique_ptr() noexcept
            : _source(nullptr), _delete_fun(Deleter())
        {}

        constexpr unique_ptr( std::nullptr_t ) noexcept
            : _source(nullptr), _delete_fun(Deleter())
        {}

        template<class U>
        explicit unique_ptr( U *p ) noexcept
            : _source(static_cast<pointer>(p)),
              _delete_fun(Deleter())
        {}

        unique_ptr( pointer p, const deleter_type& d1 ) noexcept
            : _source(p), _delete_fun(d1)
        {}

        unique_ptr( pointer p, deleter_type&& d2 ) noexcept
            : _source(p), _delete_fun(std::move(d2))
        {}

        unique_ptr( const unique_ptr& u ) noexcept = delete;  // 不可拷贝

        unique_ptr( unique_ptr&& u ) noexcept
            : _source(u._source),
              _delete_fun(std::move(u._delete_fun)) {
            u._source = nullptr;
        }

        template< class U, class E >
        unique_ptr( unique_ptr<U, E>&& u ) noexcept {
            _ctor(std::forward<unique_ptr<U, E>&&>(u),
                  std::is_reference<E>());
        }

        ~unique_ptr() {
            if(_source)
                _delete_fun(_source);
        }

        unique_ptr& operator=(const unique_ptr&) = delete;

        unique_ptr& operator=( unique_ptr&& r ) noexcept {
            if(this != &r) {
                _source = r._source;
                _delete_fun = std::move(r._delete_fun);
                r._source = nullptr;
            }
            return  *this;
        }

        template< class U, class E >
        unique_ptr& operator=( unique_ptr<U,E>&& u ) noexcept {
            if(this != &u) {
                _ctor(std::forward<unique_ptr<U, E>&&>(u),
                      std::is_reference<E>());
            }
            return *this;
        }

        unique_ptr& operator=( std::nullptr_t ) noexcept {
            _source = nullptr;
            return *this;
        }

        pointer get() const noexcept {
            return _source;
        }

        Deleter& get_deleter() noexcept {
            return _delete_fun;
        }

        const Deleter& get_deleter() const noexcept {
            return _delete_fun;
        }

        explicit operator bool() const noexcept {
            return _source != nullptr;
        }

        pointer release() noexcept {
            pointer tmp = _source;
            _source = nullptr;
            return tmp;
        }

        template< class U >
        void reset( U *ptr ) noexcept {
            pointer tmp = _source;
            _source = static_cast<pointer>(ptr);
            if(tmp)
                _delete_fun(tmp);
        }

        void reset( std::nullptr_t p = nullptr ) noexcept {
            reset(p);
        }

        void swap(unique_ptr& other) noexcept {
            {
                pointer tmp = _source;
                _source = other._source;
                other._source = tmp;
            }
            {
                Deleter t = _delete_fun;
                _delete_fun = other._delete_fun;
                other._delete_fun = t;
            }
        }

        T& operator[]( std::size_t i ) const {
            return *(_source + i);
        }
    };

    template< class T, class D >
    void swap( jr_std::unique_ptr<T, D>& lhs,
               jr_std::unique_ptr<T, D>& rhs ) noexcept {
        lhs.swap(rhs);
    }

    template<class T1, class D1, class T2, class D2>
    bool operator==(const jr_std::unique_ptr<T1, D1>& x,
                    const jr_std::unique_ptr<T2, D2>& y) {
        return x.get() == y.get();
    }

    template<class T1, class D1, class T2, class D2>
    bool operator!=(const jr_std::unique_ptr<T1, D1>& x,
                    const jr_std::unique_ptr<T2, D2>& y) {
        return !(x == y);
    }

    template<class T1, class D1, class T2, class D2>
    bool operator<(const jr_std::unique_ptr<T1, D1>& x,
                   const jr_std::unique_ptr<T2, D2>& y) {
        return *x < *y;
    }

    template<class T1, class D1, class T2, class D2>
    bool operator<=(const jr_std::unique_ptr<T1, D1>& x,
                    const jr_std::unique_ptr<T2, D2>& y) {
        return !(y < x);
    }

    template<class T1, class D1, class T2, class D2>
    bool operator>(const jr_std::unique_ptr<T1, D1>& x,
                   const jr_std::unique_ptr<T2, D2>& y) {
        return y < x;
    }

    template<class T1, class D1, class T2, class D2>
    bool operator>=(const jr_std::unique_ptr<T1, D1>& x,
                    const jr_std::unique_ptr<T2, D2>& y) {
        return !(x < y);
    }

    template <class T, class D>
    bool operator==(const jr_std::unique_ptr<T, D>& x,
                    std::nullptr_t) noexcept {
        return !x;
    }

    template <class T, class D>
    bool operator==(std::nullptr_t,
                    const jr_std::unique_ptr<T, D>& x) noexcept {
        return !x;
    }

    template <class T, class D>
    bool operator!=(const jr_std::unique_ptr<T, D>& x,
                    std::nullptr_t) noexcept {
        return static_cast<bool>(x);
    }

    template <class T, class D>
    bool operator!=(std::nullptr_t,
                    const jr_std::unique_ptr<T, D>& x) noexcept {
        return static_cast<bool>(x);
    }

    template <class T, class D>
    bool operator<(const jr_std::unique_ptr<T, D>& x,
                   std::nullptr_t) {
        return x.get() < nullptr;
    }

    template <class T, class D>
    bool operator<(std::nullptr_t,
                   const jr_std::unique_ptr<T, D>& y) {
        return nullptr < y.get();
    }

    template <class T, class D>
    bool operator<=(const jr_std::unique_ptr<T, D>& x,
                    std::nullptr_t) {
        return !(nullptr < x);
    }

    template <class T, class D>
    bool operator<=(std::nullptr_t,
                    const jr_std::unique_ptr<T, D>& y) {
        return !(y < nullptr);
    }

    template <class T, class D>
    bool operator>(const jr_std::unique_ptr<T, D>& x,
                   std::nullptr_t) {
        return nullptr < x;
    }

    template <class T, class D>
    bool operator>(std::nullptr_t,
                   const jr_std::unique_ptr<T, D>& y) {
        return y < nullptr;
    }

    template <class T, class D>
    bool operator>=(const jr_std::unique_ptr<T, D>& x,
                    std::nullptr_t) {
        return !(x < nullptr);
    }

    template <class T, class D>
    bool operator>=(std::nullptr_t,
                    const jr_std::unique_ptr<T, D>& y) {
        return !(nullptr < y);
    }

    /*==============shared ptr==============*/
    template< class T >
    class weak_ptr;

    // 类型擦除（采用多态实现的类型擦除）实现不依赖模板参数存储删除器与分配器
    struct _shared_deleter_base {
        virtual void delete_fun(void*) = 0;

        virtual ~_shared_deleter_base() {}
    };

    template< class Y, class Deleter >
    struct _shared_deleter
            : public _shared_deleter_base {
        Deleter _d;

        _shared_deleter(Deleter d) : _d(d) {}

        virtual void delete_fun(void* ptr) {
            _d(static_cast<Y*>(ptr));
        }
    };

    // shared_ptr计数器
    struct _shared_count {
        int _strong_count, _weak_count;

        _shared_count()
            : _strong_count(0),
              _weak_count(0)
        {}

        // 获取强引用计数
        int use_count() const {
            return _strong_count;
        }

        // 获取弱引用计数
        int weak_count() const {
            return _weak_count;
        }

        // 改变引用计数
        void add_strong_cnt() {
            ++_strong_count;
        }

        void add_weak_cnt() {
            ++_weak_count;
        }

        void decrease_strong_cnt() {
            --_strong_count;
        }

        void decrease_weak_cnt() {
            --_weak_count;
        }
    };

    // shared_ptr控制块(存放引用计数，删除器与分配器)
    struct _control_block_base {

        virtual ~_control_block_base() {}

        // 获取强引用计数
        virtual int use_count() const = 0;

        // 获取弱引用计数
        virtual int weak_count() const = 0;

        // 改变引用计数
        virtual void add_strong_cnt() = 0;

        virtual void add_weak_cnt() = 0;

        virtual void decrease_strong_cnt() = 0;

        virtual void decrease_weak_cnt() = 0;

        // 删除器操作
        virtual void delete_fun(void *ptr) = 0;
    };

    template< class T >
    struct _control_block
            : public _control_block_base {

            _shared_count *_cnt;
            _shared_deleter_base *_deleter;

            _control_block()
                : _cnt(new _shared_count()),
                  _deleter(new _shared_deleter<T, jr_std::default_delete<T> >(jr_std::default_delete<T>()))
            {}

            _control_block(_shared_count *cnt)
                : _cnt(cnt),
                  _deleter(new _shared_deleter<T, jr_std::default_delete<T> >(jr_std::default_delete<T>()))
            {}

            template< class Deleter>
            _control_block(Deleter d, int)
                : _cnt(new _shared_count()),
                  _deleter(new _shared_deleter< T, Deleter >(d))
            {}

            template< class Deleter, class Alloc >
            _control_block(Deleter d, Alloc)
                : _cnt(new _shared_count()),
                  _deleter(new _shared_deleter< T, Deleter >(d))
            {}

            ~_control_block() {
                if(_cnt)
                    delete _cnt;
                if(_deleter)
                    delete _deleter;
            }

            // 获取强引用计数
            virtual int use_count() const {
                return _cnt->use_count();
            }

            // 获取弱引用计数
            virtual int weak_count() const {
                return _cnt->weak_count();
            }

            // 改变引用计数
            virtual void add_strong_cnt() {
                _cnt->add_strong_cnt();
            }

            virtual void add_weak_cnt() {
                _cnt->add_weak_cnt();
            }

            virtual void decrease_strong_cnt() {
                _cnt->decrease_strong_cnt();
            }

            virtual void decrease_weak_cnt() {
                _cnt->decrease_weak_cnt();
            }

            // 删除器操作
            virtual void delete_fun(void *ptr) {
                _deleter->delete_fun(ptr);
            }
    };

    // shaered_ptr本体
    template< class T >
    class shared_ptr {

        template< class U1, class U2 >
        friend U1* get_deleter( const shared_ptr<U2>& p ) noexcept;

        template< class Y >
        friend class shared_ptr;

        template< class U >
        friend class weak_ptr;

        public:
            typedef T element_type;

        private:
            element_type *_ptr;
            _control_block_base *_cb;

            void _delete_object() {
                if(!_ptr)
                    return;
                _cb->delete_fun(_ptr);
                _ptr = nullptr;
            }

            void _delete_control_block() {
                delete _cb;
                _cb = nullptr;
            }

            void _drop_strong_count() {
                if(!_cb)
                    return;
                _cb->decrease_strong_cnt();
                if(!use_count()) {
                    _delete_object();
                    if(!_cb->weak_count())
                        _delete_control_block();
                }
            }

        public:
            constexpr shared_ptr() noexcept
                : _ptr(nullptr),
                  _cb(nullptr)
            {}

            constexpr shared_ptr( std::nullptr_t ptr ) noexcept
                : _ptr(ptr),
                  _cb(nullptr)
            {}

            template< class Y >
            explicit shared_ptr( Y* ptr )
                : _ptr(static_cast<element_type*>(ptr)),
                  _cb(new _control_block<element_type>()) {
                _cb->add_strong_cnt();
            }

            template< class Y, class Deleter >
            shared_ptr( Y *ptr, Deleter d )
                : _ptr(static_cast<element_type*>(ptr)),
                  _cb(new _control_block<element_type>(d, 0)) {
                _cb->add_strong_cnt();
            }

            template< class Deleter >
            shared_ptr( std::nullptr_t ptr, Deleter d )
                : _ptr(ptr),
                  _cb(new _control_block<T>(d, 0))
            {}

            template< class Y, class Deleter, class Alloc >
            shared_ptr( Y *ptr, Deleter d, Alloc alloc )
                : _ptr(static_cast<element_type*>(ptr)),
                  _cb(new _control_block<element_type>(d, alloc)) {
                _cb->add_strong_cnt();
            }

            template< class Deleter, class Alloc >
            shared_ptr( std::nullptr_t ptr,
                        Deleter d,
                        Alloc alloc )
                : _ptr(ptr),
                  _cb(new _control_block<element_type>(d, alloc))
            {}

            template< class Y >
            shared_ptr( const shared_ptr<Y>& r,
                        element_type* ptr ) noexcept
                : _ptr(ptr),
                  _cb(r._cb) {
                _cb->add_strong_cnt();
            }

            shared_ptr( const shared_ptr& r ) noexcept
                : _ptr(r.get()),
                  _cb(r._cb) {
                _cb->add_strong_cnt();
            }

            template< class Y >
            shared_ptr( const shared_ptr<Y>& r ) noexcept
                : _ptr(static_cast<element_type*>(r.get())),
                  _cb(r._cb) {
                _cb->add_strong_cnt();
            }

            shared_ptr( shared_ptr&& r ) noexcept
                : _ptr(r.get()),
                  _cb(r._cb) {
                r._ptr = nullptr;
                r._cb = new _control_block<element_type>();
            }

            template< class Y >
            shared_ptr( shared_ptr<Y>&& r ) noexcept
                : _ptr(static_cast<element_type*>(r.get())),
                  _cb(r._cb) {
                r._ptr = nullptr;
                r._cb = new _control_block<Y>();
            }

            template< class Y >
            explicit shared_ptr( const jr_std::weak_ptr<Y>& r )
                : _ptr(static_cast<element_type*>(r._ptr)),
                  _cb(r._cb) {
                if(_cb)
                    _cb->add_strong_cnt();
            }

            template< class Y, class Deleter >
            shared_ptr( jr_std::unique_ptr<Y, Deleter>&& r )
                : _ptr(static_cast<element_type*>(r.release())),
                  _cb(new _control_block<element_type>(r.get_deleter(), 0)) {
                _cb->add_strong_cnt();
            }

            ~shared_ptr() {
                _drop_strong_count();
            }

            shared_ptr& operator=( const shared_ptr& r ) noexcept {
                if(this != &r) {
                    _ptr = r.get();
                    _drop_strong_count();
                    _cb = r._cb;
                    _cb->add_strong_cnt();
                }
                return *this;
            }

            template< class Y >
            shared_ptr& operator=( const shared_ptr<Y>& r ) noexcept {
                if(std::is_same<T, Y>::value) {
                    _ptr = static_cast<element_type*>(r.get());
                    _drop_strong_count();
                    _cb = r._cb;
                    _cb->add_strong_cnt();
                }
                return *this;
            }

            shared_ptr& operator=( shared_ptr&& r ) noexcept {
                if(this != &r) {
                    _ptr = r.get();
                    _drop_strong_count();
                    _cb = r._cb;
                    r._ptr = nullptr;
                    r._cb = new _control_block<element_type>();
                }
                return *this;
            }

            template< class Y >
            shared_ptr& operator=( shared_ptr<Y>&& r ) noexcept {
                if(std::is_same<T, Y>::value) {
                    _ptr = static_cast<element_type*>(r.get());
                    _drop_strong_count();
                    _cb = r._cb;
                    r._ptr = nullptr;
                    r._cb = new _control_block<Y>();
                }
                return *this;
            }

            template< class Y, class Deleter >
            shared_ptr& operator=( jr_std::unique_ptr<Y, Deleter>&& r ) {
                if(std::is_same<T, Y>::value) {
                    _ptr = static_cast<element_type*>(r.release());
                    _drop_strong_count();
                    _cb = new _control_block<Y>(r.get_deleter());
                    _cb->add_strong_cnt();
                }
                return *this;
            }

            void reset() noexcept {
                _drop_strong_count();
            }

            template< class Y >
            void reset( Y* ptr ) {
                _drop_strong_count();
                _ptr = static_cast<element_type*>(ptr);
                _cb = new _control_block<Y>();
                _cb->add_strong_cnt();
            }

            template< class Y, class Deleter >
            void reset( Y* ptr, Deleter d ) {
                _drop_strong_count();
                _ptr = static_cast<element_type*>(ptr);
                _cb = new _control_block<Y>(d,0);
                _cb->add_strong_cnt();
            }

            template< class Y, class Deleter, class Alloc >
            void reset( Y* ptr, Deleter d, Alloc alloc ) {
                _drop_strong_count();
                _ptr = static_cast<element_type*>(ptr);
                _cb = new _control_block<Y>(d, alloc);
                _cb->add_strong_cnt();
            }

            T* get() const noexcept {
                return _ptr;
            }

            void swap( shared_ptr& r ) noexcept {
                {
                    element_type *et = _ptr;
                    _ptr = r._ptr;
                    r._ptr = et;
                }

                {
                    _control_block<T> *sc = _cb;
                    _cb = r._cb;
                    r._cb = sc;
                }
            }

            T& operator*() const noexcept {
                return *get();
            }

            T* operator->() const noexcept {
                return &operator*();
            }

            long use_count() const noexcept {
                return _cb->use_count();
            }

            bool unique() const noexcept {
                return use_count() == 1;
            }

            explicit operator bool() const noexcept {
                return get() != nullptr;
            }

            template< class Y >
            bool owner_before( const shared_ptr<Y>& other) const noexcept {
                return _cb < other._cb;
            }

            template< class Y >
            bool owner_before( const jr_std::weak_ptr<Y>& other) const noexcept {
                return _cb < other._cb;
            }
    };

    template< class T, class Alloc, class... Args >
    shared_ptr<T> allocate_shared( const Alloc& alloc,
                                   Args&&... args ) {
        T *p = alloc.allocate(1);
        alloc.construct(p, std::forward<Args>(args)...);
        return shared_ptr<T>(p);
    }

    template< class T, class... Args >
    shared_ptr<T> make_shared( Args&&... args ) {
        auto a = jr_std::allocator<T>();
        return allocate_shared< T,
                                jr_std::allocator<T>,
                                Args...
                              >(a,
                                std::forward<Args>(args)...);
    }

    template< class T, class U >
    shared_ptr<T> static_pointer_cast( const shared_ptr<U>& r ) noexcept {
        return shared_ptr<T>(r, static_cast<T*>(r.get()));
    }

    template< class T, class U >
    shared_ptr<T> dynamic_pointer_cast( const shared_ptr<U>& r ) noexcept {
        return shared_ptr<T>(r, dynamic_cast<T*>(r.get()));
    }

    template< class T, class U >
    shared_ptr<T> const_pointer_cast( const shared_ptr<U>& r ) noexcept {
        return shared_ptr<T>(r, const_cast<T*>(r.get()));
    }

    template< class Deleter, class T >
    Deleter* get_deleter( const shared_ptr<T>& p ) noexcept {
        return p._cb->_deleter;
    }

    template <class T, class U, class V>
    std::basic_ostream<U, V>&
    operator<<(std::basic_ostream<U, V>& os,
               const shared_ptr<T>& ptr) {
        os << ptr.get();
        return os;
    }

    template< class T >
    void swap( shared_ptr<T>& lhs, shared_ptr<T>& rhs ) noexcept {
        lhs.swap(rhs);
    }

    template<class T1, class T2>
    bool operator==(const jr_std::shared_ptr<T1>& x,
                    const jr_std::shared_ptr<T2>& y) {
        return x.get() == y.get();
    }

    template<class T1, class T2>
    bool operator!=(const jr_std::shared_ptr<T1>& x,
                    const jr_std::shared_ptr<T2>& y) {
        return !(x == y);
    }

    template<class T1, class T2>
    bool operator<(const jr_std::shared_ptr<T1>& x,
                   const jr_std::shared_ptr<T2>& y) {
        return *x < *y;
    }

    template<class T1, class T2>
    bool operator<=(const jr_std::shared_ptr<T1>& x,
                    const jr_std::shared_ptr<T2>& y) {
        return !(y < x);
    }

    template<class T1, class T2>
    bool operator>(const jr_std::shared_ptr<T1>& x,
                   const jr_std::shared_ptr<T2>& y) {
        return y < x;
    }

    template<class T1, class T2>
    bool operator>=(const jr_std::shared_ptr<T1>& x,
                    const jr_std::shared_ptr<T2>& y) {
        return !(x < y);
    }

    template <class T>
    bool operator==(const jr_std::shared_ptr<T>& x,
                    std::nullptr_t) noexcept {
        return !x;
    }

    template <class T>
    bool operator==(std::nullptr_t,
                    const jr_std::shared_ptr<T>& x) noexcept {
        return !x;
    }

    template <class T>
    bool operator!=(const jr_std::shared_ptr<T>& x,
                    std::nullptr_t) noexcept {
        return static_cast<bool>(x);
    }

    template <class T>
    bool operator!=(std::nullptr_t,
                    const jr_std::shared_ptr<T>& x) noexcept {
        return static_cast<bool>(x);
    }

    template <class T>
    bool operator<(const jr_std::shared_ptr<T>& x,
                   std::nullptr_t) {
        return x.get() < nullptr;
    }

    template <class T>
    bool operator<(std::nullptr_t,
                   const jr_std::shared_ptr<T>& y) {
        return nullptr < y.get();
    }

    template <class T>
    bool operator<=(const jr_std::shared_ptr<T>& x,
                    std::nullptr_t) {
        return !(nullptr < x);
    }

    template <class T>
    bool operator<=(std::nullptr_t,
                    const jr_std::shared_ptr<T>& y) {
        return !(y < nullptr);
    }

    template <class T>
    bool operator>(const jr_std::shared_ptr<T>& x,
                   std::nullptr_t) {
        return nullptr < x;
    }

    template <class T>
    bool operator>(std::nullptr_t,
                   const jr_std::shared_ptr<T>& y) {
        return y < nullptr;
    }

    template <class T>
    bool operator>=(const jr_std::shared_ptr<T>& x,
                    std::nullptr_t) {
        return !(x < nullptr);
    }

    template <class T>
    bool operator>=(std::nullptr_t,
                    const jr_std::shared_ptr<T>& y) {
        return !(nullptr < y);
    }

    /*==============weak ptr==============*/
    template< class T >
    class weak_ptr {

    template< class Y >
    friend class shared_ptr;

    template< class U >
    friend class weak_ptr;

    public:
        typedef T element_type;

    private:
        _control_block_base *_cb;
        element_type *_ptr;

    public:
        constexpr weak_ptr() noexcept
            : _cb(nullptr),
              _ptr(nullptr)
        {}

        weak_ptr( const weak_ptr& r ) noexcept
            : _cb(r._cb),
              _ptr(r._ptr) {
            if(_cb)
                _cb->add_weak_cnt();
        }

        template< class Y >
        weak_ptr( const weak_ptr<Y>& r ) noexcept
            : _cb(r._cb),
              _ptr(static_cast<element_type*>(r._ptr)) {
            if(_cb)
                _cb->add_weak_cnt();
        }

        template< class Y >
        weak_ptr( const shared_ptr<Y>& r ) noexcept
            : _cb(r._cb),
              _ptr(r._ptr) {
            if(_cb)
                _cb->add_weak_cnt();
        }

        weak_ptr( weak_ptr&& r ) noexcept
            : _cb(r._cb),
              _ptr(r._ptr) {
            r._cb = nullptr;
            r._ptr = nullptr;
        }

        template< class Y >
        weak_ptr( weak_ptr<Y>&& r ) noexcept
            : _cb(r._cb),
              _ptr(static_cast<element_type*>(r._ptr)) {
            r._cb = nullptr;
            r._ptr = nullptr;
        }

        ~weak_ptr() {
            if(_cb && !_cb->weak_count()) {
                delete _cb;
                _cb = nullptr;
            }
        }

        weak_ptr& operator=( const weak_ptr& r ) noexcept {
            if(this != &r) {
                _cb = r._cb;
                _ptr = r._ptr;
                if(_cb)
                    _cb->add_weak_cnt();
            }
            return *this;
        }

        template< class Y >
        weak_ptr& operator=( const weak_ptr<Y>& r ) noexcept {
            _cb = r._cb;
            _ptr = static_cast<element_type*>(r._ptr);
            if(_cb)
                _cb->add_weak_cnt();
            return *this;
        }

        template< class Y >
        weak_ptr& operator=( const shared_ptr<Y>& r ) noexcept {
            _cb = r._cb;
            _ptr = r._ptr;
            if(_cb)
                _cb->add_weak_cnt();
            return *this;
        }

        template< class Y >
        weak_ptr& operator=( weak_ptr<Y>&& r ) noexcept {
            _cb = r._cb;
            _ptr = static_cast<element_type*>(r._ptr);
            r._cb = nullptr;
            r._ptr = nullptr;
            return *this;
        }

        weak_ptr& operator=( weak_ptr&& r ) noexcept {
            if(this != &r) {
                _cb = r._cb;
                _ptr = r._ptr;
                r._cb = nullptr;
                r._ptr = nullptr;
            }
            return *this;
        }

        void reset() noexcept {
            if(_cb)
                _cb->decrease_weak_cnt();
            _cb = nullptr;
            _ptr = nullptr;
        }

        void swap( weak_ptr& r ) noexcept {
            {
                int *wt = _cb;
                _cb = r._cb;
                r._cb = wt;
            }

            {
                element_type *pt = _ptr;
                _ptr = r._ptr;
                r._ptr = pt;
            }
        }

        long use_count() const noexcept {
            return _cb->use_count();
        }

        bool expired() const noexcept {
            return _ptr == nullptr;
        }

        shared_ptr<T> lock() const noexcept {
            return use_count() ? shared_ptr<element_type>(*this)
                               : shared_ptr<element_type>();
        }

        template< class Y >
        bool owner_before( const weak_ptr<Y>& other) const noexcept {
                return _cb < other._cb;
        }

        template< class Y >
        bool owner_before( const shared_ptr<Y>& other) const noexcept {
            return _cb < other._cb->_cb;
        }
    };

    template< class T >
    void swap( jr_std::weak_ptr<T>& lhs,
               jr_std::weak_ptr<T>& rhs ) noexcept {
        lhs.swap(rhs);
    }
}

#endif // JR_SMART_PTR_H
