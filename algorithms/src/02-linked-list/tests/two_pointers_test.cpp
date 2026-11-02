// ============================================================
// [E] LC 876 链表的中间结点 — 单元测试
// [E] LC 141 环形链表 — 单元测试
// [M] LC 142 环形链表 II — 单元测试
// [E] LC 160 相交链表 — 单元测试
// 覆盖: middleNode / hasCycle / detectCycle / getIntersectionNode
// ============================================================

#include <gtest/gtest.h>
#include <vector>
using namespace std;

#include "../02_two_pointers.cpp"
#include "../list_utils.h"

// 说明：
// 1. middleNode 返回的是链表内部结点的指针，不转移所有权，
//    因此结果不能交给 ListPtr 托管，用「指针相等」断言位置。
// 2. hasCycle 的带环用例结束后必须 breakCycle()，
//    否则 ListPtr 析构时 freeList 会在环中死循环。

// ============================================================
// [E] LC 876 链表的中间结点
// ============================================================

// ------------------------------------------------------------
// 常规用例
// ------------------------------------------------------------

TEST(MiddleNodeTest, OddLength) {
  ListPtr in(list_utils::buildList({1, 2, 3, 4, 5}));
  EXPECT_EQ(middleNode(in.get()), list_utils::nodeAt(in.get(), 2)); // 值 3
}

TEST(MiddleNodeTest, EvenLengthReturnsSecondMiddle) {
  ListPtr in(list_utils::buildList({1, 2, 3, 4}));
  EXPECT_EQ(middleNode(in.get()), list_utils::nodeAt(in.get(), 2)); // 值 3
}

TEST(MiddleNodeTest, LongerList) {
  ListPtr in(list_utils::buildList({1, 2, 3, 4, 5, 6, 7}));
  EXPECT_EQ(middleNode(in.get()), list_utils::nodeAt(in.get(), 3)); // 值 4
}

// ------------------------------------------------------------
// 边界用例
// ------------------------------------------------------------

TEST(MiddleNodeTest, SingleNode) {
  ListPtr in(list_utils::buildList({1}));
  EXPECT_EQ(middleNode(in.get()), in.get());
}

TEST(MiddleNodeTest, TwoNodes) {
  ListPtr in(list_utils::buildList({1, 2}));
  EXPECT_EQ(middleNode(in.get()), list_utils::nodeAt(in.get(), 1)); // 值 2
}

TEST(MiddleNodeTest, NegativeValues) {
  ListPtr in(list_utils::buildList({-3, -2, -1, 0, 1}));
  EXPECT_EQ(middleNode(in.get()), list_utils::nodeAt(in.get(), 2)); // 值 -1
}

// ------------------------------------------------------------
// 易错用例
// ------------------------------------------------------------

TEST(MiddleNodeTest, SixNodesReturnFourth) {
  // 偶数长度必须返回「第二个」中间结点（值 4），而不是第一个（值 3）
  ListPtr in(list_utils::buildList({1, 2, 3, 4, 5, 6}));
  EXPECT_EQ(middleNode(in.get()), list_utils::nodeAt(in.get(), 3));
}

TEST(MiddleNodeTest, ReturnsSuffixWithoutModifyingList) {
  // 从中点出发的序列应是原链表的后缀，且原链表保持不变
  ListPtr in(list_utils::buildList({1, 2, 3, 4, 5}));
  ListNode *mid = middleNode(in.get());
  EXPECT_EQ(list_utils::toVector(mid), (vector<int>{3, 4, 5}));
  EXPECT_EQ(list_utils::toVector(in.get()), (vector<int>{1, 2, 3, 4, 5}));
}

// ------------------------------------------------------------
// 规模用例（题面上限 n = 100）
// ------------------------------------------------------------

TEST(MiddleNodeTest, MaxConstraintLength) {
  // 100 个结点：偶数长度返回第二个中点，即下标 50（值 51）
  vector<int> vals;
  vals.reserve(100);
  for (int i = 1; i <= 100; ++i) {
    vals.push_back(i);
  }
  ListPtr in(list_utils::buildList(vals));
  ListNode *mid = middleNode(in.get());
  EXPECT_EQ(mid, list_utils::nodeAt(in.get(), 50));
  EXPECT_EQ(mid->val, 51);
}

// ============================================================
// [E] LC 141 环形链表
// ============================================================

// ------------------------------------------------------------
// 常规用例
// ------------------------------------------------------------

