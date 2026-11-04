// ============================================================
// [M] LC 707 设计链表 — 单元测试
// [M] LC 146 LRU 缓存 — 单元测试
// [M] LC 2   两数相加 — 单元测试
// 覆盖: MyLinkedList / LRUCache / addTwoNumbers
// ============================================================

#include <gtest/gtest.h>
#include <vector>
using namespace std;

#include "../05_design.cpp"
#include "../list_utils.h"

// 说明：
// 1. MyLinkedList / LRUCache 自行管理内部结点，测试中直接在栈上构造，
//    析构函数负责释放，无需 ListPtr。
// 2. addTwoNumbers 只读取输入链表并返回新建链表：
//    输入交给 ListPtr 托管，返回值另行托管，两者互不干扰。

// ============================================================
// [M] LC 707 设计链表
// ============================================================

// ------------------------------------------------------------
// 常规用例
// ------------------------------------------------------------

TEST(MyLinkedListTest, LeetCodeSample) {
  // ["MyLinkedList","addAtHead","addAtTail","addAtIndex","get",
  //  "deleteAtIndex","get"]
  // [[],[1],[3],[1,2],[1],[1],[1]]
  MyLinkedList list;
  list.addAtHead(1);
  list.addAtTail(3);
  list.addAtIndex(1, 2); // 1 → 2 → 3
  EXPECT_EQ(list.get(1), 2);
  list.deleteAtIndex(1); // 1 → 3
  EXPECT_EQ(list.get(1), 3);
}

TEST(MyLinkedListTest, AddAtHeadMultiple) {
  // 连续头插，最终顺序与插入顺序相反
  MyLinkedList list;
  list.addAtHead(1);
  list.addAtHead(2);
  list.addAtHead(3);
  EXPECT_EQ(list.get(0), 3);
  EXPECT_EQ(list.get(1), 2);
  EXPECT_EQ(list.get(2), 1);
}

TEST(MyLinkedListTest, AddAtTailMultiple) {
  MyLinkedList list;
  list.addAtTail(1);
  list.addAtTail(2);
  list.addAtTail(3);
  EXPECT_EQ(list.get(0), 1);
  EXPECT_EQ(list.get(1), 2);
  EXPECT_EQ(list.get(2), 3);
}

TEST(MyLinkedListTest, InsertInMiddle) {
  MyLinkedList list;
  list.addAtTail(1);
  list.addAtTail(4);
  list.addAtIndex(1, 2);
  list.addAtIndex(2, 3);
  EXPECT_EQ(list.get(0), 1);
  EXPECT_EQ(list.get(1), 2);
  EXPECT_EQ(list.get(2), 3);
  EXPECT_EQ(list.get(3), 4);
}

// ------------------------------------------------------------
// 边界用例
// ------------------------------------------------------------

TEST(MyLinkedListTest, GetOnEmptyList) {
  MyLinkedList list;
  EXPECT_EQ(list.get(0), -1);
}

TEST(MyLinkedListTest, SingleElement) {
  MyLinkedList list;
  list.addAtHead(7);
  EXPECT_EQ(list.get(0), 7);
  EXPECT_EQ(list.get(1), -1);
}

TEST(MyLinkedListTest, AddAtIndexAtSizeAppends) {
  // index == size 等价于 addAtTail
  MyLinkedList list;
  list.addAtHead(1);
  list.addAtIndex(1, 2);
  EXPECT_EQ(list.get(1), 2);
}

TEST(MyLinkedListTest, AddAtIndexZeroEqualsAddAtHead) {
  MyLinkedList list;
  list.addAtTail(2);
  list.addAtIndex(0, 1);
  EXPECT_EQ(list.get(0), 1);
  EXPECT_EQ(list.get(1), 2);
}

TEST(MyLinkedListTest, DeleteHeadAndTail) {
  MyLinkedList list;
  list.addAtTail(1);
  list.addAtTail(2);
  list.addAtTail(3);
  list.deleteAtIndex(0); // 删头 → 2 → 3
  EXPECT_EQ(list.get(0), 2);
  list.deleteAtIndex(1); // 删尾 → 2
  EXPECT_EQ(list.get(0), 2);
  EXPECT_EQ(list.get(1), -1);
}

