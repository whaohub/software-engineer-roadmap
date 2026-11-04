# 学习规划（2026Q4 — 2027Q2，26 周）

> 面向方向：**C++ 自动驾驶中间件**（工作背景含 EM / 执行管理）
> 范围约定：**AUTOSAR 规范暂不投入学习时间**——EM 是你已有的工作经验，转为「案例整理 + 口头表达」，不再读书；
> 学习时间全部投向 **C++ 深水区、Linux、中间件、性能与确定性**。
> 目标：既能干活（性能/稳定性/确定性），也能面试（原理 + 系统设计 + 排障）
> 关联：[issue-workflow.md](./help/issue-workflow.md) · [算法专题规范](./algorithms/.github/skills/algorithms-topic-authoring/SKILL.md)

---

## 0. 一句话诊断

**当前的问题不是「缺方向」，而是「广度透支」**：
目录已经铺到 28 个专题（cpp 9 + linux 9 + algorithms 10），真正有实质内容的只有 **4 个（14%）**，
其余 24 个是 14~20 行的占位 README（另有 `linux/apue`、`linux/assembly` 两个杂项目录）。
继续横向铺目录只会加重"每次打开都不知道该做什么"的负担。

结论：**停止新建专题，改为串行深挖 + 每层一个可运行作品**。

## 0.1 现状数据（2026-11-02）

| 层 | 专题数 | 有内容 | 内容完成度 | 说明 |
|---|:---:|:---:|:---:|---|
| `cpp/` | 9 | 1 | 11% | `01-object-lifetime` 有 3 个 src + 2 篇 Q 文档，质量不错 |
| `linux/` | 9 | 1 | 11% | `01-process` 有 3 篇 Q 文档（200~300 行）+ mini_shell |
| `algorithms/` | 10 | 2 | 20% | `01/02` 有实现 + gtest，测试基建是三者中最好的 |
| `help/` `docs/` | - | - | - | issue 工作流 + 算法 skill 已成型（AI 协作已验证可行） |

已具备的可复用资产（这是最大优势，别推倒重来）：

- ✅ **文档标准**：`Qxxx_主题.md` = 一句话总结 → 原理表 → 代码 → 面试追问
- ✅ **测试基建**：CMake + gtest，`add_single_test` 模式
- ✅ **Agent 规范**：`algorithms/.github/skills/*/SKILL.md`（把约定写成 AI 可执行的规范）
- ✅ **Issue 驱动**：每日计划 → 3 类 issue（learning / documentation / demo）

技术债（10 分钟内可清）：

- ⚠️ `cpp/01-object-lifetime/build/` 46 个 CMake 产物已入库（22MB 级的同类目录被 .gitignore 挡住了，但这个早于 ignore 就提交了）
- ⚠️ `01-array-string` 有 16 个 RED 用例 + 缺失 `03_prefix_sum_test.cpp`（skill 里已记录）
- ⚠️ `cpp/08-compiler-link`、`linux/08-build`、`linux/09-debug` 是"工具层"，但排在最后 —— 与实际使用顺序相反

---

## 0.2 操作层（跨设备 / 跨 agent 连续工作）

计划本身解决"做什么"，下面这套文件解决"**换电脑、换 agent 怎么接着做**"——
所有状态都在 git 里，不依赖任何工具的会话记忆：

| 文件 | 职责 | 谁更新 | 何时 |
|---|---|---|---|
| `AGENTS.md` | 工作契约（铁律 / 会话协议 / 验收命令 / 提交规范） | 人 | 规则变化时 |
| `NEXT.md` | 唯一「下一步」，最多 3 条，每条带验收命令 | agent 或人 | **每次收工** |
| `PROGRESS.md` | 阶段状态表 + 会话日志（日期/阶段/产物/下一步） | agent 或人 | **每次收工** |
| `scripts/status.sh` | 一条命令还原全部上下文 | - | 每次开工 |
| `scripts/check.sh` | 机器可执行验收（卫生/死链/四件套矩阵/密钥） | - | 每次收工 |
| `scripts/bootstrap.sh` | 新机器工具链自检 | - | 换机器时 |
| `CLAUDE.md` / `.github/copilot-instructions.md` | 各工具的一行指针 → `AGENTS.md` | 人 | 换工具时 |

