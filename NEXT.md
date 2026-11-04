# NEXT — 下一步

> **每次开工只看这个文件；每次收工只改这个文件。**
> 规则：「现在做」最多 3 条，每条必须带验收命令；完成后删除，不要留旧条目。
> 最后更新：2026-11-04 · 阶段 **S0（W1 仓库整顿）**
> 明日执行清单（含遗留问题）：**[plans/2026-11-05.md](./plans/2026-11-05.md)** · 查看命令 `make plan`

## 1. 现在做

- [ ] **把在飞的 linked-list 测试落袋**
  ```bash
  cd algorithms/src/02-linked-list
  cmake -S . -B build -DFETCHCONTENT_SOURCE_DIR_GOOGLETEST=$PWD/../01-array-string/build/_deps/googletest-src
  cmake --build build -j8 && cd build && ctest --output-on-failure
  ```
  验收：5 个测试目标全编译、ctest 全绿（若有 RED，只修编译错误 + TOP-1 用例，其余记入 `tests/README.md` 已知债务）
  完成标志：`git commit -m "feat(linked-list): ..."`

- [ ] **`cpp/04-smart-pointers` 的"看见现象"demo（60 行）**
  文件：`cpp/04-smart-pointers/src/mini_shared_ptr.cpp`
  验收：输出能一眼看出 4 个事实 —— `sizeof` 16 字节 / `use_count` 变化 / 循环引用泄漏 / `weak_ptr` 破环
  完成标志：输出贴进 `cpp/04-smart-pointers/README.md` 题目表 + commit

- [ ] **写 `cpp/04-smart-pointers/docs/Q001_shared_ptr控制块.md`**
  参照：`cpp/01-object-lifetime/docs/Q001_RAII.md` 的结构（一句话总结 → 原理表/图 → 代码 → 3 个面试追问）
  验收：≥150 行；能白板画出控制块布局；`interview/README.md` 的 C03/C04 标 `🟡`
  完成标志：commit + 在 `PROGRESS.md` 记一行

## 2. 排队（按顺序取）

- `cpp/06-concurrency`：`memory_order` 六种语义实验 + false sharing 微基准（阶段 A）
- `linux/08-build`、`linux/09-debug`：提到第 0 层先学（`perf`/`strace`/`gdb` 基本盘）
- `01-array-string` 的 16 个 RED 用例（或先给 `add_single_test` 加 `EXISTS` 守卫）
- 从工作里挑 1 个 EM 真实 case 写成 Q 文档（启动时序 / 资源超限 / 进程重启 选一）
- 建 `middleware/.github/skills/middleware-topic/SKILL.md`（照搬算法 skill 结构）

## 3. 阻塞 / 待决策

- 无

## 4. 会话历史指针

最近进展见 `PROGRESS.md` 顶部 3 条；完整流水见 `git log --oneline -20`。
