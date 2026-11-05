# mini-pipeline 需求规格（v2）

> 本文回答三个问题：**它是什么** · **工业界谁这么干** · **每一条需求怎么算通过**。
> 工作方式与里程碑见 [TASK.md](./TASK.md)；代码在 `src/`，用例在 `tests/`。

---

## 1. 它是什么（一句话）

> **一个进程内的"零拷贝数据通路"内核**：用引用计数句柄传递内存块，用有界通道在阶段之间移动数据，
> 用明确的背压与取消语义保证「不丢、不涨、不死锁」，并用可测量的延迟分位数证明它。

拆开是五个机制（也正是 `cpp/` 九个专题的落点）：

| 机制 | 一句话 | 关键不变量 |
|---|---|---|
| 块池（pool） | 构造时一次性分配，运行期零 malloc | 借出/归还 O(1)，不产生新的系统调用 |
| 句柄（handle） | `<=16B`，拷贝即共享，最后一个析构才归还 | 无 double-free、无悬垂、`use_count` 精确 |
| 通道（channel） | 有界、可关闭、带背压策略 | 满时**不会静默丢**；关闭后语义明确 |
| 阶段（stage） | 处理单元，可多态/类型擦除 | 不持有别人的句柄所有权（只借用） |
| 运行时（runtime） | 启动 → 运行 → 取消 → 排空 → 关闭 | 关闭**不丢在途数据**，且不死锁 |

## 2. 工业界谁这么干（这不是玩具）

| 你项目里的机制 | 真实系统 | 对应物 |
|---|---|---|
| 块池 + 引用计数块 | **iceoryx**（Eclipse/Apex.AI，AUTOSAR 生态广泛使用） | `ChunkHeader` + `ChunkManagement`，借出/归还与引用计数 |
| 同上 | **DPDK** | `rte_mempool` + `rte_mbuf`（`refcnt`），批量 alloc/free |
| 同上 | **Linux 内核** | `sk_buff` 的 `users` 引用计数、`kmem_cache` 池 |
| 同上 | **ROS 2 零拷贝** | `rmw_borrow_loaned_message`（配合 iceoryx 共享内存） |
| 零拷贝数据通路 | **Apollo Cyber RT** | 进程内/共享内存 transport，避免序列化 |
| 同上 | **ROS 2 intra-process** | 同进程节点间跳过序列化（`intra-process manager`） |
| 有界通道 + 背压 | **LMAX Disruptor** | ring buffer + gating，填充缓存行避免伪共享 |
| 同上 | **io_uring** | SQ/CQ 双环 + 内存屏障，用户态无锁提交 |
| 同上 | **DDS QoS** | `History{depth}`、`Reliability`、`Deadline` 决定"满了怎么办" |
| 阶段/流水线抽象 | **GStreamer / MediaPipe** | element / calculator 的 pad 与帧流水线 |
| 取消与优雅关闭 | **AUTOSAR AP EM/SM/PHM** | 进程启停、watchdog 超时、状态机迁移 |
| 同上 | **systemd** | `After=`/`Requires=` 依赖 + `WatchdogSec` + `RestartSec` 退避 |
| 同上 | **Kubernetes** | `SIGTERM` → `terminationGracePeriodSeconds` → `SIGKILL` |
| 延迟分位数 | **HdrHistogram / OpenTelemetry** | p50/p99/p999 直方图，而非平均值 |

> 结论：**你把这几件事做对，就已经是一个「中间件进程内快通路」的最小实现**，
> 和 iceoryx / Cyber RT / ROS 2 intra-process 做的是同一类事，只是范围收在单进程、单机。

## 3. 范围

**In（要做）**：单进程、多线程；定长块；有界通道；三种背压策略；可取消；优雅关闭；延迟/吞吐可测量。
**Out（明确不做）**：网络传输、序列化/IDL、服务发现、跨进程共享内存（→ 阶段 C 的 `mini-ipc`）、
持久化、动态扩容、TLS/鉴权、跨语言绑定。

## 4. 功能需求

> 编号规则：`R*` 功能，`N*` 非功能。每条都必须有对应用例；用例编号见 `tests/`。

### M1 · 池与句柄

| ID | 需求 | 不变量 / 语义 | 对应用例 |
|---|---|---|---|
| R1 | `BufferPool(block_size, count)` 一次性分配 `block_size*count` 连续内存 | 不得在 `acquire` 路径上分配内存（`mallopt`/hook 可验证） | `AcquireBasic` |
| R2 | `Buffer` 拷贝 = 共享块 + 引用计数 +1；`move` = 转移且源置空 | 多线程拷贝安全（原子）；`sizeof(Buffer) <= 16` | `CopySharesBlock` `MoveLeavesEmpty` `HandleSizeFitsIn16Bytes` |
| R3 | 最后一个句柄析构/reset 时块归还池 | 归还后 `stats().in_use` 减 1；不允许 double-free | `ResetReturnsBlock` `NoLeakSingleThread` |
| R4 | 池耗尽语义**二选一并写进文档**：返回空句柄 / 抛 `std::bad_alloc` | 不允许返回"野指针"或静默复用在用块 | `PoolExhaustionIsDefined` |
| R5 | `data()` 对齐到 `alignof(std::max_align_t)`；不同块不重叠 | 块区大小 ≥ `block_size`，无别名 | `DataIsAligned` `BlocksDoNotAlias` |
| R6 | 空句柄所有操作安全（`data()==nullptr`、`size()==0`、`reset()` 无副作用） | 默认构造、move 后、reset 后 | `EmptyHandleIsSafe` |
| R7 | `stats()` 提供 `in_use` / `peak` / `capacity` | `peak` 单调不减；并发读不崩 | `StatsPeak` |

