# 实战项目：`mini-pipeline`（可取消、可观测的进程内数据流水线）

> **为什么做这个**：`cpp/` 目录的 9 个专题是"知识点"，工程里它们是"同一坨代码的不同侧面"。
> 这个项目用**一个真实跑得起来的组件**把它们串起来 —— 形态对标你工作里的
> 「中间件数据通路 + 执行管理」：一块内存池 → 一个引用计数句柄 → 一条带背压的通道 →
> 一组可取消的处理阶段 → 一个能优雅关闭的运行时。
>
> **需求规格（做什么/怎么算通过）**：[REQUIREMENTS.md](./REQUIREMENTS.md) —— 16 条功能需求 + 6 条非功能指标。
> **它是不是玩具**：不是。冷启动对照表见 REQUIREMENTS.md §2（iceoryx / DPDK / Disruptor / Cyber RT / ROS 2 intra-process / AUTOSAR AP EM / systemd / K8s）。
>
> **学习方式**：每个里程碑都**内置了必然会踩的坑**（内存序、双释放、false sharing、ODR、关闭死锁）。
> 不是先学再写，而是**先写 → 踩坑 → 用工具定位 → 修 → 留下数据**。

---

## 1. 与 `cpp/` 目录的映射

| 里程碑 | 覆盖的 cpp/ 专题 | 你要写的核心机制 |
|:--:|---|---|
| **M1 句柄与内存池** | 01 对象生命周期、02 内存管理、04 智能指针、05 STL 容器 | 定长块池（free list）、自定义引用计数句柄（`<=16B`）、RAII 归还 |
| **M2 通道与背压** | 06 并发、09 现代特性 | 有界队列三实现（mutex / spin / SPSC ring）、背压策略、`atomic` 内存序 |
| **M3 阶段抽象** | 03 多态、07 设计模式、09 现代特性 | 三种 Stage 实现（虚函数 / `std::function` / 类型擦除）+ 性能对比 |
| **M4 运行时与关闭** | 01、06、09（`stop_token`、`concepts`） | 启动顺序、取消传播、看门狗超时、优雅关闭、统计 |
| **M5 构建与性能** | 08 编译链接 + 阶段 D 性能 | 静态库/符号可见性/ODR/LTO + `perf` 火焰图与延迟直方图 |

> 每条都需要交付"**四件套**"：代码 + 测试 + **数据** + 一篇问题复盘文档（现象→工具→根因→修复→前后对比）。

## 2. 总目标（毕业标准）

> 指标编号与测量方法以 [REQUIREMENTS.md](./REQUIREMENTS.md) §5 为准（N1-N6）：Debug 构建的数字不采信。

1. `make test DIR=project/mini-pipeline` 全绿，且 `-fsanitize=address,undefined` 与 `=thread` 各跑一遍干净
2. 一条 3 阶段流水线：`读入 → 处理 → 输出`，处理 100 万条 1KB 消息，**端到端 p99 延迟与吞吐有数字**
3. `docs/` 里 5 篇问题复盘，每篇都有**工具输出**（ASan/TSan/perf/heaptrack 的原始片段）
4. 能在面试里 10 分钟讲完："为什么句柄只有 16 字节""为什么通道要三种实现""关闭顺序怎么定的"

---

## 3. M1 · 句柄与内存池（2 周，4h/周）

### 你要交付的 API

```cpp
namespace mp {
class BufferPool;   // 定长块池：预分配 + free list

class Buffer {      // 句柄：拷贝增引用，最后一个析构归还池
 public:
  Buffer() noexcept;                       // 空句柄
  std::byte* data() const noexcept;
  std::size_t size() const noexcept;
  std::uint32_t use_count() const noexcept;
  void reset() noexcept;                   // 归还
  explicit operator bool() const noexcept;
};

BufferPool& global_pool();                 // 单例，M5 讨论静态初始化顺序
}
```

### 硬性指标（不是"尽量"，是必须）

| 指标 | 要求 | 为什么 |
|---|:--:|---|
| `sizeof(mp::Buffer)` | **≤ 16** | 要能塞进通道的槽位，不然每来一条消息都要再分配 |
| `acquire()` 单线程 | < 30 ns | 纯指针操作 + free list 弹出 |
| 多线程 8 线程并发 acquire/release | 无泄漏、无崩溃，`use_count` 正确 | 引用计数必须是原子的 |
| 池耗尽行为 | **明确定义**（返回空句柄 or 抛 `std::bad_alloc`），并有测试 | 工程里"静默返回野指针"是灾难 |

### 内置的坑（必须踩到并写进复盘）

1. **双释放**：引用计数用 `int` 而不是 `atomic` → 8 线程压测下 `double free`
   → 定位：`-fsanitize=thread` 报告 data race；`-fsanitize=address` 报告 heap-use-after-free
