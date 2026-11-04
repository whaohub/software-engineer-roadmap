# AGENTS.md — 工作契约

> 本文件对**任何 AI agent（Claude Code / Codex / Cursor / Copilot / Gemini CLI …）和人类协作者**同时生效。
> 它是唯一的"怎么干活"事实源；`PLAN.md` 是"做什么"，`NEXT.md` 是"现在做哪一步"，`PROGRESS.md` 是"做到哪了"。

## 0. 30 秒上手（新机器 / 新 agent / 新会话）

```bash
./scripts/status.sh     # 一行命令拿到全部上下文
```

只读它输出的文件，**不要通读整个仓库**（上下文预算有限，仓库会越来越大）。
需要动手时按顺序读：`NEXT.md` → `PROGRESS.md` 最近 3 条 → 该专题的 README/现有文档。

## 1. 项目地图

| 目录 | 内容 | 状态 |
|---|---|---|
| `cpp/` | C++ 语言层专题（`NN-topic/{docs,src}`） | 1/9 有内容 |
| `linux/` | Linux 系统层专题（`NN-topic/{docs,src,demo}`） | 1/9 有内容 |
| `algorithms/src/` | LeetCode 专题（CMake + gtest，基建最完整） | 2/10 有内容 |
| `middleware/` | 中间件层（阶段 C 主线，骨架已建） | 待开始 |
| `interview/` | 面试题库 checklist（39 题） | 跟踪中 |
| `PLAN.md` | 五阶段计划、四件套标准、AI 时代重点 | 事实源 |
| `NEXT.md` | **唯一**的"下一步"指针 | 每次会话更新 |
| `PROGRESS.md` | 阶段状态 + 会话日志 | 每次会话追加 |
| `scripts/` | `status.sh` / `check.sh` / `bootstrap.sh` | 可执行入口 |

## 2. 铁律（违反即返工）

**Do**
- 一次只推进**一个**专题；完成按"四件套"判定：文档 + 可运行 demo + **实测数据** + 自测卡
- 任何结论都要能指向证据：代码、测试、benchmark、`perf`/`strace` 输出或图
- 新文档先照最近的同类范文结构写（C++ → `cpp/01-object-lifetime/docs/Q001_RAII.md`；Linux → `linux/01-process/docs/Q002_fork流程.md`；算法 → 该专题 README）
- 代码改动必须能编译 + 测试通过；`-Wall -Wextra` 零新增告警
- 每次会话结束：更新 `NEXT.md` + `PROGRESS.md`，并 commit

**Don't**
- ❌ 不新建空专题目录（当前 28 个专题只有 4 个有内容，广度已经透支）
- ❌ 不提交 `build/` 产物、二进制、`.DS_Store`、密钥
- ❌ 不擅自 `git push`、不改写历史、不 stash/discard 他人未提交的改动
- ❌ 不写没有验证过的"结论"（尤其性能数字）
- ❌ 不在文档里写本机绝对路径（`/Users/xxx`），一律用仓库相对路径

## 3. 会话协议（保证跨 agent / 跨机器连续）

**开工（≤1 分钟）**
1. `./scripts/status.sh`
2. 看 `git status`：**有未提交改动先收尾它**，不要开新战场
3. 从 `NEXT.md` 取"现在做"的第一条，它就是本次会话唯一产出

**收工（≤3 分钟）**
1. `./scripts/check.sh` 必须通过（或明确记录失败原因）
2. 更新 `NEXT.md`：删掉已完成项，补充后续（**最多 3 条**，必须带验收命令）
3. `PROGRESS.md` 顶部追加一行：`日期 | 阶段 | 做了什么 | 产物路径 | 下一步`
4. `git add -A && git commit`（见第 5 节）

## 4. 验收命令（任何 agent 都要能跑）

```bash
./scripts/check.sh        # 卫生 + 死链 + 四件套矩阵 + 密钥扫描
./scripts/status.sh       # 上下文全景

# 算法专题（唯一有测试基建的层）
cd algorithms/src/<NN-topic>
cmake -S . -B build -DFETCHCONTENT_SOURCE_DIR_GOOGLETEST=$PWD/../01-array-string/build/_deps/googletest-src
cmake --build build -j8 && cd build && ctest --output-on-failure

# C++ / Linux 专题（无 gtest，用最小可执行 + assert）
g++ -std=c++17 -Wall -Wextra -O0 -g src/<file>.cpp -o /tmp/demo && /tmp/demo
```

> 新机器先跑 `./scripts/bootstrap.sh` 检查工具链（cmake ≥ 3.14、C++17 编译器、git、可选 python3）。

## 5. 提交规范

```
<type>(<scope>): <中文摘要>

type: feat | fix | docs | test | refactor | chore
scope: cpp/04 | linux/03 | middleware/02 | algorithms/02 | plan | scripts
```

例：`feat(cpp/04): 手写 shared_ptr 控制块 + 循环引用 demo`
`git log --oneline` 就是进度流；不要在 message 里写"WIP 提交"。

## 6. 各层专属约定

| 层 | 约定文件 | 要点 |
|---|---|---|
| `algorithms/` | `algorithms/.github/skills/algorithms-topic-authoring/SKILL.md` | 注释块顺序、TODO 桩、测试矩阵、CMake 注册 |
| `cpp/` | 参照 `cpp/01-object-lifetime/`（`docs/Qxxx_主题.md` + `src/` + CMakeLists） | 每题一个 `.cpp`，文件头写题目/难度/标签/要点 |
| `linux/` | 参照 `linux/01-process/`（`docs/Qxxx_*.md` + `demo/`） | 文档 200~300 行，含表/图/代码/一句话总结 |
| `middleware/` | `middleware/README.md` | 7 个主题 + mini-ipc 验收标准（p50/p99 数字硬指标） |
| `interview/` | `interview/README.md` | 只登记**练过**的题；复述日期必填 |

## 7. 给 agent 的固定提示词（可复制）

> 先跑 `./scripts/status.sh`，然后只按 `NEXT.md` 的"现在做"推进一件事。
> 遵守 `AGENTS.md` 的铁律：不新建空专题、不提交构建产物、结论必须有证据。
> 完成后：跑 `./scripts/check.sh`，更新 `NEXT.md` + `PROGRESS.md`，按规范提交。
> 如果发现工作区有未提交改动且不属于本次任务，先停下来问我。
