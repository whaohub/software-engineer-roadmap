/**
 * 题目: 基于RAII实现mini_unique_ptr
 * 难度: ⭐⭐
 * 标签: RAII, 智能指针, 对象生命周期
 *
 * 要点:
 * 1. RAII 核心: 构造函数获取资源，析构函数释放资源
 * 2. 当前实现缺少拷贝控制（拷贝构造/赋值应 delete 或实现移动语义）
 * 3. 面试常问: 与 shared_ptr 对比、移动语义、异常安全
 */

#include <iostream>
#include <ostream>
class Test {
public:
  Test() { std::cout << "Test constructor" << std::endl; }

  ~Test() { std::cout << "Test destructor" << std::endl; }
};

template <typename T> class MiniUniquePtr {
public:
  explicit MiniUniquePtr(T *ptr = nullptr) : ptr_(ptr) {}
  ~MiniUniquePtr() { delete ptr_; }

  T *operator->() { return ptr_; }
  T &operator*() { return *ptr_; }

private:
  T *ptr_;
};

int main() {
  MiniUniquePtr<Test> test(new Test());
  return 0;
}