# 测试目录

本目录目前为空 —— `02-linked-list` 处于**骨架阶段**，各模式文件中的函数体还是 `// TODO: 实现`。

## 命名约定

```
tests/<target>_test.cpp   →   可执行目标 <target>_test
```

`CMakeLists.txt` 通过 `file(GLOB tests/*_test.cpp)` 自动扫描并注册测试目标，
新增文件后重新执行 `cmake ..` 即可生效，**不需要改动 CMakeLists.txt**。

## 示例

以 LC 206 反转链表为例，`tests/reverse_list_test.cpp`：

```cpp
// ============================================================
// [E] LC 206 反转链表 — 单元测试
// ============================================================

#include <gtest/gtest.h>

#include "../03_reverse.cpp"   // 直接 include 被测源文件

TEST(ReverseListTest, BasicCase) {
  ListPtr in(list_utils::buildList({1, 2, 3, 4, 5}));
  ListPtr out(reverseList(in.get()));          // 结果同样交给 ListPtr 托管
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
