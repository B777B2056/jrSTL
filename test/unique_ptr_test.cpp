#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <memory>
#include <cstdio>
#include <fstream>
#include <cassert>
#include <functional>
#include "../memory/jr_smart_ptr.h"

std::string str = "", str0 = "";
std::shared_ptr<int> a;
struct B {
  virtual void bar() { str += "B::bar"; }
  virtual ~B() = default;
};
struct D : B
{
    D() { str += "D::D";  }
    ~D() { str += "D::~D";  }
    void bar() override { str += "D::bar";  }
};

// 消费 unique_ptr 的函数能以值或以右值引用接收它
std::unique_ptr<D> pass_through(std::unique_ptr<D> p)
{
    p->bar();
    return p;
}

struct B_j {
  virtual void bar() { str0 += "B::bar"; }
  virtual ~B_j() = default;
};
struct D_j : B_j
{
    D_j() { str0 += "D::D";  }
    ~D_j() { str0 += "D::~D";  }
    void bar() override { str0 += "D::bar";  }
};

// 消费 unique_ptr 的函数能以值或以右值引用接收它
jrSTL::unique_ptr<D_j> pass_through(jrSTL::unique_ptr<D_j> p)
{
    p->bar();
    return p;
}

void close_file(std::FILE* fp) { std::fclose(fp); }

void std_unique() {
    {
        auto p = std::unique_ptr<D>(new D()); // p 是占有 D 的 unique_ptr
        auto q = pass_through(std::move(p));
        assert(!p); // 现在 p 不占有任何内容并保有空指针
        q->bar();   // 而 q 占有 D 对象
    } // ~D 调用于此
    {
      std::unique_ptr<B> p = std::unique_ptr<D>(new D()); // p 是占有 D 的 unique_ptr
                                                    // 作为指向基类的指针
      p->bar(); // 虚派发

      std::vector<std::unique_ptr<B>> v;  // unique_ptr 能存储于容器
      v.push_back(std::unique_ptr<D>(new D()));
      v.push_back(std::move(p));
      v.emplace_back(new D());
      for(auto& p: v) p->bar(); // 虚派发
    } // ~D called 3 times
    std::ofstream("demo.txt") << 'x'; // 准备要读的文件
    {
        std::unique_ptr<std::FILE, void (*)(std::FILE*) > fp(std::fopen("demo.txt", "r"),
                                                             close_file);
        if(fp) // fopen 可以打开失败；该情况下 fp 保有空指针
          (char)std::fgetc(fp.get());
    } // fclose() 调用于此，但仅若 FILE* 不是空指针
      // （即 fopen 成功）
std::shared_ptr<D>();
    {
      std::unique_ptr<D, std::function<void(D*)>> p(new D, [](D* ptr)
          {
              delete ptr;
          });  // p 占有 D
      p->bar();
    } // 调用上述 lambda 并销毁 D

    {
        std::unique_ptr<D[]> p(new D[3]);
    } // 调用 ~D 3 次
}

void jr_std_unique() {
    {
        auto p = jrSTL::unique_ptr<D_j>(new D_j()); // p 是占有 D 的 unique_ptr
        auto q = pass_through(std::move(p));
        assert(!p); // 现在 p 不占有任何内容并保有空指针
        q->bar();   // 而 q 占有 D 对象
    } // ~D 调用于此
    {
      jrSTL::unique_ptr<B_j> p = jrSTL::unique_ptr<D_j>(new D_j()); // p 是占有 D 的 unique_ptr
                                                    // 作为指向基类的指针
      p->bar(); // 虚派发

      std::vector<jrSTL::unique_ptr<B_j>> v;  // unique_ptr 能存储于容器
      v.push_back(jrSTL::unique_ptr<D_j>(new D_j()));
      v.push_back(std::move(p));
      v.emplace_back(new D_j());
      for(auto& p: v) p->bar(); // 虚派发
    } // ~D called 3 times
    std::ofstream("demo.txt") << 'x'; // 准备要读的文件
    {
        jrSTL::unique_ptr<std::FILE, void (*)(std::FILE*) > fp(std::fopen("demo.txt", "r"),
                                                             close_file);
        if(fp) // fopen 可以打开失败；该情况下 fp 保有空指针
          (char)std::fgetc(fp.get());
    } // fclose() 调用于此，但仅若 FILE* 不是空指针
      // （即 fopen 成功）

    {
      jrSTL::unique_ptr<D_j, std::function<void(D_j*)>> p(new D_j, [](D_j* ptr)
          {
              delete ptr;
          });  // p 占有 D
      p->bar();
    } // 调用上述 lambda 并销毁 D

    {
        jrSTL::unique_ptr<D_j[]> p(new D_j[3]);
    } // 调用 ~D 3 次
}

TEST(testCase, unique_ptr_test) {
    std_unique();
    jr_std_unique();
    EXPECT_EQ(str, str0);
}