TEST(HasCycleTest, CycleInMiddle) {
  ListPtr in(list_utils::buildList({3, 2, 0, -4}));
  list_utils::connectTailTo(in.get(), 1); // 尾 → 第 1 个结点（值 2）
  EXPECT_TRUE(hasCycle(in.get()));
  list_utils::breakCycle(in.get());
}

TEST(HasCycleTest, NoCycle) {
  ListPtr in(list_utils::buildList({1, 2}));
  EXPECT_FALSE(hasCycle(in.get()));
}

TEST(HasCycleTest, CycleToHead) {
  ListPtr in(list_utils::buildList({1, 2, 3, 4}));
  list_utils::connectTailTo(in.get(), 0); // 整条成环
  EXPECT_TRUE(hasCycle(in.get()));
  list_utils::breakCycle(in.get());
}

// ------------------------------------------------------------
// 边界用例
// ------------------------------------------------------------

TEST(HasCycleTest, EmptyList) { EXPECT_FALSE(hasCycle(nullptr)); }

TEST(HasCycleTest, SingleNodeNoCycle) {
  ListPtr in(list_utils::buildList({1}));
  EXPECT_FALSE(hasCycle(in.get()));
}

TEST(HasCycleTest, SingleNodeSelfCycle) {
  ListPtr in(list_utils::buildList({1}));
  list_utils::connectTailTo(in.get(), 0);
  EXPECT_TRUE(hasCycle(in.get()));
  list_utils::breakCycle(in.get());
}

TEST(HasCycleTest, TwoNodesCycle) {
  ListPtr in(list_utils::buildList({1, 2}));
  list_utils::connectTailTo(in.get(), 0);
  EXPECT_TRUE(hasCycle(in.get()));
  list_utils::breakCycle(in.get());
}

// ------------------------------------------------------------
// 易错用例
// ------------------------------------------------------------

TEST(HasCycleTest, SelfLoopAtTail) {
  // 尾结点自环：fast 每轮走两步时 fast->next 可能指向自己
  ListPtr in(list_utils::buildList({1, 2, 3}));
  list_utils::connectTailTo(in.get(), 2);
  EXPECT_TRUE(hasCycle(in.get()));
  list_utils::breakCycle(in.get());
}

TEST(HasCycleTest, NegativeValuesCycle) {
  ListPtr in(list_utils::buildList({-1, -2, -3}));
  list_utils::connectTailTo(in.get(), 1);
  EXPECT_TRUE(hasCycle(in.get()));
  list_utils::breakCycle(in.get());
}

// ------------------------------------------------------------
// 规模用例（验证不退化）
// ------------------------------------------------------------

TEST(HasCycleTest, LargeListNoCycle) {
  // n = 10000，无环时 fast 必须正常走到 nullptr 而不是越界
  vector<int> vals;
  vals.reserve(10000);
  for (int i = 0; i < 10000; ++i) {
    vals.push_back(i);
  }
  ListPtr in(list_utils::buildList(vals));
  EXPECT_FALSE(hasCycle(in.get()));
}

TEST(HasCycleTest, LargeListWithCycle) {
  // n = 10000，尾结点回到下标 5000，快慢指针应在环内相遇
  vector<int> vals;
  vals.reserve(10000);
  for (int i = 0; i < 10000; ++i) {
    vals.push_back(i);
  }
  ListPtr in(list_utils::buildList(vals));
  list_utils::connectTailTo(in.get(), 5000);
  EXPECT_TRUE(hasCycle(in.get()));
  list_utils::breakCycle(in.get());
}

// ============================================================
// [M] LC 142 环形链表 II
// ============================================================
// 注意：detectCycle 返回链表内部结点指针，不转移所有权；
//       带环用例结束后必须 breakCycle()。

// ------------------------------------------------------------
// 常规用例
// ------------------------------------------------------------

TEST(DetectCycleTest, CycleInMiddle) {
  ListPtr in(list_utils::buildList({3, 2, 0, -4}));
  ListNode *expected = list_utils::nodeAt(in.get(), 1); // 值 2
  list_utils::connectTailTo(in.get(), 1);
  EXPECT_EQ(detectCycle(in.get()), expected);
  list_utils::breakCycle(in.get());
}

TEST(DetectCycleTest, NoCycle) {
  ListPtr in(list_utils::buildList({1, 2, 3}));
  EXPECT_EQ(detectCycle(in.get()), nullptr);
}

TEST(DetectCycleTest, CycleToHead) {
  ListPtr in(list_utils::buildList({1, 2, 3, 4}));
  ListNode *expected = in.get();
  list_utils::connectTailTo(in.get(), 0);
  EXPECT_EQ(detectCycle(in.get()), expected);
  list_utils::breakCycle(in.get());
}

