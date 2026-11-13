# NEXT — 下一步

> **每次开工只看这个文件；每次收工只改这个文件。**
> 「现在做」只保留当前 **1 个单元**，完成即删，不留旧条目。
> 最后更新：2026-11-13 · 阶段 **A · P1（mini-pipeline M1 · 6 单元，已完成 0/6）**
> 路线：**P1 手写最小版 → P2 定向读 iceoryx → P3 RouDi-lite / 跨进程 demo 二选一**
> 今日计划：[plans/2026-11-13.md](./plans/2026-11-13.md) · 项目：[project/mini-pipeline/TASK.md](./project/mini-pipeline/TASK.md)

## 1. 现在做（唯一单元）

### U1 · 池底层内存 + 索引 free list

- **入口**：`project/mini-pipeline/src/buffer_pool.cpp` 的 `BufferPool` 构造、`acquire()`、`release()`
  接口契约见 `include/mini_pipeline/buffer_pool.hpp`
- **命令**：

  ```bash
  make test DIR=project/mini-pipeline
  cd project/mini-pipeline/build && ./buffer_pool_test \
    --gtest_filter='BufferPoolTest.AcquireBasic:BufferPoolTest.BlocksDoNotAlias'
  ```

- **验收**：`AcquireBasic`、`BlocksDoNotAlias` 变绿；`sizeof(mp::Buffer) <= 16`
- **降级**：60 分钟没绿就只保 `AcquireBasic` 绿并提交，`BlocksDoNotAlias` 推到 U2
- **必答一句**（写进当天复盘）：为什么 free list 存 `uint32` 索引而不是裸指针？

## 2. 下一个单元（不要提前做）

| 顺序 | 单元 | 验收 |
|:--:|---|---|
| U2 | acquire/release/对齐/池耗尽 | `ResetReturnsBlock` `DataIsAligned` `PoolExhaustionIsDefined` → 6/12 |
| U3 | 句柄 copy/move/空句柄 | `CopySharesBlock` `MoveLeavesSourceEmpty` `EmptyHandleIsSafe` |
| U4 | 原子计数 + 8 线程压测 | `ConcurrentAcquireRelease` `NoLeakSingleThread` |
| U5 | 压到 ≤16B + ASan/TSan | `HandleSizeFitsIn16Bytes` → 12/12 |
| U6 | 对照 iceoryx（写笔记，不写码） | 一页差异笔记 |

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

- 暂无（`cpp/06-concurrency` 已收尾，`448eb5b`）

## 6. 会话历史

最近 3 条见 `PROGRESS.md` 顶部；完整流水 `git log --oneline -20`。
