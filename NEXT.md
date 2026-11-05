# NEXT — 下一步

> **每次开工只看这个文件；每次收工只改这个文件。**
> 规则：「现在做」最多 3 条，每条必须带验收命令；完成后删除，不要留旧条目。
> 最后更新：2026-11-05 · 阶段 **S0 ✅ → A（C++ 深水区）**
> 执行清单（含遗留问题）：**[plans/2026-11-06.md](./plans/2026-11-06.md)** · 查看命令 `make plan`

## 1. 现在做

> 完整版（含验收与时间盒）：[plans/2026-11-06.md](./plans/2026-11-06.md)

- [ ] **`cpp/04` demo 补成"有对比有数据"**（20min）
  文件：`cpp/04-smart-pointers/src/mini_shared_ptr.cpp`
  必须看到：`sizeof(shared_ptr)==16` / `use_count` 且标签正确 / 组1 双向 shared_ptr 无析构 / 组2 weak 正常析构
  验收：`make demo FILE=cpp/04-smart-pointers/src/mini_shared_ptr.cpp` 输出含两组对照；真实输出贴进 README

- [ ] **让 `reverse_test` 转绿（只实现 2 个函数）**（20min）
  文件：`algorithms/src/02-linked-list/03_reverse.cpp` → `reverseList`（LC206 迭代）、`swapPairs`（LC24）
  验收：`make test DIR=algorithms/src/02-linked-list` 中 `reverse_test` Passed；其余 4 个函数继续留桩并登记债务

- [ ] **收工**（10min）
  `make check` → 更新 `PROGRESS.md` + 本文件 + `plans/2026-11-06.md` 复盘 → commit（代码与文档分开提交）

## 2. 排队（按顺序取）

- `algorithms/src/02-linked-list`：剩余 23 个未实现函数（03:5 / 04:7 / 05:11），周末整块时间处理 `05_design`
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
