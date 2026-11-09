# PROGRESS — 进度与会话日志

> 每次收工在「会话日志」顶部追加一行（新的在最上面）。阶段状态在里程碑变化时更新。
> 计划详情见 [PLAN.md](./PLAN.md)；下一步见 [NEXT.md](./NEXT.md)。

## 阶段状态

| 阶段 | 周次 | 主题 | 状态 | 产物 |
|:--:|:--:|---|:--:|---|
| S0 | W1 | 仓库整顿 + 标准定稿 | ✅ 完成 | `AGENTS.md` `NEXT.md` `PROGRESS.md` `Makefile` `scripts/` `plans/` `interview/` `middleware/README.md` |
| A | W2-W6 | C++ 深水区 | 🚧 进行中（项目基建就绪，M1 未开工） | `project/mini-pipeline/`（REQUIREMENTS/PREREQ/LINUX-PREREQ/probe） |
| B | W7-W11 | Linux 系统层 | 📋 未开始 | - |
| C | W12-W17 | 中间件 | 📋 未开始 | 目标：mini-ipc |
| D | W18-W22 | 性能与确定性 | 📋 未开始 | 目标：性能分析报告 |
| E | W23-W26 | 系统设计 + 面试冲刺 | 📋 未开始 | 目标：30 题 ✅ |

## 会话日志

| 日期 | 阶段 | 做了什么 | 产物 | 下一步 |
|---|---|---|---|---|
| 2026-11-09 | A | mini-pipeline 需求规格（R1-R16/N1-N6）+ 工业界对照；准入自测；Linux 前置与 `make probe`；修 `new-plan.sh` BSD date 与 bash 中文变量名两个 bug | `project/mini-pipeline/{REQUIREMENTS,PREREQ,LINUX-PREREQ}.md` `scripts/linux-probe.sh` `8b4500b 3152ee1` | M1 闸门（atomic + ADR-002） |
| 2026-11-09 | A | 链表：实现 `swapPairs`(LC24) + `isPalindrome`(LC234)，`03_reverse` TODO 5→1 | `03_reverse.cpp` `cf215d0` | 实现 `reverseListRecursive` |
| 2026-11-06 | A | 链表：实现 `reverseList`(LC206) + `reverseBetween`(LC92) | `03_reverse.cpp` `5ff698d` | 同上 |
| 2026-11-05 | A | `detectCycle`（Floyd）实现 → `two_pointers_test` 转绿；`cpp/04` 循环引用 demo（weak 版）可运行 | `02_two_pointers.cpp` `cpp/04/src/mini_shared_ptr.cpp` `3581abc` | demo 补三组对照 + `reverse_test` 转绿 |
| 2026-11-05 | S0 | 每日计划系统（`plans/`、`make plan`、`new-plan.sh`）与 IDE 入口 | `plans/` `Makefile` `scripts/new-plan.sh` `7fd3c87` | - |
| 2026-11-04 | S0 | 建立跨设备/跨 agent 操作层：工作契约、下一步指针、进度日志、三个脚本 | `AGENTS.md` `NEXT.md` `PROGRESS.md` `scripts/*.sh` | 跑通 linked-list 测试 |
| 2026-11-04 | S0 | 面试题库 checklist（6 板块 39 题）；中间件层骨架；清理构建产物跟踪 | `interview/README.md` `middleware/README.md` | - |
| 2026-11-02 | S0 | 学习规划定稿：五阶段（中间件 + 性能与确定性为主线，AUTOSAR 不在范围） | `PLAN.md` `README.md` | - |
