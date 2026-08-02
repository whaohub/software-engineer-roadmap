# Software Engineer Roadmap 🗺️

C++ 面试题知识库，按主题分类，每题包含代码实现与关键要点。

## 学习阶段

```
第一阶段: C++ 语言层 (cpp/)           ✅ 进行中
第二阶段: Linux 系统层 (linux/)       🚧 已规划
第三阶段: 中间件 / 分布式             📋 待规划
```

## C++ 专题

| # | 主题 | 题目数 | 涵盖内容 |
|---|------|:------:|----------|
| 01 | [对象生命周期](./cpp/01-object-lifetime/) | 1 | RAII、构造/析构顺序、拷贝控制 |
| 02 | [内存管理](./cpp/02-memory-management/) | 0 | new/delete、内存泄漏、内存布局 |
| 03 | [多态](./cpp/03-polymorphism/) | 0 | 虚函数表、抽象类、菱形继承 |
| 04 | [智能指针](./cpp/04-smart-pointers/) | 0 | shared_ptr、weak_ptr、循环引用 |
| 05 | [STL 容器](./cpp/05-stl-containers/) | 0 | vector、map、unordered_map 实现原理 |
| 06 | [并发与多线程](./cpp/06-concurrency/) | 0 | 线程安全、锁、原子操作 |
| 07 | [设计模式](./cpp/07-design-patterns/) | 0 | 单例、工厂、观察者等 |
| 08 | [编译与链接](./cpp/08-compiler-link/) | 0 | 编译过程、链接、符号表 |
| 09 | [现代 C++ 特性](./cpp/09-modern-cpp/) | 0 | C++11/14/17/20 新特性 |

---

## Linux 专题（第二阶段）

| # | 主题 | 面试热度 | 涵盖内容 |
|---|------|:--------:|----------|
| 01 | [进程](./linux/01-process/) | 🔥🔥🔥🔥🔥 | fork、exec、waitpid、daemon、僵尸进程 |
| 02 | [线程](./linux/02-thread/) | 🔥🔥🔥🔥🔥 | pthread、互斥锁、条件变量、线程池 |
| 03 | [内存](./linux/03-memory/) | 🔥🔥🔥🔥🔥 | mmap、page cache、malloc 底层、COW |
| 04 | [网络](./linux/04-network/) | 🔥🔥🔥🔥🔥 | TCP 三次握手、TIME_WAIT、socket 编程 |
| 05 | [文件系统](./linux/05-filesystem/) | 🔥🔥🔥🔥 | VFS、inode、硬/软链接、fd |
| 06 | [IPC](./linux/06-ipc/) | 🔥🔥🔥🔥🔥 | pipe、fifo、shm、eventfd、信号量 |
| 07 | [Epoll](./linux/07-epoll/) | 🔥🔥🔥🔥🔥 | select/poll/epoll、LT/ET、Reactor |
| 08 | [构建](./linux/08-build/) | 🔥🔥🔥 | 编译流程、静态/动态库、链接装载 |
| 09 | [调试](./linux/09-debug/) | 🔥🔥🔥🔥 | gdb、core dump、strace、perf |

---

*C++ 阶段每道题一个 `.cpp` 文件，文件头包含题目、难度、标签和要点说明。*  
*Linux 阶段每题一个 `Qxxx` 文档 + `src/` 代码示例。*