TEST(MyLinkedListTest, DeleteAllThenGet) {
  MyLinkedList list;
  list.addAtTail(1);
  list.addAtTail(2);
  list.deleteAtIndex(0);
  list.deleteAtIndex(0);
  EXPECT_EQ(list.get(0), -1);
}

// ------------------------------------------------------------
// 易错用例
// ------------------------------------------------------------

TEST(MyLinkedListTest, AddAtIndexBeyondSizeIgnored) {
  // index > size 属于非法下标，不插入
  MyLinkedList list;
  list.addAtHead(1);
  list.addAtIndex(5, 9);
  EXPECT_EQ(list.get(0), 1);
  EXPECT_EQ(list.get(1), -1);
}

TEST(MyLinkedListTest, AddAtIndexNegativeIgnored) {
  MyLinkedList list;
  list.addAtHead(1);
  list.addAtIndex(-1, 9);
  EXPECT_EQ(list.get(0), 1);
  EXPECT_EQ(list.get(1), -1);
}

TEST(MyLinkedListTest, DeleteAtIndexOutOfRangeIgnored) {
  // 越界条件应为 index >= size（而非 index > size）
  MyLinkedList list;
  list.addAtHead(1);
  list.deleteAtIndex(1);
  EXPECT_EQ(list.get(0), 1);
  EXPECT_EQ(list.get(1), -1);
}

TEST(MyLinkedListTest, DeleteAtIndexNegativeIgnored) {
  MyLinkedList list;
  list.addAtHead(1);
  list.deleteAtIndex(-1);
  EXPECT_EQ(list.get(0), 1);
}

TEST(MyLinkedListTest, GetNegativeIndex) {
  MyLinkedList list;
  list.addAtHead(1);
  EXPECT_EQ(list.get(-1), -1);
}

TEST(MyLinkedListTest, RepeatedDeleteKeepsConsistency) {
  // 反复增删后结构仍应一致（双向链接的 prev/next 未写错）
  MyLinkedList list;
  list.addAtTail(1);
  list.addAtTail(2);
  list.addAtTail(3);
  list.deleteAtIndex(1);
  list.addAtIndex(1, 9);
  EXPECT_EQ(list.get(0), 1);
  EXPECT_EQ(list.get(1), 9);
  EXPECT_EQ(list.get(2), 3);
  EXPECT_EQ(list.get(3), -1);
}

// ------------------------------------------------------------
// 规模用例（验证不退化；操作数取题面上限 2000）
// ------------------------------------------------------------

TEST(MyLinkedListTest, LargeSequence) {
  MyLinkedList list;
  for (int i = 0; i < 1000; ++i) {
    list.addAtTail(i);
  }
  for (int i = 0; i < 1000; ++i) {
    EXPECT_EQ(list.get(i), i);
  }
  for (int i = 0; i < 1000; ++i) {
    list.deleteAtIndex(0);
  }
  EXPECT_EQ(list.get(0), -1);
}

// ============================================================
// [M] LC 146 LRU 缓存
// ============================================================

// ------------------------------------------------------------
// 常规用例
// ------------------------------------------------------------

TEST(LRUCacheTest, LeetCodeSample) {
  // ["LRUCache","put","put","get","put","get","put","get","get","get"]
  // [[2],[1,1],[2,2],[1],[3,3],[2],[4,4],[1],[3],[4]]
  LRUCache cache(2);
  cache.put(1, 1);
  cache.put(2, 2);
  EXPECT_EQ(cache.get(1), 1); // 1 变为最近使用
  cache.put(3, 3);            // 逐出 key 2
  EXPECT_EQ(cache.get(2), -1);
  cache.put(4, 4);            // 逐出 key 1
  EXPECT_EQ(cache.get(1), -1);
  EXPECT_EQ(cache.get(3), 3);
  EXPECT_EQ(cache.get(4), 4);
}

TEST(LRUCacheTest, CapacityOne) {
  LRUCache cache(1);
  cache.put(1, 1);
  cache.put(2, 2); // 逐出 1
  EXPECT_EQ(cache.get(1), -1);
  EXPECT_EQ(cache.get(2), 2);
}

TEST(LRUCacheTest, GetRefreshesRecency) {
  LRUCache cache(2);
  cache.put(1, 1);
  cache.put(2, 2);
  EXPECT_EQ(cache.get(1), 1); // 1 变为最近使用
  cache.put(3, 3);            // 应逐出 2
  EXPECT_EQ(cache.get(2), -1);
  EXPECT_EQ(cache.get(1), 1);
}

