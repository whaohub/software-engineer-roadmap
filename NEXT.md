# NEXT — 下一步

> **每次开工只看这个文件；每次收工只改这个文件。**
> 规则：「现在做」最多 3 条，每条必须带验收命令；完成后删除，不要留旧条目。
> 最后更新：2026-11-09 · 阶段 **A（C++ 深水区 · 项目驱动）**
> 执行清单（含遗留问题）：**[plans/2026-11-10.md](./plans/2026-11-10.md)** · 查看命令 `make plan`

## 1. 现在做

> **冲刺日（4.5h）完整时间盒**：[plans/2026-11-10.md](./plans/2026-11-10.md)
> 纪律：每 60 分钟 commit 一次；一个 block 卡 20 分钟就换下一个。

- [ ] **B1 · `reverse_test` 收尾**（10min）
  `algorithms/src/02-linked-list/03_reverse.cpp` → `reverseListRecursive`
  验收：`make test DIR=algorithms/src/02-linked-list` → `reverse_test` Passed（3/5）

- [ ] **B2+B3 · 闸门与设计**（40min）
  `cpp/06-concurrency/src/refcount_atomic.cpp`（20 行 + TSan 干净）
  `project/mini-pipeline/docs/ADR.md`：ADR-002 定稿（含 `sizeof(Buffer)` 实际数字）+ ADR-001/003

- [ ] **B4+B5 · mini-pipeline M1 主体**（120min）
  池（连续内存 + 索引 free list）→ 句柄（copy/move/原子计数/≤16B）
  验收：`make test` **12/12 绿**；再跑 `test-asan` / `test-tsan`

- [ ] **B6+B7+B8 · 验收 + 文档 + 收工**（80min）
  `make probe`（需 Linux）→ `docs/M1-句柄与内存池.md` → `make check` + 更新 PROGRESS/NEXT + commit

## 2. 排队（按顺序取）

- **[project/mini-pipeline](./project/mini-pipeline/TASK.md) M2**：M1 全绿后再开（通道三实现 + 伪共享）
- `algorithms/src/02-linked-list`：剩余 19 个桩（03:1 / 04:7 / 05:11）；`05_design` 留周末整块时间
- `cpp/04-smart-pointers`：手写 shared_ptr 控制块（40 分钟连续时间）→ `docs/Q001_sharedptr控制块.md`
- `cpp/06-concurrency`：`memory_order` 六种语义实验 + false sharing 微基准
- `linux/08-build`、`linux/09-debug`：提到第 0 层先学（`perf`/`strace`/`gdb` 基本盘）
- `01-array-string` 的 16 个 RED 用例（或先给 `add_single_test` 加 `EXISTS` 守卫）
- 从工作里挑 1 个 EM 真实 case 写成 Q 文档（启动时序 / 资源超限 / 进程重启 选一）
- 建 `middleware/.github/skills/middleware-topic/SKILL.md`（照搬算法 skill 结构）

## 3. 阻塞 / 待决策

- 无

## 4. 会话历史指针

最近进展见 `PROGRESS.md` 顶部 3 条；完整流水见 `git log --oneline -20`。
