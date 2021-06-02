#ifndef JR_FUNCTIONAL_H
#define JR_FUNCTIONAL_H

#include <memory>
#include <tuple>
#include <bits/invoke.h>
#include <type_traits>

namespace jr_std {
    /*函数包装器*/
    // 存储任意类型函数对象
    template< class >
    class function;

    template< class R, class... Args >
    class function<R(Args...)>;

    // 函数包装器无目标时调用函数包装器所抛出的异常
    class bad_function_call;

    // 对成员函数指针的封装(非const成员函数)
    template< class M, class T, class... Args >
    struct _Mem_Fn {
        protected:
            // 类型定义
            typedef T* argument_type;
            typedef T* first_argument_type;
            typedef T second_argument_type;
            typedef M result_type;
            // 与传入的成员函数间的联系
            M (T::*fptr)(Args...);  // 成员函数指针

        public:
            explicit _Mem_Fn(M (T::*pm)(Args...)) : fptr(pm) {}

            M operator()(T& object, Args&&... args) {
                return (object.*fptr)(static_cast<Args&&>(args)...);
            }

            M operator()(T& object, Args&&... args) const {
                return (object.*fptr)(static_cast<Args&&>(args)...);
            }

            M operator()(T *object, Args&&... args) {
                return (object->*fptr)(static_cast<Args&&>(args)...);
            }

            M operator()(T *object, Args&&... args) const {
                return (object->*fptr)(static_cast<Args&&>(args)...);
            }
    };

    template< class M, class T, class... Args >
    _Mem_Fn<M, T, Args...> mem_fn(M (T::*pm)(Args...))
    { return _Mem_Fn<M, T, Args...>(pm); }

    // 对成员函数指针的封装(const成员函数)
    template< class M, class T, class... Args >
    struct _Mem_Fn_Const {
        protected:
            // 类型定义
            typedef T* argument_type;
            typedef T* first_argument_type;
            typedef T second_argument_type;
            typedef M result_type;
            // 与传入的成员函数间的联系
            M (T::*fptr)(Args...) const;  // 成员函数指针

        public:
            explicit _Mem_Fn_Const(M (T::*pm)(Args...) const) : fptr(pm) {}

            M operator()(const T& object, Args&&... args) {
                return (object.*fptr)(static_cast<Args&&>(args)...);
            }

            M operator()(const T *object, Args&&... args) {
                return (object->*fptr)(static_cast<Args&&>(args)...);
            }
    };

    template< class M, class T, class... Args >
    _Mem_Fn_Const<M, T, Args...> mem_fn(M (T::*pm)(Args...) const)
    { return _Mem_Fn_Const<M, T, Args...>(pm); }

    // 对成员变量指针的封装
    template< class M, class T >
    struct _Mem_Fn_Var {
        protected:
            // 类型定义
            typedef T* argument_type;
            typedef T* first_argument_type;
            typedef T second_argument_type;
            typedef M result_type;
            // 与传入的成员函数间的联系
            M T::*fptr;  // 成员变量指针

        public:
            explicit _Mem_Fn_Var(M T::*pm) : fptr(pm) {}

            M operator()(T& object) { return object.*fptr; }

            M operator()(T *object) { return object->*fptr; }
    };

    template< class M, class T >
    _Mem_Fn_Var<M, T> mem_fn(M T::*pm)
    { return _Mem_Fn_Var<M, T>(pm); }

    /*绑定器*/
    // 判断T是否为bind类型
    template< class T >
    struct is_bind_expression;

    // 标准占位符
    template<int _N>
    struct _Place_Holders {};

    namespace placeholders {
        extern const _Place_Holders<1> _1;
        extern const _Place_Holders<2> _2;
        extern const _Place_Holders<3> _3;
        extern const _Place_Holders<4> _4;
        extern const _Place_Holders<5> _5;
        extern const _Place_Holders<6> _6;
        extern const _Place_Holders<7> _7;
        extern const _Place_Holders<8> _8;
        extern const _Place_Holders<9> _9;
        extern const _Place_Holders<10> _10;
    }

    // 判断T是否为标准占位符
    template< class T >
    struct is_placeholder
            : std::integral_constant<int, 0> {};

    template< int _N >
    struct is_placeholder<_Place_Holders<_N> >
            : std::integral_constant<int, _N> {};

