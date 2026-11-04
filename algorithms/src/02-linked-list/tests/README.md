# 测试目录

> ⚠️ 各模式文件（`01_dummy_head.cpp` 等）当前仍是**骨架**，函数体为 `// TODO: 实现`。
> 测试已按题面语义写全，实现补齐后即可跑绿，可作为 TDD 的验收标准。

## 文件与覆盖范围

| 测试文件 | 被测源文件 | 覆盖题目 |
|:---------|:-----------|:---------|
| `dummy_head_test.cpp` | `../01_dummy_head.cpp` | LC 203 / 83 / 82 / 19 |
| `two_pointers_test.cpp` | `../02_two_pointers.cpp` | LC 876 / 141 / 142 / 160 |
| `reverse_test.cpp` | `../03_reverse.cpp` | LC 206（迭代 + 递归）/ 92 / 24 / 234 |
| `merge_sort_test.cpp` | `../04_merge_sort.cpp` | LC 21 / 86 / 328 / 61 / 143 / 148 / 147 |
| `design_test.cpp` | `../05_design.cpp` | LC 707 / 146 / 2 |

每个文件内部按题目分节，每节再按 **常规 / 边界 / 易错 / 规模** 四类组织用例。

## 命名约定

```
tests/<target>_test.cpp   →   可执行目标 <target>_test
```

`CMakeLists.txt` 通过 `file(GLOB tests/*_test.cpp)` 自动扫描并注册测试目标，
新增文件后重新执行 `cmake ..` 即可生效，**不需要改动 CMakeLists.txt**。

## 示例

以 LC 206 反转链表为例，`tests/reverse_test.cpp`：

```cpp
// ============================================================
// [E] LC 206 反转链表 — 单元测试
// ============================================================

#include <gtest/gtest.h>

#include "../03_reverse.cpp"   // 直接 include 被测源文件

TEST(ReverseListTest, BasicCase) {
  ListPtr in(list_utils::buildList({1, 2, 3, 4, 5}));
  // 反转会换头：旧头变成尾，必须 release() 交出所有权，
  // 否则 ListPtr 从旧头释放时只会释放一个结点
  ListPtr out(reverseList(in.release()));
  EXPECT_EQ(out.toVector(), (std::vector<int>{5, 4, 3, 2, 1}));
}

TEST(ReverseListTest, EmptyList) {
  EXPECT_EQ(reverseList(nullptr), nullptr);
}
```

## 工具速查（`../list_utils.h`）

| 工具 | 用途 |
|:-----|:-----|
| `list_utils::buildList({1,2,3})` | 由初始化列表/`vector` 构造链表 |
| `list_utils::toVector(head)` | 链表 → `vector`，可直接 `EXPECT_EQ` 比较；带防环上限 |
| `list_utils::length(head)` | 统计长度，带防环上限 |
| `list_utils::nodeAt(head, i)` | 定位第 `i` 个结点（构造相交/成环数据用） |
| `list_utils::connectTailTo(head, pos)` | 让尾结点指向第 `pos` 个结点，`pos < 0` 表示不建环 |
| `list_utils::breakCycle(head)` | 断开环，恢复成单链表 |
| `list_utils::freeList(head)` | 逐个 `delete`；带环链表请先 `breakCycle` |
| `ListPtr` | RAII 包装，出作用域自动释放整条链表 |

## 每个题目的测试建议覆盖

1. 常规用例（长度 ≥ 3）
2. 边界：空链表 / 单结点 / 两结点
3. 与题目约束相关的极端场景（如 LC 19 删头结点、LC 82 全部重复、LC 61 `k >= n`）
4. 带环题目（LC 141/142）务必用 `breakCycle()` 收尾，避免内存泄漏

## 所有权约定（最容易踩的坑）

| 被测函数类型 | 输入 | 返回值 | 原因 |
|:-------------|:-----|:-------|:-----|
| 只读（`middleNode` / `hasCycle` / `isPalindrome`） | `in.get()` | 不托管 | 不改变链表结构，`ListPtr` 统一释放 |
| 换头 / 原地改写（`reverseList` / `partition` / `sortList` …） | `in.release()` | 新建 `ListPtr` 托管 | 旧头可能变成尾结点，从旧头释放会漏掉整条链 |
| 会 delete 结点（`removeElements` / `deleteDuplicates` / `removeNthFromEnd`） | `in.release()` | 新建 `ListPtr` 托管 | 命中结点已被 delete，旧 `ListPtr` 析构会二次释放 |
| 合并两条链（`mergeTwoLists`） | 两条都 `release()` | 新建 `ListPtr` 托管 | 合并后两链交织，按两个旧头分别释放会重复释放 |
| 只读输入 + 新建结果（`addTwoNumbers`） | `in.get()` | 新建 `ListPtr` 托管 | 不修改输入，输入与结果各管各的 |

## 运行

```bash
cd algorithms/src/02-linked-list
mkdir -p build && cd build
cmake .. && cmake --build . -j
ctest --output-on-failure
```