// ------------------------------------------------------------
// 边界用例
// ------------------------------------------------------------

TEST(DetectCycleTest, EmptyList) { EXPECT_EQ(detectCycle(nullptr), nullptr); }

TEST(DetectCycleTest, SingleNodeNoCycle) {
  ListPtr in(list_utils::buildList({1}));
  EXPECT_EQ(detectCycle(in.get()), nullptr);
}

TEST(DetectCycleTest, SingleNodeSelfCycle) {
  ListPtr in(list_utils::buildList({1}));
  ListNode *expected = in.get();
  list_utils::connectTailTo(in.get(), 0);
  EXPECT_EQ(detectCycle(in.get()), expected);
  list_utils::breakCycle(in.get());
}

TEST(DetectCycleTest, TwoNodesCycle) {
  ListPtr in(list_utils::buildList({1, 2}));
  ListNode *expected = in.get();
  list_utils::connectTailTo(in.get(), 0);
  EXPECT_EQ(detectCycle(in.get()), expected);
  list_utils::breakCycle(in.get());
}

TEST(DetectCycleTest, TailSelfLoop) {
  ListPtr in(list_utils::buildList({1, 2, 3}));
  ListNode *expected = list_utils::nodeAt(in.get(), 2);
  list_utils::connectTailTo(in.get(), 2);
  EXPECT_EQ(detectCycle(in.get()), expected);
  list_utils::breakCycle(in.get());
}

// ------------------------------------------------------------
// 易错用例
// ------------------------------------------------------------

TEST(DetectCycleTest, EntryIsNotMeetingPoint) {
  // 快慢指针在环内相遇的位置不是入口，必须再用同速指针从 head 出发
  ListPtr in(list_utils::buildList({1, 2, 3, 4, 5, 6}));
  ListNode *expected = list_utils::nodeAt(in.get(), 2); // 值 3
  list_utils::connectTailTo(in.get(), 2);
  EXPECT_EQ(detectCycle(in.get()), expected);
  list_utils::breakCycle(in.get());
}

TEST(DetectCycleTest, LongTailShortCycle) {
  // 头到入口较长（a = 7）、环较短（3 个结点），验证 a ≡ c (mod 环长)
  vector<int> vals;
  for (int i = 1; i <= 10; ++i) {
    vals.push_back(i);
  }
  ListPtr in(list_utils::buildList(vals));
  ListNode *expected = list_utils::nodeAt(in.get(), 7); // 值 8
  list_utils::connectTailTo(in.get(), 7);
  EXPECT_EQ(detectCycle(in.get()), expected);
  list_utils::breakCycle(in.get());
}

// ------------------------------------------------------------
// 规模用例（验证不退化）
// ------------------------------------------------------------

TEST(DetectCycleTest, LargeListNoCycle) {
  vector<int> vals;
  vals.reserve(10000);
  for (int i = 0; i < 10000; ++i) {
    vals.push_back(i);
  }
  ListPtr in(list_utils::buildList(vals));
  EXPECT_EQ(detectCycle(in.get()), nullptr);
}

TEST(DetectCycleTest, LargeListWithCycle) {
  // n = 10000，尾结点回到下标 5000
  vector<int> vals;
  vals.reserve(10000);
  for (int i = 0; i < 10000; ++i) {
    vals.push_back(i);
  }
  ListPtr in(list_utils::buildList(vals));
  ListNode *expected = list_utils::nodeAt(in.get(), 5000);
  list_utils::connectTailTo(in.get(), 5000);
  EXPECT_EQ(detectCycle(in.get()), expected);
  list_utils::breakCycle(in.get());
}

// ============================================================
// [E] LC 160 相交链表
// ============================================================
// 注意：getIntersectionNode 返回链表内部结点指针，不转移所有权。
//       为了验证相交，测试中把 B 的尾结点接到 A 的某个结点上，
//       用例结束前必须把 B 的尾结点断开，否则两个 ListPtr 会重复释放共享段。

// ------------------------------------------------------------
// 常规用例
// ------------------------------------------------------------

TEST(GetIntersectionNodeTest, LeetCodeSample) {
  ListPtr a(list_utils::buildList({4, 1, 8, 4, 5}));
  ListPtr b(list_utils::buildList({5, 6, 1}));
  ListNode *shared = list_utils::nodeAt(a.get(), 2); // 值 8
  ListNode *bTail = list_utils::nodeAt(b.get(), 2);
  bTail->next = shared;

  ListNode *result = getIntersectionNode(a.get(), b.get());

  bTail->next = nullptr; // 先断开共享，避免重复释放
  EXPECT_EQ(result, shared);
}

