# mini-pipeline

面向 C++ 专题的**实战项目**：可取消、可观测的进程内数据流水线。

- 需求规格（R1-R16 功能 / N1-N6 指标 / 真实系统对照）：**[REQUIREMENTS.md](./REQUIREMENTS.md)**
- 任务书（里程碑、内置的坑、动手顺序）：**[TASK.md](./TASK.md)**
- 设计决策记录（池耗尽语义等 3 个必须你定的取舍）：**[docs/ADR.md](./docs/ADR.md)**
- 当前进度：**M1 · 句柄与内存池**（测试已写好，实现全是桩）

## 快速开始

```bash
# 从仓库根目录
make test DIR=project/mini-pipeline        # 单测（当前 12 个用例中大部分是红的）
make test-asan DIR=project/mini-pipeline   # ASan + UBSan
make test-tsan DIR=project/mini-pipeline   # TSan（数据竞争）

# 或者直接 cmake
cmake -S project/mini-pipeline -B project/mini-pipeline/build \
  -DFETCHCONTENT_SOURCE_DIR_GOOGLETEST=$PWD/algorithms/src/01-array-string/build/_deps/googletest-src
cmake --build project/mini-pipeline/build -j8
cd project/mini-pipeline/build && ctest --output-on-failure
```

## 结构

```
project/mini-pipeline/
├── REQUIREMENTS.md               需求规格（功能/非功能/工业界对照）
├── TASK.md                       任务书（5 个里程碑 + 内置的坑）
├── docs/ADR.md                   设计决策记录（你来做取舍）
├── CMakeLists.txt
├── include/mini_pipeline/        对外头文件（接口契约）
├── src/                          实现（M1 目前是桩）
└── tests/                        gtest 用例（先写测试，再实现）
```

## M1 验收清单（缺一不可）

- [ ] `sizeof(mp::Buffer) <= 16`
- [ ] 单测 12 个用例全绿
- [ ] ASan/UBSan 干净（无 double free、无 use-after-free）
- [ ] TSan 干净（引用计数与 free list 无线程竞争）
- [ ] 8 线程 × 2 万次 acquire/release 后 `stats().in_use == 0`
- [ ] `docs/M1-句柄与内存池.md`：记录你踩的坑（现象 → 工具输出 → 根因 → 修复 → 数据）