    // 有Bug
//    template< class F, class... Args >
//    struct _Bind_Without_Ret_Type {
//        protected:
//            F fun;  // 保存函数
//            std::tuple<Args&&...> args;  // 保存函数入参
//            // 如果参数为非占位符，就返回其自身
//            template<class TArgs, class InputPara>
//            struct choose_para {
//                decltype (auto) operator()(TArgs& a, InputPara&&) {
//                    return a;
//                }
//            };
//            // 如果参数为占位符N，就返回当前入参里第N个参数
//            template<int _N, class InputPara>
//            struct choose_para<_Place_Holders<_N>, InputPara> {
//                decltype (auto) operator()(_Place_Holders<_N>& a, InputPara&& ip) {
//                    return std::get<_N>(static_cast<InputPara&&>(ip));
//                }
//            };
//            // 调用
//            template< int...index, class TArg, class InputPara >
//            decltype (auto) bind_invoke(TArg& a, InputPara&& ip) {
//                return std::__invoke(fun, choose_para<TArg, InputPara>(std::get<index>(a),
//                                                      static_cast<InputPara&&>(ip))...);
//            }

//        public:
//            _Bind_Without_Ret_Type(F&& f, Args&&... a)
//                : fun(std::forward(f)),
//                  args(std::forward_as_tuple(a...)) {}

//            template< class... InputPara >
//            decltype (auto) operator()(InputPara&&... ip) {
//                return bind_invoke(std::index_sequence_for<Args...>{}, args,
//                                   static_cast<InputPara&&>(ip)...);
//            }
//    };

//    template< class R, class F, class... Args >
//    struct _Bind_With_Ret_Type {
//    protected:
//        F (R::*fptr)(Args...);

//    public:
//        _Bind_With_Ret_Type(F (R::*x)(Args...)) : fptr(x) {}

//        F operator()(R *object, Args&&... args)
//        { return (object->*fptr)(static_cast<Args&&>(args)...); }

//        F operator()(R &object, Args&&... args)
//        { return (object.*fptr)(static_cast<Args&&>(args)...); }
//    };

//    template< class F, class... Args >
//    _Bind_Without_Ret_Type<F, Args...> bind(F&& f, Args&&...) {
//        return _Bind_Without_Ret_Type<F, Args...>(f);
//    }

//    template< class R, class F, class... Args >
//    _Bind_With_Ret_Type<R, F, Args...> bind(F&& f, Args&&...) {
//        return _Bind_With_Ret_Type<R, F, Args...>(f);
//    }

    /*引用包装*/
    // 包装引用于可复制、可赋值对象的类模板，该类的对象的表现如同一个引用
    // 主要用途：可以向容器内存入引用包装器对象，以弥补容器内不可存储实际引用的特点
    template< class T >
    class reference_wrapper {
        protected:
            T *ptr;

        public:
            // 采用std::addressof而不是c++原生的&来获取地址的原因：传入的对象x可能重载过运算符&
            reference_wrapper( T& x ) noexcept : ptr(std::addressof(x)) {}

            reference_wrapper( T&& x ) = delete;

            reference_wrapper( const reference_wrapper& other ) noexcept = default;

            reference_wrapper& operator=( const reference_wrapper& other ) noexcept = default;

            operator T& () const noexcept { return *ptr; }

            T& get() const noexcept { return *ptr; }

            template< class... ArgTypes >
            typename std::result_of<T&(ArgTypes&&...)>::type
            operator() ( ArgTypes&&... args ) const {
                return *ptr(static_cast<ArgTypes&&>(args)...);
            }
    };

    template< class T >
    reference_wrapper<T> ref(T& t) noexcept
    { return reference_wrapper<T>(t); }

    template< class T >
    reference_wrapper<T> ref( reference_wrapper<T> t ) noexcept
    { return reference_wrapper<T>(t.get()); }

    template <class T>
    void ref(const T&&) = delete;

    template< class T >
    reference_wrapper<const T> cref( const T& t ) noexcept
    { return reference_wrapper<const T>(t); }

    template< class T >
    reference_wrapper<const T> cref(reference_wrapper<T> t) noexcept
    { return reference_wrapper<const T>(t.get()); }

    template <class T>
    void cref(const T&&) = delete;

    /*算数运算*/
    template< class T >
    struct plus {
        protected:
            typedef T first_argument_type;
            typedef T second_argument_type;
            typedef T result_type;
        public:
            result_type
            operator()(const first_argument_type& x, const second_argument_type& y) const {
                return x + y;
            }
    };