TEST(LRUCacheTest, EvictionOrderAfterMultipleOps) {
  LRUCache cache(3);
  cache.put(1, 1);
  cache.put(2, 2);
  cache.put(3, 3);
  cache.get(1);    // 使用顺序变为 2, 3, 1
  cache.put(4, 4); // 逐出 2
  EXPECT_EQ(cache.get(2), -1);
  EXPECT_EQ(cache.get(3), 3);
  EXPECT_EQ(cache.get(1), 1);
  EXPECT_EQ(cache.get(4), 4);
}

// ------------------------------------------------------------
// 边界用例
// ------------------------------------------------------------

TEST(LRUCacheTest, GetMissingKey) {
  LRUCache cache(2);
  EXPECT_EQ(cache.get(99), -1);
}

TEST(LRUCacheTest, ZeroKeyAndZeroValue) {
  // key / value 的 0 都是合法值，不能被当成「不存在」
  LRUCache cache(2);
  cache.put(0, 0);
  EXPECT_EQ(cache.get(0), 0);
}

TEST(LRUCacheTest, CapacityExceededByOne) {
  LRUCache cache(2);
  cache.put(1, 1);
  cache.put(2, 2);
  cache.put(3, 3); // 只剩 2 个
  EXPECT_EQ(cache.get(1), -1);
  EXPECT_EQ(cache.get(2), 2);
  EXPECT_EQ(cache.get(3), 3);
}

// ------------------------------------------------------------
// 易错用例
// ------------------------------------------------------------

TEST(LRUCacheTest, UpdateExistingKeyDoesNotEvict) {
  // 命中已有 key 应更新值 + 移到头部，而不是新建结点
  LRUCache cache(2);
  cache.put(1, 1);
  cache.put(2, 2);
  cache.put(1, 10); // 更新 1，不应逐出 2
  EXPECT_EQ(cache.get(2), 2);
  EXPECT_EQ(cache.get(1), 10);
}

TEST(LRUCacheTest, UpdateExistingKeyRefreshesRecency) {
  LRUCache cache(2);
  cache.put(1, 1);
  cache.put(2, 2);
  cache.put(1, 5); // 1 变为最近使用
  cache.put(3, 3); // 应逐出 2
  EXPECT_EQ(cache.get(2), -1);
  EXPECT_EQ(cache.get(1), 5);
}

TEST(LRUCacheTest, OverwriteTwiceKeepsSingleEntry) {
  LRUCache cache(1);
  cache.put(1, 1);
  cache.put(1, 2);
  cache.put(1, 3);
  EXPECT_EQ(cache.get(1), 3);
}

TEST(LRUCacheTest, MissDoesNotRefreshAnything) {
  // get 未命中不应影响淘汰顺序
  LRUCache cache(2);
  cache.put(1, 1);
  cache.put(2, 2);
  EXPECT_EQ(cache.get(99), -1);
  cache.put(3, 3); // 仍应逐出 1
  EXPECT_EQ(cache.get(1), -1);
  EXPECT_EQ(cache.get(2), 2);
}

// ------------------------------------------------------------
// 规模用例（验证不退化；capacity 取题面上限 3000）
// ------------------------------------------------------------

TEST(LRUCacheTest, LargeCapacityNoEviction) {
  LRUCache cache(3000);
  for (int i = 0; i < 3000; ++i) {
    cache.put(i, i * 2);
  }
  for (int i = 0; i < 3000; ++i) {
    EXPECT_EQ(cache.get(i), i * 2);
  }
}

TEST(LRUCacheTest, LargeCapacityWithEviction) {
  LRUCache cache(1000);
  for (int i = 0; i < 2000; ++i) {
    cache.put(i, i);
  }
  // 前 1000 个应全部被逐出
  EXPECT_EQ(cache.get(0), -1);
  EXPECT_EQ(cache.get(999), -1);
  EXPECT_EQ(cache.get(1000), 1000);
  EXPECT_EQ(cache.get(1999), 1999);
}

// ============================================================
// [M] LC 2 两数相加
// ============================================================

// ------------------------------------------------------------
// 常规用例
// ------------------------------------------------------------