2. **块归还竞态**：free list 用 `std::vector::push_back` 且不加锁 → 结构损坏
   → 定位：ASan 的 `container-overflow` / 随机崩溃；改用"索引 + 原子 CAS"或每线程独立缓存
3. **池与句柄的析构顺序**：全局池先于句柄析构（静态析构顺序）
   → 定位：退出时 crash；`cpp/08` 里要讲清 `atexit` 与 `__cxa_atexit` 的顺序
4. **句柄膨胀**：控制块放在堆上 → `sizeof(Buffer)` 变成 24/32，达不到 ≤16
   → 解法：控制块内联在块头（`std::byte[block]` 前 16 字节），或索引 + 池指针压缩

### 验收

```bash
make test DIR=project/mini-pipeline                 # 单测全绿
make test-asan DIR=project/mini-pipeline            # ASan+UBSan 干净
make test-tsan DIR=project/mini-pipeline            # TSan 无 data race
```

### 起步：测试已写好（现在是红的）

`tests/buffer_pool_test.cpp` 共 12 个用例，覆盖：
句柄大小 ≤16 / acquire 计数 / copy-move 语义 / 归还后 `use_count==0` /
池耗尽语义 / 单线程 1e6 次无泄漏 / 8 线程压测 / 块数据不串味 / 对齐检查 / 空句柄安全 / 统计峰值。

**你的任务**：让它们变绿，并且**在 `docs/M1-句柄与内存池.md` 里记录你踩的坑**。

---

## 4. M2 · 通道与背压（2 周）

```cpp
template <class T>
class BoundedChannel {           // 有界、可关闭、支持背压策略
 public:
  enum class Policy { Block, DropOldest, DropNewest, Fail };
  explicit BoundedChannel(std::size_t cap, Policy p);
  bool push(T v);                // false = 被策略拒绝
  bool pop(T& out);              // false = 已关闭且空
  void close();
};
```

- 三种实现：`MutexChannel` / `SpinChannel` / `SpscRingChannel`，用 benchmark 对比**延迟分位**与**吞吐**
- 内置坑：条件变量**虚假唤醒**与**丢失唤醒**（`notify` 在读之前）、`relaxed` 内存序导致元素乱序可见、
  统计计数器与队列索引**同一条 cache line** → false sharing（`perf c2c` 或自写微基准量化）
- 验收：`docs/M2-通道与背压.md` 里给出三种实现的 p50/p99 与"为什么"

## 5. M3 · 阶段抽象（1-2 周）

同一个 `Stage` 语义，三种实现：`virtual` / `std::function` / 手写类型擦除（vtable + small-buffer）。
- 内置坑：多重继承下的 `this` 指针调整、`std::function` 的堆分配（>16 字节捕获）、
  类型擦除中的对象生命周期与 `noexcept` 正确性
- 验收：benchmark 表 + 一段"什么时候该用哪种"的判断（面试必问）

## 6. M4 · 运行时与优雅关闭（1-2 周）

`Runtime`：按依赖顺序启动阶段 → 运行 → 收到 `stop_token` → 排空在途数据 → 关闭通道 → join 线程。
- 内置坑：关闭时**丢最后一条数据**、join 顺序造成**死锁**、看门狗把慢阶段误杀
- 验收：`docs/M4-优雅关闭.md` 含关闭时序图 + 一次真实的死锁定位过程

## 7. M5 · 构建与性能（1 周，接阶段 D）

- 拆静态库、`-fvisibility=hidden`、演示一次 **ODR 违规**（头文件里非 inline 定义导致的静默重复符号）
- LTO 开关前后对比；`perf record` → 火焰图；延迟直方图（p50/p99/p999）
- 验收：`docs/M5-构建与性能.md`（含火焰图与优化前后数字）

---

## 8. 工具与命令

| 目的 | 命令 |
|---|---|
| 单测 | `make test DIR=project/mini-pipeline` |
| 内存/UB | `make test-asan DIR=project/mini-pipeline` |
| 数据竞争 | `make test-tsan DIR=project/mini-pipeline` |
| 泄漏 | `heaptrack ./build/buffer_pool_test`（Linux/容器内） |
| 性能 | `perf record -g ./build/bench && perf report` |
| 火焰图 | `perf script \| stackcollapse-perf.pl \| flamegraph.pl > f.svg` |

> macOS 上 ASan/UBSan/TSan 都可用；`heaptrack`/`perf` 需 Linux（容器或远端机，见 `scripts/bootstrap.sh`）。

## 9. 纪律（和 AGENTS.md 一致）

- 一次只推一个里程碑，**M1 没全绿不碰 M2**
- 每个坑必须留下"工具原始输出"片段，禁止写"我修好了"这种结论
- 代码与文档分开提交：`feat(pipeline): ...` / `docs(pipeline): ...`
- 不为了炫技加功能（无网络、无序列化、无 DDS —— 那是阶段 C 的事）