    template< class T >
    struct minus {
        protected:
            typedef T first_argument_type;
            typedef T second_argument_type;
            typedef T result_type;
        public:
            result_type
            operator()(const first_argument_type& x, const second_argument_type& y) const {
                return x - y;
            }
    };

    template< class T >
    struct multiplies {
        protected:
            typedef T first_argument_type;
            typedef T second_argument_type;
            typedef T result_type;
        public:
            result_type
            operator()(const first_argument_type& x, const second_argument_type& y) const {
                return x * y;
            }
    };

    template< class T >
    struct divides {
        protected:
            typedef T first_argument_type;
            typedef T second_argument_type;
            typedef T result_type;
        public:
            result_type
            operator()(const first_argument_type& x, const second_argument_type& y) const {
                return x / y;
            }
    };

    template< class T >
    struct modulus {
        protected:
            typedef T first_argument_type;
            typedef T second_argument_type;
            typedef T result_type;
        public:
            result_type
            operator()(const first_argument_type& x, const second_argument_type& y) const {
                return x % y;
            }
    };

    template< class T >
    struct negate {
        protected:
            typedef T argument_type;
            typedef T result_type;
        public:
            result_type operator()(const argument_type& x) const {
                return -x;
            }
    };

    /*比较运算*/
    template< class T >
    struct equal_to {
        protected:
            typedef T first_argument_type;
            typedef T second_argument_type;
            typedef bool result_type;
        public:
            result_type
            operator()(const first_argument_type& x, const second_argument_type& y) const {
                return x == y;
            }
    };

    template< class T >
    struct not_equal_to {
        protected:
            typedef T first_argument_type;
            typedef T second_argument_type;
            typedef bool result_type;
        public:
            result_type
            operator()(const first_argument_type& x, const second_argument_type& y) const {
                return x != y;
            }
    };

    template< class T >
    struct less {
        protected:
            typedef T first_argument_type;
            typedef T second_argument_type;
            typedef bool result_type;
        public:
            result_type
            operator()(const first_argument_type& x,
                       const second_argument_type& y) const {
                return x < y;
            }
    };

    template< class T >
    struct greater {
        protected:
            typedef T first_argument_type;
            typedef T second_argument_type;
            typedef bool result_type;
        public:
            result_type
            operator()(const first_argument_type& x, const second_argument_type& y) const {
                return x > y;
            }
    };

    template< class T >
    struct less_equal {
        protected:
            typedef T first_argument_type;
            typedef T second_argument_type;
            typedef bool result_type;
        public:
            result_type
            operator()(const first_argument_type& x, const second_argument_type& y) const {
                return x <= y;
            }
    };

    template< class T >
    struct greater_equal {
        protected:
            typedef T first_argument_type;
            typedef T second_argument_type;
            typedef bool result_type;
        public:
            result_type
            operator()(const first_argument_type& x, const second_argument_type& y) const {
                return x >= y;
            }
    };

    /*逻辑运算*/
    template< class T >
    struct logical_and {
        protected:
            typedef T first_argument_type;
            typedef T second_argument_type;
            typedef bool result_type;
        public:
            result_type
            operator()(const first_argument_type& x, const second_argument_type& y) const {
                return x and y;
            }
    };

    template< class T >
    struct logical_or {
        protected:
            typedef T first_argument_type;
            typedef T second_argument_type;
            typedef bool result_type;
        public:
            result_type
            operator()(const first_argument_type& x, const second_argument_type& y) const {
                return x or y;
            }
    };

    template< class T >
    struct logical_not {
        protected:
            typedef T argument_type;
            typedef bool result_type;
        public:
            result_type
            operator()(const argument_type& x) const {
                return not x;
            }
    };

    /*位运算*/
    template< class T >
    struct bit_and {
        protected:
            typedef T first_argument_type;
            typedef T second_argument_type;
            typedef bool result_type;
        public:
            result_type
            operator()(const first_argument_type& x, const second_argument_type& y) const {
                return x & y;
            }
    };

    template< class T >
    struct bit_or {
        protected:
            typedef T first_argument_type;
            typedef T second_argument_type;
            typedef bool result_type;
        public:
            result_type
            operator()(const first_argument_type& x, const second_argument_type& y) const {
                return x | y;
            }
    };

    template< class T >
    struct bit_xor {
        protected:
            typedef T first_argument_type;
            typedef T second_argument_type;
            typedef bool result_type;
        public:
            result_type operator()(const first_argument_type& x, const second_argument_type& y){
                return x ^ y;
            }
    };
}

#endif // JR_FUNCTIONAL_H
