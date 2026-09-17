---
name: algorithms-topic-authoring
description: "WORKFLOW SKILL — 在 algorithms/ 下新增算法专题、LeetCode 题目骨架或 gtest 单元测试时使用。USE FOR: 新增专题目录与目录结构；按仓库约定生成题目文件（LC 编号/难度注释块、TODO 桩、BF 与最优解命名、公共数据结构头）；生成 tests/<target>_test.cpp 单元测试用例；注册或排查 CMake 测试目标；同步专题 README 的目录结构与题目框架表格；审查既有题目/测试是否符合规范。DO NOT USE FOR: 与 algorithms/ 无关的 C++ 代码；环境与工具链问题排查；LeetCode 题解思路讲解（本 skill 只约束写法与交付标准，不代替解题推导）。"
---

# 算法专题创作 (algorithms-topic-authoring)

统一 `algorithms/` 下**专题目录、题目文件、单元测试**三者的写法与交付标准。
规则基准是 `src/02-linked-list`（新专题一律照此建），`src/01-array-string` 为历史写法，正在向基准靠拢。

## 规则速览

| 规则 | 内容 | 详见 |
|:----|:-----|:-----|
| **目录结构** | 专题编号与命名、文件布局、公共头、构建命令 | [references/directory-structure.md](./references/directory-structure.md) |
| **题目生成** | 注释块固定顺序、难度标记、TODO 桩、BF/最优解命名、README 同步 | [references/problem-rules.md](./references/problem-rules.md) |
| **测试生成** | 直连 include、TEST 命名、用例矩阵、断言选择、CMake 注册与运行 | [references/test-rules.md](./references/test-rules.md) |

## 标准工作流

1. **定位专题** —— 确认目标目录为 `algorithms/src/NN-<topic>/`；新专题先按 [directory-structure.md](./references/directory-structure.md) 建目录、`CMakeLists.txt`（用 `templates/CMakeLists.txt.tmpl`）与 `tests/README.md`。
2. **写题目骨架** —— 按 [problem-rules.md](./references/problem-rules.md) 与 `templates/problem.cpp.tmpl` 追加函数：文件头 banner → 每题注释块（`题目/要求/约束/思路/预期复杂度`）→ 函数体 `// TODO: 实现` + 零值返回 + `(void)param;`。
3. **同步专题 README** —— 目录结构清单登记新文件；「题目框架」表格补一行（# / 题目 / 难度 / 模式 / 核心思想 / 易错点）；新模式补「代码模板」小节；`algorithms/README.md` 状态改为 🚧 骨架已建。
4. **生成测试** —— 按 [test-rules.md](./references/test-rules.md) 与 `templates/test.cpp.tmpl`，每个题目文件一个 `tests/<target>_test.cpp`，覆盖常规 / 边界 / 易错 / 规模四类用例。
5. **构建验证** —— 用下方命令构建并运行，**新增测试必须全绿**；骨架阶段至少确认 `-Wall -Wextra` 零新增告警。

## 常用命令

```bash
cd algorithms/src/<NN-topic>

# 单目标构建 + 运行（推荐，骨架阶段无需 clang/gcc 之外的东西）
cmake -S . -B build -DBUILD_TEST_TARGET=<target>
cmake --build build
./build/<target>

# 全量构建后跑 ctest，并只在失败时输出详情
cmake -S . -B build && cmake --build build
cd build && ctest --output-on-failure

# 只跑某个 TEST 组
cmake -S . -B build -DTEST_FILTER='TwoSumTest.*'
./build/<target> --gtest_filter='TwoSumTest.*'
```

> `-DBUILD_TEST_TARGET` 生效的前提是 `CMakeLists.txt` 的 `add_single_test` 带 `EXISTS` 守卫（见模板）。没有守卫的专题（如当前的 `01-array-string`）必须显式传该参数，否则缺失的测试源文件会让 configure 直接失败。

## 模板索引

| 模板 | 用途 |
|:-----|:-----|
| [templates/topic-readme.md.tmpl](./templates/topic-readme.md.tmpl) | 新专题的 `README.md` 骨架 |
| [templates/CMakeLists.txt.tmpl](./templates/CMakeLists.txt.tmpl) | 新专题的构建配置（GLOB 自动扫描 + `EXISTS` 守卫 + OBJECT 骨架校验） |
| [templates/problem.cpp.tmpl](./templates/problem.cpp.tmpl) | 单题骨架（banner + 注释块 + TODO 桩） |
| [templates/test.cpp.tmpl](./templates/test.cpp.tmpl) | 单题单元测试（直连 include + 分节用例） |

## 现状偏差（已知债务，勿当作规范）

| 项 | 现状 | 目标 |
|:--|:-----|:-----|
| `01-array-string/tests/03_prefix_sum_test.cpp` | 文件缺失，但 `CMakeLists.txt` 已注册 → 默认 configure 失败 | 补文件，或给 `add_single_test` 加 `EXISTS` 守卫 |
| `01-array-string/tests/02_sliding_window_test.cpp` | 16 个用例全 RED（引用了尚未实现的 `lengthOfLongestSubstring`） | 实现后转绿 |
| `02_sliding_window.cpp` 的 `lengthOfLongestSubstringBF` | 内层 `while` 的 `j` 不递增 → 遇重复字符死循环；`s[j]` 可能越界 | BF 实现同样必须可终止 |
| `01-array-string/CMakeLists.txt` | 显式逐行注册、无 OBJECT 骨架校验目标 | 向 02-linked-list 口径对齐 |

## 何时不要用本 skill

- 只是问某道题的解法思路或复杂度推导 —— 直接回答即可。
- 修改 `cpp/`、`linux/` 等其他专题 —— 各自约定不同。
- 踩到编译器/链接器/环境问题 —— 按常规排查，本 skill 只描述仓库约定。
