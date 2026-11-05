#include <iostream>
#include <memory>

struct B;

struct A {
  A() { std::cout << "A ctor" << std::endl; }
  ~A() { std::cout << "A dtor" << std::endl; }
  std::shared_ptr<B> sb;
};

struct B {
  B() { std::cout << "B ctor" << std::endl; }
  ~B() { std::cout << "B dtor" << std::endl; }
  std::weak_ptr<A> sa;
};

int main() {

  {
    auto b = std::make_shared<B>();
    auto a = std::make_shared<A>();
    b->sa = a;
    a->sb = b;
    std::cout << "a use count: " << a->sb.use_count() << " b use count"
              << b->sa.use_count() << std::endl;
  }

  return 0;
}