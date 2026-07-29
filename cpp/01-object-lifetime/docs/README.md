# 对象生命周期

## 概述
C++ 对象的构造、析构、拷贝、移动等生命周期管理，以及 RAII（资源获取即初始化）原则。

## 题目列表

| # | 题目 | 难度 | 标签 | 要点 |
|---|------|:----:|------|------|
| 1 | [mini_unique_ptr](./mini_unique_ptr.cpp) | ⭐⭐ | RAII, 智能指针 | 构造获取资源，析构释放资源 |

## 编译运行

```bash
g++ -std=c++17 -o mini_unique_ptr mini_unique_ptr.cpp && ./mini_unique_ptr
```

## 参考
- 《Effective C++》Item 13 — 以对象管理资源
- 《Effective C++》Item 14 — 资源管理类中小心 copying 行为
- RAII（Resource Acquisition Is Initialization）