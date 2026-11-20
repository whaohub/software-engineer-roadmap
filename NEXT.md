# NEXT — 下一步

> **每次开工只看这个文件；每次收工只改这个文件。**
> 「现在做」只保留当前 **1 个单元**，完成即删，不留旧条目。
> 最后更新：2026-11-20 · 阶段 **A · P1（mini-pipeline M1 · 6 单元，已完成 5/6，U6 待做）**
> 路线：**P1 手写最小版 → P2 定向读 iceoryx → P3 RouDi-lite / 跨进程 demo 二选一**
> 今日计划：[plans/2026-11-13.md](./plans/2026-11-13.md) · 项目：[project/mini-pipeline/TASK.md](./project/mini-pipeline/TASK.md)

## 1. 现在做（唯一单元）

### U6 · 对照 iceoryx 写一页差异笔记（只读，不写码）

- **入口**：iceoryx `chunk_header.hpp` → `chunk_management.hpp` → `shared_chunk.hpp`
- **验收**：一页笔记，至少回答与当前实现的 3 个差异：控制块放哪、free list 什么结构、引用计数存哪
- **边界**：不改 `project/mini-pipeline/src` 与 `tests`

## 2. M1 现状（U1–U5 已完成）

| 单元 | 结果 |
|:--:|---|
| U1 池底层 + 索引 free list | ✅ 通过 |
| U2 acquire/release/对齐/池耗尽 | ✅ 通过 |
| U3 句柄 copy/move/空句柄 | ✅ 通过（补了「共享持有者 = 空句柄」用例） |
| U4 原子计数 + 8 线程压测 | ✅ 通过（TSan 0 data race） |
| U5 ≤16B + ASan/TSan | ✅ 通过（sizeof=16，ASan/UBSan 干净） |

**M1 收尾前还欠（不算完成）**

- [ ] **N1 未达标**：`acquire()` 实测 ≈34.5 ns（Release -O2，1e7 次，减基线）> 30 ns；需优化（原子 free-list / 自旋锁 / 线程本地缓存）或重新论证后留数据
- [ ] `docs/M1-句柄与内存池.md` 未写：需要 ASan/TSan/probe 原始片段 + 4 个「为什么」
- [ ] README 的 M1 验收清单未勾选

## 3. 排队（P1 结束前一律不动）

- **P2 定向读 iceoryx**（只读头文件）：`chunk_header.hpp` → `chunk_management.hpp` → `shared_chunk.hpp` → `mem_pool.hpp` → `loffli.hpp` → `chunk_queue_popper.hpp`
- **P3 二选一**：RouDi-lite（跨进程 shm + 队列 + 崩溃清理）或跨进程 chunk 传递 demo
- M2 通道与背压：**只有 U6 复盘确认面试需要时才开**
- 工作案例 1 篇（EM：启动时序 / 资源超限 / 崩溃重启，选一个）
- `interview/README.md` 只登记**练过**的题

## 4. 本阶段明确不做

- ❌ M3 阶段抽象（virtual / std::function / 类型擦除对比）
- ❌ M5 构建、LTO、ODR（改在真实仓库顺手学）
- ❌ M4 完整运行时（P3 之后再定）
- ❌ 新建专题目录；不并行推进 algorithms / middleware
- ❌ 通读 iceoryx 或从 RouDi 开始读

## 5. 阻塞 / 待决策

- **N1 性能指标**：`acquire()` ≈34.5 ns > 30 ns（Release -O2，1e7 次取平均并减基线）。主因是每块一次 `std::mutex` 加解锁 + `vector::pop_back`。待决策：优化（原子 free-list / 自旋锁 / 线程本地缓存）还是重新论证指标。
- **M1 复盘文档缺口**：`docs/M1-句柄与内存池.md` 尚未创建；现有证据（39 用例全绿、ASan/UBSan 与 TSan 干净、probe RSS 6 MB、sizeof=16）需落盘。
- **环境**：本机跑 ASan 需 `ASAN_OPTIONS=verify_asan_link_order=0`；`make probe` 的 perf 段因内核工具缺失跳过（不算失败）。
- 无其它阻塞。`cpp/06-concurrency` 的 demo 骨架已清理，回到 HEAD 状态。

## 6. 会话历史

最近 3 条见 `PROGRESS.md` 顶部；完整流水 `git log --oneline -20`。
