# Linux 前置：需要什么、什么时候需要、怎么用项目验证

> 结论先说：**写 M1 不需要任何 Linux 前置知识**（只用到 `<atomic>`/`<thread>`/`<vector>`）；
> 但**验收 M1 的非功能指标需要 Linux 工具**，而这恰恰是最好的 Linux 学习方式 ——
> 每个概念都必须由一条命令的输出证明，而不是读一遍文档。

## 1. 需要 / 不需要

| 阶段 | 需要 Linux 吗 | 说明 |
|---|:--:|---|
| **M1 编码** | ❌ 不需要 | 纯标准库：`std::atomic`、`std::thread`、`new/delete`；macOS 上就能写 |
| **M1 验收** | ✅ 需要（Linux） | 要证明"零系统调用 / 零堆分配 / 内存只增长一次"，只能用 `strace`、`/usr/bin/time -v`、`pmap` |
| **M2 编码** | 🟡 部分 | 标准库即可写；但"为什么阻塞唤醒这么慢"要 `strace -e futex` 才看得见 |
| **M4 编码** | ✅ 需要 | 信号处理、线程 join 死锁排查、`/proc` 观测 |
| **M5 / 阶段 D** | ✅ 需要 | `perf`、`sched`、`taskset`、cgroup —— macOS 上不存在 |

> macOS 上 `perf`/`strace`/`pmap` 都没有（`dtruss` 需 sudo 且不通用）。
> 用 `orbstack`/`docker` 起一个 Linux 容器，或者一台远端机 —— `scripts/bootstrap.sh` 会提示这一点。

## 2. 验证驱动的 Linux 学习（每个概念必须有一条命令的输出）

| Linux 概念 | 在项目里的触发点 | 验证命令（Linux） | 期望看到什么 | 对应文档 |
|---|---|---|---|---|
| 系统调用开销 | `acquire()` 设计成"零 syscall" | `strace -c -f -e trace=brk,mmap ./buffer_pool_test` | `brk/mmap` 次数**不随迭代数增长**（只有启动时几次） | `linux/01-process` ✅ 已有 |
| 堆 vs mmap 阈值 | 池一次性分配 >128KB 时 `malloc` 会走 `mmap` | `strace -e trace=brk,mmap` | 大池看到 `mmap`，小池看到 `brk` | `linux/03-memory` ⬜ 待写 |
| 虚拟内存与懒映射 | 池构造后 RSS 未必立刻上涨 | `/usr/bin/time -v`、`/proc/<pid>/status` 的 `VmRSS` | 首次触碰页才增长（page fault 计数） | `linux/03-memory` ⬜ |
| page fault 成本 | 首次 `acquire` 的延迟尖峰 | `perf stat -e page-faults,minor-faults` | 预热后 page-faults 不再增长（预热策略的依据） | `linux/03-memory` ⬜ |
| 线程与栈 | 8 线程压测 | `pmap <pid>` / `ulimit -s` | 每条线程栈默认 8MB（虚拟），RSS 远小于它 | `linux/02-thread` ⬜ |
| futex（条件变量底层） | M2 阻塞通道 | `strace -e futex` / `perf trace` | 每次阻塞/唤醒 = 一次 `futex(FUTEX_WAIT/FUTEX_WAKE)` | `linux/02-thread` ⬜ |
| 调度延迟 | M2 的 p99 抖动 | `perf sched latency` | 看 run-queue 等待时间而非 CPU 时间 | `linux/02-thread` ⬜ |
| CPU cache / 伪共享 | M2 队列索引与统计计数器 | `perf stat -e cache-misses,cache-references` | 分开填充缓存行后 miss 率明显下降 | `linux/09-debug` ⬜ |
| 信号与优雅退出 | M4 的 `SIGTERM` 排空 | `kill -TERM <pid>` + 计数 | 退出码与"发送数 == 处理数" | `linux/01-process` ✅ 已有 |
| 线程死锁定位 | M4 join 顺序错误 | `gdb -p <pid> -ex 'thread apply all bt'` | 看到互相等待的栈（谁持锁谁等锁） | `linux/09-debug` ⬜ |

> **纪律**：这些命令的输出要**贴进** `docs/M1-*.md` / `docs/M2-*.md`。
> 没贴输出 = 没学过 —— 和 `AGENTS.md` 第 2 条一致。

## 3. 所以 `linux/` 目录怎么用

**不要**先按 01→09 顺序读（你的历史数据：4 个月只填了 1/9）。
**改成**：项目撞到哪个概念 → 跑命令拿输出 → 顺手写成该专题的 `Qxxx` 文档。
好处是文档自带"为什么需要它"的真实上下文，比抽象阅读留存率高得多。

| 现在可以先写的（项目直接给素材） | 触发里程碑 |
|---|---|
| `linux/03-memory/docs/Q004_brk与mmap的分配阈值.md` | M1 |
| `linux/03-memory/docs/Q005_page_fault与懒分配.md` | M1 |
| `linux/02-thread/docs/Q004_条件变量与futex.md` | M2 |
| `linux/09-debug/docs/Q001_perf定位伪共享.md` | M2 |
| `linux/09-debug/docs/Q002_gdb定位join死锁.md` | M4 |

## 4. 一条命令跑完 M1 的验收

```bash
# Linux 上（或 Linux 容器内）
./project/mini-pipeline/scripts/linux-probe.sh project/mini-pipeline/build/buffer_pool_test
```

它会依次给出：最大 RSS（自动判定阈值）、`brk/mmap/munmap` 次数、page-fault 与 cache 统计，
并在 macOS 上直接告诉你该用什么容器命令。