TEST(GetIntersectionNodeTest, SameLength) {
  ListPtr a(list_utils::buildList({1, 2, 3}));
  ListPtr b(list_utils::buildList({4, 5}));
  ListNode *shared = list_utils::nodeAt(a.get(), 2); // 值 3
  ListNode *bTail = list_utils::nodeAt(b.get(), 1);
  bTail->next = shared;

  ListNode *result = getIntersectionNode(a.get(), b.get());

  bTail->next = nullptr;
  EXPECT_EQ(result, shared);
}

TEST(GetIntersectionNodeTest, NoIntersection) {
  ListPtr a(list_utils::buildList({2, 6, 4}));
  ListPtr b(list_utils::buildList({1, 5}));
  EXPECT_EQ(getIntersectionNode(a.get(), b.get()), nullptr);
}

// ------------------------------------------------------------
// 边界用例
// ------------------------------------------------------------

TEST(GetIntersectionNodeTest, BothEmpty) {
  EXPECT_EQ(getIntersectionNode(nullptr, nullptr), nullptr);
}

TEST(GetIntersectionNodeTest, OneEmpty) {
  ListPtr b(list_utils::buildList({1}));
  EXPECT_EQ(getIntersectionNode(nullptr, b.get()), nullptr);
}

TEST(GetIntersectionNodeTest, IntersectionAtHead) {
  ListPtr a(list_utils::buildList({1, 2, 3}));
  ListPtr b(list_utils::buildList({4, 5}));
  ListNode *bTail = list_utils::nodeAt(b.get(), 1);
  bTail->next = a.get(); // 共享 A 的整个链表

  ListNode *result = getIntersectionNode(a.get(), b.get());

  bTail->next = nullptr;
  EXPECT_EQ(result, a.get());
}

TEST(GetIntersectionNodeTest, IntersectionAtTail) {
  ListPtr a(list_utils::buildList({7, 8}));
  ListPtr b(list_utils::buildList({9, 10}));
  ListNode *shared = list_utils::nodeAt(a.get(), 1); // 值 8
  ListNode *bTail = list_utils::nodeAt(b.get(), 1);
  bTail->next = shared;

  ListNode *result = getIntersectionNode(a.get(), b.get());

  bTail->next = nullptr;
  EXPECT_EQ(result, shared);
}

TEST(GetIntersectionNodeTest, SameListTwice) {
  ListPtr a(list_utils::buildList({1, 2, 3}));
  EXPECT_EQ(getIntersectionNode(a.get(), a.get()), a.get());
}

// ------------------------------------------------------------
// 易错用例
// ------------------------------------------------------------

TEST(GetIntersectionNodeTest, EqualValuesButDifferentNodes) {
  // 两条链表值完全相同但不是同一批结点，应返回 nullptr（比较指针而非 val）
  ListPtr a(list_utils::buildList({1, 2, 3}));
  ListPtr b(list_utils::buildList({1, 2, 3}));
  EXPECT_EQ(getIntersectionNode(a.get(), b.get()), nullptr);
}

TEST(GetIntersectionNodeTest, VeryDifferentLengths) {
  // a 长 6，b 长 1，交换赛道后仍应在共享结点相遇
  ListPtr a(list_utils::buildList({1, 2, 3, 4, 5, 6}));
  ListPtr b(list_utils::buildList({7}));
  ListNode *shared = list_utils::nodeAt(a.get(), 4); // 值 5
  ListNode *bTail = list_utils::nodeAt(b.get(), 0);
  bTail->next = shared;

  ListNode *result = getIntersectionNode(a.get(), b.get());

  bTail->next = nullptr;
  EXPECT_EQ(result, shared);
}

// ------------------------------------------------------------
// 规模用例（验证不退化）
// ------------------------------------------------------------

TEST(GetIntersectionNodeTest, LargeInputLinearTime) {
  // a、b 各 10000 个结点，共享 a 的后 5000 个
  vector<int> va;
  vector<int> vb;
  va.reserve(10000);
  vb.reserve(10000);
  for (int i = 0; i < 10000; ++i) {
    va.push_back(i);
    vb.push_back(-i - 1);
  }
  ListPtr a(list_utils::buildList(va));
  ListPtr b(list_utils::buildList(vb));
  ListNode *shared = list_utils::nodeAt(a.get(), 5000);
  ListNode *bTail = list_utils::nodeAt(b.get(), 9999);
  bTail->next = shared;

  ListNode *result = getIntersectionNode(a.get(), b.get());

  bTail->next = nullptr;
  EXPECT_EQ(result, shared);
}
