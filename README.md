# Software Engineer Roadmap 🗺️

C++ 面试题知识库，按主题分类，每题包含代码实现与关键要点。

## 🚀 开始工作（新机器 / 新 agent / 新会话）

```bash
git clone <repo> && cd software-engineer-roadmap
./scripts/bootstrap.sh     # 工具链自检（首次）
make status                # 一键拿到：当前阶段 / 下一步 / 未提交改动
```

然后**只读** `NEXT.md` 指向的文件，不要通读全仓。约定见 **[AGENTS.md](./AGENTS.md)**。

| 文件 | 作用 |
|---|---|
| [AGENTS.md](./AGENTS.md) | 工作契约：铁律、会话协议、验收命令、提交规范（**任何 agent 都读这个**） |
| [NEXT.md](./NEXT.md) | 唯一的「下一步」，每次开工/收工更新 |
| [PROGRESS.md](./PROGRESS.md) | 阶段状态 + 会话日志（跨设备恢复上下文靠它 + `git log`） |
| [PLAN.md](./PLAN.md) | 五阶段计划、四件套标准、AI 时代重点 |
| `scripts/` | `status.sh` 上下文 / `check.sh` 验收 / `bootstrap.sh` 环境 |
| `Makefile` | 统一入口：`make status/check/test/demo`（IDE agent 直接用） |
| `.vscode/tasks.json` | VS Code 任务面板（已入库，换机器也在） |

> 📌 **学习规划见 [PLAN.md](./PLAN.md)**（阶段划分、每周节奏、交付标准、AI 时代重点）
> 现状：28 个专题中 4 个有实质内容（14%）—— 策略已改为**串行深挖，停止横向铺骨架**。

## 学习阶段

```
第 0 层 工具链: git / CMake / gdb / perf            🚧 提前学（linux/08-build, 09-debug）
第一阶段: C++ 语言层 (cpp/)           ✅ 进行中
第二阶段: Linux 系统层 (linux/)       🚧 已规划
第三阶段: 中间件 (middleware/)         🚧 已建骨架
第四阶段: 性能与确定性 (perf/)         📋 待建
第五阶段: 作品与面试 (project/, interview/) 📋 待建
可选延伸: 执行管理实践 (runtime/)      💡 想补作品再做
```

> 范围：**AUTOSAR 规范不在学习范围**（EM 属于工作经验，转为案例整理，见 PLAN.md）。

每个专题的完成标准是「四件套」：Q 文档 + 可运行 demo + 实测数据 + 自测卡（见 [PLAN.md](./PLAN.md#4-交付标准每个专题的四件套)）。

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

---

## 中间件专题（第四阶段）

面向自动驾驶中间件：通信模型、共享内存零拷贝、序列化、DDS / SOME-IP、传输与时序。

详见 [middleware/README.md](./middleware/README.md) —— 该阶段的核心交付物是 **mini-ipc**（共享内存 + 无锁队列 + 生命周期管理，含 p99 延迟数据）。