**三条规矩**
1. 开工只跑 `./scripts/status.sh`，不读全仓（上下文预算）；
2. 收工必须让 `NEXT.md` + `PROGRESS.md` + `git log` 三者一致；
3. 有未提交改动时**先收尾**，不开新战场——这是跨 agent 冲突的唯一根源。

---

## 1. 目标架构（分层与落点）

```
第 0 层 工具链       git / CMake / gdb / perf / strace / bpftrace     → linux/08-build, 09-debug（提前）
第 1 层 语言         C++17/20 深水区、内存模型、并发原语              → cpp/（现状已铺，需收敛）
第 2 层 系统         Linux 进程/线程/内存/网络/epoll/IPC              → linux/
第 3 层 中间件       DDS / SOME-IP / 共享内存零拷贝 / 服务发现 / QoS  → middleware/  ★已建骨架
第 4 层 性能与确定性  perf/bpftrace、延迟分解、抖动、调度与 CPU 亲和   → perf/        ★新增
第 5 层 作品与面试   mini-ipc、性能报告、系统设计、排障复盘            → project/  interview/  ★新增
横向    AI 工程能力   agent 流水线 / 性能工程 / 端侧推理 / 可观测性    → ai/
可选延伸 执行管理(EM)  进程编排 / 资源预算 / 看门狗 / 故障恢复          → runtime/  （想补作品再做）
```

- 中间件 README 已就位（[middleware/](./middleware/README.md)），其余章节在对应阶段开始时再建，避免又变成空骨架。
- **执行管理**相关的通用能力（进程生命周期、资源隔离、故障恢复）保留在面试板块，
  以「工作案例 + 可选 mini-runtime 作品」的方式准备，**不读 AUTOSAR 规范**。

---

## 2. 三条主线与权重

| 主线 | 权重 | 产出 | 衡量标准 |
|------|:---:|------|----------|
| **A. 深度（工作能用）** | 50% | middleware / perf 的专题 + 可运行 demo | 能在工作中解释并改进一处真实实现（延迟、抖动、故障恢复） |
| **B. 面试（能讲清）** | 30% | `interview/` 题库 + 30 天冲刺 | 任意一题能在 3 分钟内讲清原理 + 画图 + 说清陷阱 |
| **C. AI 时代能力** | 20% | `ai/` 规范 + 工具链 | 用 AI 完成"读陌生大仓库 / 定位性能问题 / 生成可测代码"三类任务 |

> 反直觉结论：**AI 越强，"性能分析、并发正确性、故障归因、跨层系统设计"越值钱**。
> 这几件事的共同点是——**能被验证**，而 AI 生成的代码最难的自证也在这里。

---

## 3. 五阶段计划（+ 一个可选延伸）

- **稳速档**：每周 4h（工作日 2×1h + 周末 2h）
- **冲刺档**：每周 8-10h（面试前 2 个月切这档）

| 阶段 | 周次 | 主题 | 核心产出（必须可运行/可复述） |
|:---:|:---:|---|---|
| **S0** | W1 | 仓库整顿 + 标准定稿 | 清 build 产物；四件套交付标准；把 `cpp/04-smart-pointers` 做成样板 |
| **A** | W2-W6 | C++ 深水区 | 手写 shared_ptr/weak_ptr/控制块、内存序六种语义实验、false sharing 微基准、allocator/对象池、完美转发与 concepts |
| **B** | W7-W11 | Linux 系统层 | 线程池、mmap/COW/malloc 行为实测、shm+eventfd 对比、epoll Reactor |
| **C** | W12-W17 | 中间件 | 通信模型 → 序列化 → 共享内存零拷贝 → DDS/SOME-IP 对照 → **作品 1：mini-ipc**（p99 延迟对比 UDP） |
| **D** | W18-W22 | 性能与确定性 | perf 火焰图 + bpftrace + 延迟直方图；对 mini-ipc 与一个真实业务进程做延迟分解与抖动归因，给出优化前后数据 → **作品 2：性能分析报告** |
| **E** | W23-W26 | 系统设计 + 面试冲刺 | 5 个设计方案（传感器分发/OTA/日志回传/跨域通信/时间同步）+ 30 题自测 + 3 次录音复盘 |
| 可选 | 随时 | 执行管理实践 | **mini-runtime**：读配置启动/停机、依赖顺序、cgroup+affinity 限资源、看门狗与退避重启（对标 systemd，不涉及 AUTOSAR 规范） |

