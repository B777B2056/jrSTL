#include <gtest/gtest.h>
#include "../memory/jr_smart_ptr.h"
#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include <mutex>
#include <sstream>

/*构造函数测试*/
std::string ctor = "", ctor0 = "";
TEST(testCase, shared_ptr_ctor_test)
{
    struct Foo {
        Foo() { ctor += "Foo..."; }
        ~Foo() { ctor += "~Foo..."; }
    };

    struct D {
        void operator()(Foo* p) const {
            ctor += "Call delete from function object...";
            delete p;
        }
    };

    struct Foo_j {
        Foo_j() { ctor0 += "Foo..."; }
        ~Foo_j() { ctor0 += "~Foo..."; }
    };

    struct D_j {
        void operator()(Foo_j* p) const {
            ctor0 += "Call delete from function object...";
            delete p;
        }
    };

    {
        std::shared_ptr<Foo> sh1;
    }

    {
        std::shared_ptr<Foo> sh2(new Foo);
        std::shared_ptr<Foo> sh3(sh2);
        ctor += std::to_string(sh2.use_count());
        ctor += std::to_string(sh3.use_count());
    }

    {
        std::shared_ptr<Foo> sh4(new Foo, D());
        std::shared_ptr<Foo> sh5(new Foo, [](Foo *p) {
           delete p;
        });
    }

    {
        jrSTL::shared_ptr<Foo_j> sh1_j;
    }

    {
        jrSTL::shared_ptr<Foo_j> sh2_j(new Foo_j);
        jrSTL::shared_ptr<Foo_j> sh3_j(sh2_j);
        ctor0 += std::to_string(sh2_j.use_count());
        ctor0 += std::to_string(sh3_j.use_count());
    }

    {
        jrSTL::shared_ptr<Foo_j> sh4_j(new Foo_j, D_j());
        jrSTL::shared_ptr<Foo_j> sh5_j(new Foo_j, [](Foo_j *p) {
           delete p;
        });
    }
    ASSERT_EQ(ctor, ctor0);
}

/*reset测试*/
std::string reset = "", reset0 = "";

TEST(testCase, shared_ptr_reset_test)
{
    struct Foo {
        Foo(int n = 0) noexcept : bar(n) {
            reset += ("Foo: constructor, bar = " + std::to_string(bar));
        }
        ~Foo() {
             reset += ("Foo: destructor, bar = " + std::to_string(bar));
        }
        int getBar() const noexcept { return bar; }
    private:
        int bar;
    };

    struct Foo_j {
        Foo_j(int n = 0) noexcept : bar(n) {
            reset0 += ("Foo: constructor, bar = " + std::to_string(bar));
        }
        ~Foo_j() {
             reset0 += ("Foo: destructor, bar = " + std::to_string(bar));
        }
        int getBar() const noexcept { return bar; }
    private:
        int bar;
    };

    std::shared_ptr<Foo> sptr = std::make_shared<Foo>(1);
    reset += ("The first Foo's bar is " + std::to_string(sptr->getBar()));

    // 重置，交与新的 Foo 实例
    // （此调用后将销毁旧实例）
    sptr.reset(new Foo);
    reset += ("The second Foo's bar is " + std::to_string(sptr->getBar()));

    jrSTL::shared_ptr<Foo_j> sptr_j = jrSTL::make_shared<Foo_j>(1);
    reset0 += ("The first Foo's bar is " + std::to_string(sptr_j->getBar()));

    // 重置，交与新的 Foo 实例
    // （此调用后将销毁旧实例）
    sptr_j.reset(new Foo_j);
    reset0 += ("The second Foo's bar is " + std::to_string(sptr_j->getBar()));

    ASSERT_EQ(reset, reset0);
}

/*强引用计数测试*/
std::string ref_cnt = "", ref_cnt0 = "";

void fun(std::shared_ptr<int> sp)
{
    ref_cnt += ("fun: sp.use_count() == " + std::to_string(sp.use_count()));
}

void fun(jrSTL::shared_ptr<int> sp)
{
    ref_cnt0 += ("fun: sp.use_count() == " + std::to_string(sp.use_count()));
}

TEST(testCase, shared_ptr_use_count_test)
{
    auto sp1 = std::make_shared<int>(5);
    ref_cnt += ("sp.use_count() == " + std::to_string(sp1.use_count()));
    fun(sp1);

    auto sp2 = jrSTL::make_shared<int>(5);
    ref_cnt0 += ("sp.use_count() == " + std::to_string(sp2.use_count()));
    fun(sp2);

    ASSERT_EQ(ref_cnt, ref_cnt0);
}

/*类型强转测试*/
std::string cast = "", cast0 = "";

TEST(testCase, shared_ptr_type_cast_test) {
    struct BaseClass {};

    struct DerivedClass : BaseClass
    {
        void f() const
        {
            cast += "Hello World!";
        }
        ~DerivedClass(){ // 注意：它不是虚的
            cast += "~DerivedClass";
        }
    };

    struct BaseClass_j {};

    struct DerivedClass_j : BaseClass_j
    {
        void f() const
        {
            cast0 += "Hello World!";
        }
        ~DerivedClass_j(){ // 注意：它不是虚的
            cast0 += "~DerivedClass";
        }
    };

    std::shared_ptr<BaseClass> ptr_to_base(std::make_shared<DerivedClass>());

    std::static_pointer_cast<DerivedClass>(ptr_to_base)->f(); // OK
    // （构造临时 shared_ptr ，然后调用 operator-> ）

    static_cast<DerivedClass*>(ptr_to_base.get())->f(); // 亦 OK
    // （直接转型，不构造临时 shared_ptr ）

    jrSTL::shared_ptr<BaseClass_j> ptr_to_base0(jrSTL::make_shared<DerivedClass_j>());

    jrSTL::static_pointer_cast<DerivedClass_j>(ptr_to_base0)->f(); // OK
    // （构造临时 shared_ptr ，然后调用 operator-> ）

    static_cast<DerivedClass_j*>(ptr_to_base0.get())->f(); // 亦 OK
    // （直接转型，不构造临时 shared_ptr ）

    ASSERT_EQ(cast, cast0);
}
