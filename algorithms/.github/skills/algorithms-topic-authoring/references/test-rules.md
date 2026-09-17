# 测试生成规则

## 文件粒度与骨架

- 一个题目源文件对应一个测试文件：`src/NN-<topic>/tests/<target>_test.cpp`，目标名 = 文件名去扩展名。
- 固定骨架（顺序不能变）：

```cpp
// ============================================================
// [E] LC 167 两数之和 II — 单元测试
// 覆盖: twoSum
// ============================================================

#include <gtest/gtest.h>
#include <vector>
using namespace std;

#include "../01_two_pointers.cpp"      // 直接 include 被测源文件

TEST(TwoSumTest, BasicCase) { ... }
```

- **直接 `#include` 被测 `.cpp`**，不做头文件/库拆分；路径相对 `tests/` 目录（`../NN_<pattern>.cpp`）。
- 只 include 真正用到的标准库（多余 include 会触发 IDE 的 unused-include 诊断）。`<vector>`、`<string>` 等按需保留。
- 文件头注释两行：难度 + LC 编号 + 标题，`覆盖:` 列出被测函数清单。

## 命名

| 对象 | 规则 | 例 |
|:----|:-----|:---|
| `TEST` 组名 | 函数名首字母大写的 PascalCase + `Test` | `TwoSumTest`、`MoveZeroesTest`、`LengthOfLongestSubstringTest` |
| 用例名 | PascalCase，描述**场景**而非序号 | `BasicCase`、`WithNegativeNumbers`、`NoSolution`、`AllSameCharacters` |
| 分组 | 用 `// ===== 分组名 =====` 分节，与用例矩阵一致 | `常规用例` / `边界用例` / `易错用例` |

## 用例矩阵（每个函数的下限）

| 类别 | 要求 | 例 |
|:----|:-----|:---|
| 常规 | ≥ 2 例，含题面样例 | `abcabcbb` → 3、`pwwkew` → 3 |
| 边界 | 空输入 / 单元素 / 全相同 / 极值 | `""` → 0、`"a"` → 1、`"bbbbb"` → 1 |
| 易错 | 题目专属陷阱，每条附中文注释说明"为什么" | `dvdf` → 3（左界须跳跃）、`abba` → 2（左界不可回退） |
| 规模 | ≥ 1 例，验证复杂度不退化为暴力，n 取 1e4 量级 | 52 字符 × 1000 次（n=52000）→ 52 |

## 断言规范

- 值比较用 `EXPECT_EQ`；布尔用 `EXPECT_TRUE` / `EXPECT_FALSE`。
- **不引入 gmock 匹配器**（`EXPECT_THAT`、`UnorderedElementsAre` 等），保持与现有测试一致。
- 多解题目用「长度 + 性质」间接断言，不要硬编码其中一种答案：

  ```cpp
  TEST(LongestPalindromeTest, NoPalOverOne) {
    string s = longestPalindrome("ab");
    EXPECT_EQ(s.length(), 1u);   // "a" 或 "b" 均合法
  }
  ```

- 就地修改类题目（形参为引用）先构造局部变量，再断言容器的最终状态：

  ```cpp
  vector<int> nums = {0, 1, 0, 3, 12};
  moveZeroes(nums);
  EXPECT_EQ(nums, (vector<int>{1, 3, 12, 0, 0}));
  ```

- 链表类题目必须用该专题的公共头工具托管内存，禁止裸 `new` / `delete`：

  | 工具 | 用途 |
  |:-----|:-----|
  | `list_utils::buildList({1,2,3})` | 构造链表 |
  | `list_utils::toVector(head)` | 链表 → `vector`，可直接比较 |
  | `list_utils::connectTailTo` / `breakCycle` | 建环 / 断环，带环题**必须**收尾 |
  | `ListPtr` | RAII 包装，出作用域自动释放 |

## CMake 注册

- `tests/*_test.cpp` 由 `file(GLOB ... CONFIGURE_DEPENDS)` 自动扫描 → **新增测试文件无需改 `CMakeLists.txt`**，重新 configure 即生效。
- 只有当模板未使用 GLOB 时才需要手工逐行注册 `add_single_test(<target> tests/<target>_test.cpp)`。
- `add_single_test` 必须带 `EXISTS` 守卫，避免"已登记但文件未创建"导致 configure 失败。

## 运行

```bash
cd algorithms/src/<NN-topic>
cmake -S . -B build -DBUILD_TEST_TARGET=<target> && cmake --build build
./build/<target>                                              # 全部用例
./build/<target> --gtest_filter='TwoSumTest.*'                # 只跑一组
cmake -S . -B build -DTEST_FILTER='TwoSumTest.*'              # 由 ctest 执行时过滤
```

## 交付标准（硬性）

1. **测试与被测实现同轮交付，新增用例必须全绿**。被测函数仍是 `// TODO: 实现` 时，先实现再写测试，或明确标注为待实现并**不要**把测试说成"已完成"。
2. 禁止为了让用例通过而放宽断言：改期望值、把 `EXPECT_EQ` 换成近似/弱断言、删掉失败用例都不允许。
3. 不写依赖用例执行顺序的测试（gtest 顺序不保证），组间不共享可变全局状态。
4. 期望值必须能口算复核：非平凡用例在代码里写一行中文注释说明推导过程。