> 为什么把「性能与确定性」单独成阶段：它是中间件与执行管理两条线的**共同验收标准**，
> 也是 AI 时代最难被替代、面试最能拉开差距的部分。没有这一层，前三个阶段只是「知道」，不是「会做」。

### 每周节奏（稳速档可直接照抄）

| 时间 | 动作 | 产出 |
|------|------|------|
| Mon（30min） | 读 1 段真实源码（iceoryx / CycloneDDS / AP 规范任意一节） | 5 行笔记 + 3 个自问 |
| Tue-Wed（2h） | 写 demo + 测试 | 可编译运行的 `src/` + gtest |
| Thu（30min） | 测数据：benchmark / strace / perf / lat / 截图 | `docs/` 里一张数据表 |
| Fri（1h） | 写 1 篇 `Qxxx_*.md` | 1 篇 Q 文档 + 3 张自测卡 |
| 周末（1h） | 复盘 + 1 个 commit | `git log` 不断链 |

**唯一硬指标：每周至少 1 个 commit。** 断链两周以上，先把节奏恢复再谈内容。

---

## 4. 交付标准（每个专题的"四件套"）

1. **Q 文档**：一句话总结 → 原理（表格/图）→ 代码 → **3 个面试追问 + 陷阱**
2. **可运行 demo**：`src/` + `tests/`，`-Wall -Wextra` 零新增告警
3. **数据**：benchmark 数字、`perf`/`strace`/`lat` 输出或内存布局图（**没有数据的"优化"不算完成**）
4. **自测卡 5 张 + 1 分钟口述稿**（对着手机录音讲一遍，卡壳的地方就是没懂的地方）

> 判据：能不看资料白板画出结构 + 说清为什么这样设计 + 指出 2 个失败模式。

---

## 5. AI 时代的重点（20% 精力，回报最高）

### 5.1 把 AI 变成"流水线"，而不是"答案机"

你已经有 `SKILL.md` 的成功经验，把它推广到每一层：

| 层 | 规范文件 | Agent 负责 | 你负责 |
|---|---|---|---|
| `middleware/` | `middleware/.github/skills/middleware-topic/SKILL.md`（照搬算法那一套） | 生成骨架、CMake、gtest 用例矩阵、README 表格同步 | 设计取舍、性能数据、失败模式 |
| `perf/` | 同上 | 生成压测脚本、火焰图汇总表、回归对比 | 归因、取舍、结论 |
| `interview/` | `interview/.github/skills/mock-interview/SKILL.md` | 出题、追问、按 rubric 打分 | 口述、复盘、补短板 |

要点：**规范写得越"可执行"，AI 产出越稳定**（这就是当前的差异化能力）。

### 5.2 五项要深挖的能力

1. **性能工程**（最高优先级）
   `perf record/report`、`pprof` 替代品、火焰图、`bpftrace`、ftrace、延迟直方图（p50/p99/p999）。
   → 交付物：能对任意一个进程说出"时间花在哪、为什么抖动"。
2. **并发正确性与内存模型**
   `memory_order` 六种语义的实测程序、TSan/ASan 实战、无锁队列的 ABA 与内存回收（hazard pointer / epoch）。
   → 这是 AI 最容易写错、也最容易面试挂的地方。
3. **零拷贝与数据通路**
   共享内存生命周期（谁释放、进程崩溃怎么办）、内存池、序列化选择（CDR vs FlatBuffers）、
   从传感器到推理的 memcpy 次数账本。
4. **端侧推理落地**（AD 特有，但别去训模型）
   ONNX Runtime / TensorRT 集成、INT8/FP16 量化精度与延迟权衡、异步推理与流水线、
   **把 tensor 通过共享内存零拷贝传给下游**、推理耗时的抖动纳入调度预算。
5. **可观测性**
   结构化日志、metrics、trace（OpenTelemetry），在中间件里内置而不是事后加。

### 5.3 明确"不要学"

- ❌ 从零训练大模型 / 调 RL 算法（投入产出比极低，岗位也不需要）
- ❌ 刷算法题难度（维护手感即可，你的主战场不在那里）
- ❌ 追框架 API（DDS 厂商 API 一周就变，规范与模型十年不变）
- ❌ 继续横向铺新专题目录