TEST(AddTwoNumbersTest, LeetCodeSample) {
  ListPtr a(list_utils::buildList({2, 4, 3})); // 342
  ListPtr b(list_utils::buildList({5, 6, 4})); // 465
  ListPtr out(addTwoNumbers(a.get(), b.get()));
  EXPECT_EQ(out.toVector(), (vector<int>{7, 0, 8})); // 807
}

TEST(AddTwoNumbersTest, DifferentLengths) {
  ListPtr a(list_utils::buildList({1, 8})); // 81
  ListPtr b(list_utils::buildList({0}));
  ListPtr out(addTwoNumbers(a.get(), b.get()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 8}));
}

// ------------------------------------------------------------
// 边界用例
// ------------------------------------------------------------

TEST(AddTwoNumbersTest, BothZero) {
  ListPtr a(list_utils::buildList({0}));
  ListPtr b(list_utils::buildList({0}));
  ListPtr out(addTwoNumbers(a.get(), b.get()));
  EXPECT_EQ(out.toVector(), (vector<int>{0}));
}

TEST(AddTwoNumbersTest, SingleDigitsNoCarry) {
  ListPtr a(list_utils::buildList({1}));
  ListPtr b(list_utils::buildList({2}));
  ListPtr out(addTwoNumbers(a.get(), b.get()));
  EXPECT_EQ(out.toVector(), (vector<int>{3}));
}

// ------------------------------------------------------------
// 易错用例
// ------------------------------------------------------------

TEST(AddTwoNumbersTest, CarryProducesExtraDigit) {
  // 5 + 5 = 10：循环条件必须包含 carry != 0，否则会少一位
  ListPtr a(list_utils::buildList({5}));
  ListPtr b(list_utils::buildList({5}));
  ListPtr out(addTwoNumbers(a.get(), b.get()));
  EXPECT_EQ(out.toVector(), (vector<int>{0, 1}));
}

TEST(AddTwoNumbersTest, LongCarryChain) {
  ListPtr a(list_utils::buildList({9, 9, 9, 9, 9, 9, 9}));
  ListPtr b(list_utils::buildList({9, 9, 9, 9}));
  ListPtr out(addTwoNumbers(a.get(), b.get()));
  EXPECT_EQ(out.toVector(), (vector<int>{8, 9, 9, 9, 0, 0, 0, 1}));
}

TEST(AddTwoNumbersTest, DifferentLengthsWithCarry) {
  // 9 + 91 = 100：短链走完后仍要按 0 继续算
  ListPtr a(list_utils::buildList({9}));
  ListPtr b(list_utils::buildList({1, 9}));
  ListPtr out(addTwoNumbers(a.get(), b.get()));
  EXPECT_EQ(out.toVector(), (vector<int>{0, 0, 1}));
}

TEST(AddTwoNumbersTest, OneSideCarryToEnd) {
  // 99 + 1 = 100
  ListPtr a(list_utils::buildList({9, 9}));
  ListPtr b(list_utils::buildList({1}));
  ListPtr out(addTwoNumbers(a.get(), b.get()));
  EXPECT_EQ(out.toVector(), (vector<int>{0, 0, 1}));
}

TEST(AddTwoNumbersTest, InputsNotModified) {
  // 结果应为全新链表，输入链表保持原样
  ListPtr a(list_utils::buildList({2, 4, 3}));
  ListPtr b(list_utils::buildList({5, 6, 4}));
  ListPtr out(addTwoNumbers(a.get(), b.get()));
  EXPECT_EQ(a.toVector(), (vector<int>{2, 4, 3}));
  EXPECT_EQ(b.toVector(), (vector<int>{5, 6, 4}));
}

// ------------------------------------------------------------
// 规模用例（验证不退化；每位取题面上限 100 位）
// ------------------------------------------------------------

TEST(AddTwoNumbersTest, LargeInputWithCarryOut) {
  // 100 个 9 + 1 = 1 后面 100 个 0（共 101 位）
  vector<int> nines(100, 9);
  ListPtr a(list_utils::buildList(nines));
  ListPtr b(list_utils::buildList({1}));
  ListPtr out(addTwoNumbers(a.get(), b.get()));

  ASSERT_EQ(list_utils::length(out.get()), 101u);
  vector<int> result = out.toVector();
  EXPECT_EQ(result[0], 0);
  EXPECT_EQ(result[99], 0);
  EXPECT_EQ(result[100], 1);
}
