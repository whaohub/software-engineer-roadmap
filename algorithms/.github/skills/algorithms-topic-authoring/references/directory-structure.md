# 算法目录结构说明

## 仓库级布局

```
algorithms/
├── README.md                 # 专题总览：专题列表 + 前置知识 + 面试热度 + 状态
├── docs/                     # 跨专题方法论文档（如 Pruning.md）+ docs/README.md
└── src/
    ├── 01-array-string/      # 专题目录：NN-<kebab-case 英文名>
    ├── 02-linked-list/
    └── 03-binary-tree/ ... 10-graph/

<仓库根>/third_party/CMakeLists.txt   # gtest 由 FetchContent 统一提供（v1.14.0）
```

- 专题编号固定为 `01`~`10`，与 `algorithms/README.md` 的专题列表一一对应；新增专题必须同步该表格。
- 状态标记：`📋 待添加` / `🚧 骨架已建`（题目框架 + 思路注释完成，函数体待实现）/ `✅ 已完成`。
- 依赖只有 gtest 一个来源：各专题 `add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../third_party ${CMAKE_BINARY_DIR}/third_party)`，**不要**新增第三方依赖。

## 专题目录布局（以 `02-linked-list` 为基准）

```
src/NN-<topic>/
├── README.md                 # 专题核心笔记：目录结构 + 模式总览 + 题目框架表 + 代码模板
├── CMakeLists.txt            # 构建配置
├── <domain>_utils.h          # 可选：该专题的公共数据结构与测试工具（header-only）
├── 01_<pattern>.cpp          # 一个文件 = 一种模式（对应 README 的一节）
├── 02_<pattern>.cpp
├── ...
└── tests/
    ├── README.md             # 测试目录说明：命名约定 + 工具速查 + 覆盖建议
    └── <target>_test.cpp     # 可执行目标 <target>_test
```

`build/` 是构建产物目录，不入库。`CMAKE_EXPORT_COMPILE_COMMANDS ON` 已开启，便于 IDE 索引。

## 命名规则

| 对象 | 规则 | 正例 | 反例 |
|:----|:-----|:-----|:-----|
| 专题目录 | `NN-<kebab-case 英文名>` | `01-array-string`、`02-linked-list` | `01_array_string`、`数组` |
| 源文件 | `NN_<pattern>.cpp`，NN 一位数字，按 README 模式顺序 | `01_dummy_head.cpp`、`05_design.cpp` | `dummyHead.cpp` |
| 测试文件 | `tests/<target>_test.cpp`，**优先用题目/函数名**而非编号 | `two_sum_test.cpp`、`reverse_list_test.cpp` | `tests_01.cpp` |
| 测试目标 | 测试文件名去扩展名 `.cpp`，与文件名严格一致 | `tests/two_sum_test.cpp` → 目标 `two_sum_test` | `two_sum` |
| 公共头 | `<domain>_utils.h`，header-only | `list_utils.h` | `utils.h`、`common.hpp` |
| 构建参数 | `BUILD_TEST_TARGET` / `TEST_FILTER` 全大写下划线 | — | — |

> 测试目标名与文件名严格一致：`tests/two_sum_test.cpp` → 目标 `two_sum_test` → 可执行文件 `build/two_sum_test`。
> `01-array-string` 里存在 `02_sliding_window_test.cpp` 这种带编号的写法，属历史遗留；新文件请用题目名。

## 公共头文件（可选）

只有当该专题存在**被多个源文件与测试共用的数据结构**时才建，用 `list_utils.h` 做范本：

- header-only：所有函数与类内联（`inline` 函数 / 类内定义），可被多个 `.cpp` 与测试重复 include 而不违反 ODR。
- include guard 形如 `ALGORITHMS_<TOPIC>_UTILS_H`。
- 与 LeetCode 官方签名一致的结点/结构体放全局命名空间（`ListNode`、`DListNode`），工具放具名命名空间（`list_utils::`），RAII 包装单独命名（`ListPtr`）。
- 工具函数需带**防死循环上限**（如 `list_utils::kMaxNodes`），保证错误数据只失败不挂死。

## 构建与运行

```bash
cd algorithms/src/<NN-topic>

# 1. 只构建并运行单个测试目标（骨架阶段/迭代时首选）
cmake -S . -B build -DBUILD_TEST_TARGET=<target>
cmake --build build
./build/<target>

# 2. 全量构建 + ctest
cmake -S . -B build && cmake --build build
cd build && ctest --output-on-failure

# 3. 只跑部分用例
./build/<target> --gtest_filter='TwoSumTest.*'
```

- `-DBUILD_TEST_TARGET=<target>`：只编译该目标，其余跳过；留空表示全部。
- `-DTEST_FILTER=<pattern>`：写进 `add_test` 的 gtest 过滤参数，`-DTEST_FILTER='XxxTest.*'` 等价于运行时 `--gtest_filter`。
- 骨架阶段（`tests/` 为空）仍应能 `cmake --build` 成功 —— 由 OBJECT 库目标逐文件编译语法，见 `templates/CMakeLists.txt.tmpl`。