### M2 · 通道与背压

| ID | 需求 | 不变量 / 语义 |
|---|---|---|
| R8 | `BoundedChannel<T>(cap, policy)`：`push` 满时不阻塞失败/阻塞/淘汰旧/淘汰新 | 任何策略下都**不允许静默丢**：`push` 返回布尔或计数 |
| R9 | `pop` 在"已关闭且空"时返回 false；未关闭且空时阻塞或返回 false（按配置） | 不允许在关闭后仍永久阻塞消费者 |
| R10 | `close()` 幂等；关闭后 `push` 立即失败并唤醒所有等待者 | 不能出现"生产者睡了消费者也睡了"的丢唤醒 |
| R11 | 三种实现（`Mutex`/`Spin`/`SPSC ring`）语义一致，用同一套用例跑 | 语义一致性 > 性能；性能差异写进 `docs/M2` |

### M3 · 阶段抽象

| ID | 需求 | 不变量 |
|---|---|---|
| R12 | `Stage::process(Buffer) -> Buffer` 三种实现（virtual / `std::function` / 类型擦除） | 三者可互换，同一套用例全过；`sizeof` 与分配次数有记录 |

### M4 · 运行时关闭

| ID | 需求 | 不变量 |
|---|---|---|
| R13 | 启动按依赖顺序；关闭按**逆序** | 不因顺序错误读已销毁对象 |
| R14 | `request_stop()` 后：不再接收新数据 → 排空在途 → 逐级 close → join | 在途数据 **0 丢失**（用计数校验） |
| R15 | 关闭必须幂等、可从任意状态调用、`join` 不死锁 | 用例：运行中关闭 / 空载关闭 / 重复关闭 |
| R16 | 看门狗：阶段超过 `deadline` 未被 kill，但**必须报告**（超时计数 + 最后进度） | 不误杀：只报告/计数，不静默丢数据 |

## 5. 非功能需求（必须用数字验收）

| ID | 指标 | 目标 | 测量方法 |
|---|---|---|---|
| N1 | `acquire()` 单线程 | < 30 ns | Release `-O2`，1e7 次取平均 + 直方图 |
| N2 | 端到端 1KB 消息，3 阶段流水线 | 吞吐与 **p99** 有数字，且 p999 可解释 | HdrHistogram 风格直方图，预热 ≥10 万条 |
| N3 | 8 线程压测 | 无泄漏、无 crash、`use_count` 正确 | TSan 干净 + 压测后 `in_use==0` |
| N4 | 内存上限 | 运行期堆分配次数 = 0（池外） | `heaptrack` / 自定义 `operator new` 计数 |
| N5 | 可取消延迟 | `request_stop()` 到全线程 join < 100 ms（空载） | 用例计时 |
| N6 | 关闭丢数据 | 0（在途 + 队列内全部处理完） | 发送计数 vs 处理计数 |

> **测量纪律**：Debug 构建的数字一律不采信；每个数字标明构建类型、CPU、线程数、数据量、预热策略。

## 6. 必须由你决定的三个设计（写进 `docs/ADR.md`，各 5 行）

1. **池耗尽返回空句柄还是抛异常**？理由（热路径可行性 vs 可诊断性）
2. **`Buffer` 能否跨线程传递**？如果控制块内联在块头，句柄里存什么？（决定 R2 的实现形态）
3. **通道满时的默认策略**？在 AD 场景下（传感器数据），DropOldest 与 Block 的取舍理由

> 这三条没有"标准答案"，但**必须能讲出取舍**——面试里这比背 vtable 值钱。

## 7. 与仓库其它部分的关系

```
阶段 A ──► M1（池+句柄）──► M2（通道）──► M3（阶段）──► M4（运行时）──► M5（构建+性能）
                                   │
                                   └─► 阶段 C：把 M1/M2 从"进程内"扩到"跨进程共享内存" = mini-ipc
```

- `cpp/01/02/04/05` → M1 ｜ `cpp/06/09` → M2/M4 ｜ `cpp/03/07` → M3 ｜ `cpp/08` → M5
- 阶段 C 的 `middleware/` 直接复用 M1/M2 的接口，只替换"传输层"（进程内 → 共享内存）
- 阶段 D 的性能报告，就以 N1/N2 的数字为基线做优化前后对比