---

## 6. 面试映射（目标岗位：C++ 中间件 / 基础设施 / AD 平台）

| 板块 | 高频题（必须能白板讲 + 有 demo 或数据支撑） |
|---|---|
| C++ | 移动语义与完美转发；虚函数表与对象模型；`shared_ptr` 控制块与 `weak_ptr` 破环；`enable_shared_from_this` 陷阱；模板特化/CRTP/concepts；协程状态机；`memory_order` 与 `atomic`；allocator 与小对象池；UB/严格别名；ODR 与链接 |
| 系统 | 进程 vs 线程开销；调度与优先级反转；`malloc` 与 `mmap`/`brk`/page cache；COW；fd 与 inode；epoll LT/ET 与惊群；IPC 选型矩阵（pipe/shm/eventfd/socket/mq） |
| 中间件 | DDS vs SOME-IP vs MQTT vs gRPC 取舍；QoS（reliability/deadline/liveliness）如何影响时延；服务发现与动态拓扑；丢包/乱序/背压；IDL 演进与兼容性；零拷贝的边界条件 |
| 执行管理（工作强项，转为案例） | 进程生命周期与启动时序；资源预算与隔离（cgroup/affinity/nice）；看门狗与健康监控；崩溃重启与退避；确定性调度 |
| 系统设计 | 传感器数据分发架构；OTA 升级流程；车端日志回传；跨域通信；时间同步（PTP/gPTP） |
| 排障 | 内存泄漏（`heaptrack`/`valgrind`/`/proc`）；CPU 100%（`perf top` + 火焰图）；延迟抖动（`bpftrace` + 直方图）；死锁（`gdb` + core + 锁顺序图） |

> 每个板块在 `interview/` 下建一个 `Qxxx` 文件，只写你**练过**的题；没练过的不写（避免自欺）。

---

## 7. Sprint 0（本周就能做完，1-2 小时）

- [x] 清理入库构建产物：`git rm -r --cached cpp/01-object-lifetime/build`（已取消跟踪，待提交）
- [ ] 把 `linux/08-build`、`linux/09-debug` 的优先级提到"第 0 层"（先会调试再学系统）
- [ ] 选 **`cpp/04-smart-pointers`** 做样板专题（与现有 `01` 承接，面试最高频）
      → 手写 `shared_ptr`/`weak_ptr`/控制块 + 循环引用 demo + `sizeof` 与线程安全说明 + 3 个追问
- [ ] 修掉 `01-array-string` 的 16 个 RED 用例（或先给 `add_single_test` 加 `EXISTS` 守卫）
- [x] 建 `interview/README.md`：6 个板块 checklist（C++/系统/中间件/执行管理/系统设计/排障，共 39 题）
- [ ] 从工作里挑 1 个 EM 真实 case 写成 Q 文档（启动时序 / 资源超限 / 进程重启 各选一）——工作强项最该被「固化」成可复述的材料
- [ ] 把 `middleware/.github/skills/middleware-topic/SKILL.md` 建起来（照搬算法 skill 结构）

---

## 8. 度量与风险

**月度自检（30 分钟）**

| 指标 | 目标 | 说明 |
|---|---|---|
| commit 周数 | ≥ 3/4 周 | 节奏比强度重要 |
| 完成专题 | 1-2 个/月 | 按"四件套"算完成 |
| 数据型文档 | ≥ 2 篇/月 | 含 benchmark/实测的文档 |
| 口头复述 | ≥ 2 次/月 | 录音或讲给人听 |
| 面试题覆盖率 | 每阶段末 ≥ 60% | 按 `interview/` checklist |

**主要风险与对策**

| 风险 | 表现 | 对策 |
|---|---|---|
| 完美主义卡住 | 一篇 Q 文档写两天 | 限时 1h，先出 v1，标注 `TODO:` 后续补 |
| 只读不写 | 收藏夹越来越长 | 每个知识点必须落一份 demo 或数据 |
| 重复铺骨架 | 又新建 10 个空目录 | 新专题必须先把上一个专题四件套做完 |
| AI 依赖 | 看不懂自己 merge 的代码 | 规则：AI 生成的代码必须能逐行解释 + 有测试 |
| 工作挤占 | 连续两周 0 commit | 降级到"每周 1 个 30 分钟的最小产出" |
