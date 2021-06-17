#include <gtest/gtest.h>
#include "../memory/jr_smart_ptr.h"
#include <string>
#include <memory>

/*弱指针观察器功能测试*/
std::string ob = "", ob0 = "";

std::weak_ptr<int> gw;
jr_std::weak_ptr<int> gw_j;

void observe()
{
    ob += std::to_string(gw.use_count()) + " ";
    if (auto spt = gw.lock()) {
        ob += std::to_string(*spt) + " ";
    }
    else {
        ob += "gw is expired";
    }
}

void observe_j()
{
    ob0 += std::to_string(gw_j.use_count()) + " ";
    if (auto spt_j = gw_j.lock()) {
        ob0 += std::to_string(*spt_j) + " ";
    }
    else {
        ob0 += "gw is expired";
    }
}

TEST(testCase, weak_ptr_fun_test)
{
    {
        auto sp = std::make_shared<int>(42);
        gw = sp;
        observe();
    }
    observe();

    {
        auto sp_j = jr_std::make_shared<int>(42);
        gw_j = sp_j;
        observe_j();
    }
    observe_j();

    ASSERT_EQ(ob, ob0);
}

/*循环引用测试*/
std::string circle = "", circle0 = "";

TEST(testCase, weak_ptr_circle_ref_test) {

    class B;

    class A {
    public:
        A() {
            circle += "A::A()";
        }

        ~A() {
            circle += "A::~A()";
        }

        std::shared_ptr<B> b;
    };

    class B {
    public:
        B() {
            circle += "B::B()";
        }

        ~B() {
            circle += "B::~B()";
        }

        std::weak_ptr<A> a;
    };

    class B_j;

    class A_j {
    public:
        A_j() {
            circle0 += "A::A()";
        }

        ~A_j() {
            circle0 += "A::~A()";
        }

        jr_std::shared_ptr<B_j> b;
    };

    class B_j {
    public:
        B_j() {
            circle0 += "B::B()";
        }

        ~B_j() {
            circle0 += "B::~B()";
        }

        jr_std::weak_ptr<A_j> a;
    };

    std::shared_ptr<A> x(new A);
    x->b = std::shared_ptr<B>(new B);
    x->b->a = x;

    jr_std::shared_ptr<A_j> x_j(new A_j);
    x_j->b = jr_std::shared_ptr<B_j>(new B_j);
    x_j->b->a = x_j;

    ASSERT_EQ(circle, circle0);
}
