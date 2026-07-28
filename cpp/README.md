# C++ 面试专题

按主题组织的 C++ 面试题集合，每题包含完整代码实现与关键要点。

## 主题列表

| # | 主题 | 前置知识 | 状态 |
|---|------|----------|:----:|
| 01 | [对象生命周期](./01-object-lifetime/) | 构造/析构、RAII | ✅ 进行中 |
| 02 | [内存管理](./02-memory-management/) | 堆栈、new/delete | 📋 待添加 |
| 03 | [多态](./03-polymorphism/) | 继承、虚函数 | 📋 待添加 |
| 04 | [智能指针](./04-smart-pointers/) | RAII、引用计数 | 📋 待添加 |
| 05 | [STL 容器](./05-stl-containers/) | 模板、迭代器 | 📋 待添加 |
| 06 | [并发与多线程](./06-concurrency/) | 线程、锁 | 📋 待添加 |
| 07 | [设计模式](./07-design-patterns/) | 面向对象设计 | 📋 待添加 |
| 08 | [编译与链接](./08-compiler-link/) | 编译原理 | 📋 待添加 |
| 09 | [现代 C++ 特性](./09-modern-cpp/) | C++11 起新特性 | 📋 待添加 |

> ✅ 进行中 — 已有题目，可查阅  
> 📋 待添加 — 尚未添加题目

## 编译方法

```bash
# 进入对应题目目录
cd cpp/01-object-lifetime/

# 编译
g++ -std=c++17 -o mini_unique_ptr mini_unique_ptr.cpp

# 运行
./mini_unique_ptr
```
