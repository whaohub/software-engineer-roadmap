// ============================================================
// [E] LC 203 移除链表元素 — 单元测试
// [E] LC 83  删除排序链表中的重复元素 — 单元测试
// [M] LC 82  删除排序链表中的重复元素 II — 单元测试
// [M] LC 19  删除链表的倒数第 N 个结点 — 单元测试
// 覆盖: removeElements / deleteDuplicates / deleteDuplicatesII /
//       removeNthFromEnd
// ============================================================

#include <gtest/gtest.h>
#include <vector>
using namespace std;

#include "../01_dummy_head.cpp"
#include "../list_utils.h"

// 说明：removeElements 会 delete 命中结点，因此输入链表必须用
//       ListPtr::release() 交出所有权（用 get() 会在 ListPtr 析构时二次释放）；
//       返回值一律重新交给新的 ListPtr 托管。

// ============================================================
// 常规用例
// ============================================================

TEST(RemoveElementsTest, BasicCase) {
  ListPtr in(list_utils::buildList({1, 2, 6, 3, 4, 5, 6}));
  ListPtr out(removeElements(in.release(), 6));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 3, 4, 5}));
}

TEST(RemoveElementsTest, NoMatch) {
  ListPtr in(list_utils::buildList({1, 2, 3}));
  ListPtr out(removeElements(in.release(), 4));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 3}));
}

TEST(RemoveElementsTest, RemoveScattered) {
  ListPtr in(list_utils::buildList({1, 6, 2, 6, 3}));
  ListPtr out(removeElements(in.release(), 6));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 3}));
}

// ============================================================
// 边界用例
// ============================================================

TEST(RemoveElementsTest, EmptyList) {
  EXPECT_EQ(removeElements(nullptr, 1), nullptr);
}

TEST(RemoveElementsTest, SingleNodeMatched) {
  // 头结点被删除后 dummy.next 变为 nullptr
  ListPtr in(list_utils::buildList({6}));
  ListPtr out(removeElements(in.release(), 6));
  EXPECT_EQ(out.get(), nullptr);
}

TEST(RemoveElementsTest, SingleNodeNotMatched) {
  ListPtr in(list_utils::buildList({1}));
  ListPtr out(removeElements(in.release(), 6));
  EXPECT_EQ(out.toVector(), (vector<int>{1}));
}

TEST(RemoveElementsTest, AllNodesMatched) {
  ListPtr in(list_utils::buildList({6, 6, 6}));
  ListPtr out(removeElements(in.release(), 6));
  EXPECT_EQ(out.get(), nullptr);
}

TEST(RemoveElementsTest, RemoveHead) {
  // 头结点命中时 prev 必须停在 dummy 上，不能前移
  ListPtr in(list_utils::buildList({6, 1, 2}));
  ListPtr out(removeElements(in.release(), 6));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2}));
}

TEST(RemoveElementsTest, RemoveTail) {
  ListPtr in(list_utils::buildList({1, 2, 6}));
  ListPtr out(removeElements(in.release(), 6));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2}));
  EXPECT_EQ(list_utils::length(out.get()), 2u);
}

// ============================================================
// 易错用例
// ============================================================

TEST(RemoveElementsTest, ConsecutiveMatches) {
  // 连续命中时 prev 不能前移，否则会漏删后续结点
  ListPtr in(list_utils::buildList({6, 6, 1, 6, 6, 2, 6}));
  ListPtr out(removeElements(in.release(), 6));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2}));
}

TEST(RemoveElementsTest, ValAtConstraintBoundary) {
  // 约束: 1 <= Node.val <= 50，0 <= val <= 50；val 取上界 50
  ListPtr in(list_utils::buildList({50, 1, 50, 2}));
  ListPtr out(removeElements(in.release(), 50));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2}));
}

// ============================================================
// 规模用例（验证不退化）
// ============================================================

TEST(RemoveElementsTest, LargeInputLinearTime) {
  // n = 10000，交替 7 / 1，删除全部 7 后应剩 5000 个 1
  vector<int> vals;
  vals.reserve(10000);
  for (int i = 0; i < 10000; ++i) {
    vals.push_back(i % 2 == 0 ? 7 : 1);
  }
  ListPtr in(list_utils::buildList(vals));
  ListPtr out(removeElements(in.release(), 7));
  EXPECT_EQ(list_utils::length(out.get()), 5000u);
  EXPECT_EQ(out.toVector(), (vector<int>(5000, 1)));
}

// ============================================================
// [E] LC 83 删除排序链表中的重复元素
// ============================================================

// ------------------------------------------------------------
// 常规用例
// ------------------------------------------------------------

TEST(DeleteDuplicatesTest, BasicCase) {
  ListPtr in(list_utils::buildList({1, 1, 2}));
  ListPtr out(deleteDuplicates(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2}));
}

TEST(DeleteDuplicatesTest, NoDuplicates) {
  ListPtr in(list_utils::buildList({1, 2, 3}));
  ListPtr out(deleteDuplicates(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 3}));
}

TEST(DeleteDuplicatesTest, MultipleRuns) {
  ListPtr in(list_utils::buildList({1, 1, 2, 3, 3}));
  ListPtr out(deleteDuplicates(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 3}));
}

// ------------------------------------------------------------
// 边界用例
// ------------------------------------------------------------

TEST(DeleteDuplicatesTest, EmptyList) {
  EXPECT_EQ(deleteDuplicates(nullptr), nullptr);
}

TEST(DeleteDuplicatesTest, SingleNode) {
  ListPtr in(list_utils::buildList({1}));
  ListPtr out(deleteDuplicates(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1}));
}

TEST(DeleteDuplicatesTest, TwoNodesSame) {
  ListPtr in(list_utils::buildList({1, 1}));
  ListPtr out(deleteDuplicates(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1}));
}

TEST(DeleteDuplicatesTest, AllSame) {
  ListPtr in(list_utils::buildList({1, 1, 1, 1}));
  ListPtr out(deleteDuplicates(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1}));
}

TEST(DeleteDuplicatesTest, NegativeValues) {
  ListPtr in(list_utils::buildList({-3, -3, -1, 0, 0, 2}));
  ListPtr out(deleteDuplicates(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{-3, -1, 0, 2}));
}

// ------------------------------------------------------------
// 易错用例
// ------------------------------------------------------------

TEST(DeleteDuplicatesTest, RunOfThree) {
  // cur 命中后不能前移，否则 3 个以上的连续重复会漏删
  ListPtr in(list_utils::buildList({1, 1, 1, 2}));
  ListPtr out(deleteDuplicates(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2}));
}

TEST(DeleteDuplicatesTest, TailRun) {
  // 末尾重复：比较 cur->next->val 前必须判空，且删的必须是重复结点本身
  ListPtr in(list_utils::buildList({1, 2, 2}));
  ListPtr out(deleteDuplicates(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2}));
}

TEST(DeleteDuplicatesTest, HeadAndTailRuns) {
  ListPtr in(list_utils::buildList({1, 1, 2, 2}));
  ListPtr out(deleteDuplicates(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2}));
}

// ------------------------------------------------------------
// 规模用例（验证不退化）
// ------------------------------------------------------------

TEST(DeleteDuplicatesTest, LargeInputLinearTime) {
  // 1..100 每个值重复 3 次（n = 300），结果应为 1..100
  vector<int> vals;
  vals.reserve(300);
  for (int v = 1; v <= 100; ++v) {
    for (int k = 0; k < 3; ++k) {
      vals.push_back(v);
    }
  }
  ListPtr in(list_utils::buildList(vals));
  ListPtr out(deleteDuplicates(in.release()));

  vector<int> expected;
  expected.reserve(100);
  for (int v = 1; v <= 100; ++v) {
    expected.push_back(v);
  }
  EXPECT_EQ(list_utils::length(out.get()), 100u);
  EXPECT_EQ(out.toVector(), expected);
}

// ============================================================
// [M] LC 82 删除排序链表中的重复元素 II
// ============================================================

// ------------------------------------------------------------
// 常规用例
// ------------------------------------------------------------

TEST(DeleteDuplicatesIITest, BasicCase) {
  ListPtr in(list_utils::buildList({1, 2, 3, 3, 4, 4, 5}));
  ListPtr out(deleteDuplicatesII(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 5}));
}

TEST(DeleteDuplicatesIITest, AllUnique) {
  ListPtr in(list_utils::buildList({1, 2, 3}));
  ListPtr out(deleteDuplicatesII(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 3}));
}

TEST(DeleteDuplicatesIITest, MultipleDuplicateRuns) {
  ListPtr in(list_utils::buildList({1, 1, 2, 2, 3}));
  ListPtr out(deleteDuplicatesII(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{3}));
}

// ------------------------------------------------------------
// 边界用例
// ------------------------------------------------------------

TEST(DeleteDuplicatesIITest, EmptyList) {
  EXPECT_EQ(deleteDuplicatesII(nullptr), nullptr);
}

TEST(DeleteDuplicatesIITest, SingleNode) {
  ListPtr in(list_utils::buildList({1}));
  ListPtr out(deleteDuplicatesII(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1}));
}

TEST(DeleteDuplicatesIITest, TwoNodesSame) {
  ListPtr in(list_utils::buildList({1, 1}));
  ListPtr out(deleteDuplicatesII(in.release()));
  EXPECT_EQ(out.get(), nullptr);
}

TEST(DeleteDuplicatesIITest, AllSame) {
  ListPtr in(list_utils::buildList({1, 1, 1}));
  ListPtr out(deleteDuplicatesII(in.release()));
  EXPECT_EQ(out.get(), nullptr);
}

TEST(DeleteDuplicatesIITest, NegativeValues) {
  ListPtr in(list_utils::buildList({-3, -3, -1, 2, 2, 3}));
  ListPtr out(deleteDuplicatesII(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{-1, 3}));
}

// ------------------------------------------------------------
// 易错用例
// ------------------------------------------------------------

TEST(DeleteDuplicatesIITest, HeadRunDeleted) {
  // 头结点整段被删 → 返回值必须是 dummy.next，而不是 prev->next
  ListPtr in(list_utils::buildList({1, 1, 2}));
  ListPtr out(deleteDuplicatesII(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{2}));
}

TEST(DeleteDuplicatesIITest, TailRunDeleted) {
  ListPtr in(list_utils::buildList({1, 2, 2}));
  ListPtr out(deleteDuplicatesII(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1}));
}

TEST(DeleteDuplicatesIITest, RunOfThree) {
  // 整段删除：只删两个结点会残留一个
  ListPtr in(list_utils::buildList({1, 1, 1, 2, 2, 2}));
  ListPtr out(deleteDuplicatesII(in.release()));
  EXPECT_EQ(out.get(), nullptr);
}

TEST(DeleteDuplicatesIITest, RunBetweenUniqueValues) {
  ListPtr in(list_utils::buildList({1, 2, 2, 3}));
  ListPtr out(deleteDuplicatesII(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 3}));
}

TEST(DeleteDuplicatesIITest, AdjacentRuns) {
  ListPtr in(list_utils::buildList({1, 1, 2, 2}));
  ListPtr out(deleteDuplicatesII(in.release()));
  EXPECT_EQ(out.get(), nullptr);
}

// ------------------------------------------------------------
// 规模用例（验证不退化）
// ------------------------------------------------------------

TEST(DeleteDuplicatesIITest, LargeInputLinearTime) {
  // 1..100 各出现 2 次（应全删）+ 101..150 各出现 1 次（应保留）
  vector<int> vals;
  vals.reserve(250);
  for (int v = 1; v <= 100; ++v) {
    vals.push_back(v);
    vals.push_back(v);
  }
  for (int v = 101; v <= 150; ++v) {
    vals.push_back(v);
  }
  ListPtr in(list_utils::buildList(vals));
  ListPtr out(deleteDuplicatesII(in.release()));

  vector<int> expected;
  expected.reserve(50);
  for (int v = 101; v <= 150; ++v) {
    expected.push_back(v);
  }
  EXPECT_EQ(list_utils::length(out.get()), 50u);
  EXPECT_EQ(out.toVector(), expected);
}

// ============================================================
// [M] LC 19 删除链表的倒数第 N 个结点
// ============================================================

// ------------------------------------------------------------
// 常规用例
// ------------------------------------------------------------

TEST(RemoveNthFromEndTest, BasicCase) {
  ListPtr in(list_utils::buildList({1, 2, 3, 4, 5}));
  ListPtr out(removeNthFromEnd(in.release(), 2));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 3, 5}));
}

TEST(RemoveNthFromEndTest, RemoveMiddle) {
  ListPtr in(list_utils::buildList({1, 2, 3, 4, 5}));
  ListPtr out(removeNthFromEnd(in.release(), 3));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 4, 5}));
}

TEST(RemoveNthFromEndTest, RemoveTail) {
  ListPtr in(list_utils::buildList({1, 2, 3, 4, 5}));
  ListPtr out(removeNthFromEnd(in.release(), 1));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 3, 4}));
}

// ------------------------------------------------------------
// 边界用例
// ------------------------------------------------------------

TEST(RemoveNthFromEndTest, SingleNode) {
  ListPtr in(list_utils::buildList({1}));
  ListPtr out(removeNthFromEnd(in.release(), 1));
  EXPECT_EQ(out.get(), nullptr);
}

TEST(RemoveNthFromEndTest, RemoveHeadOfThree) {
  ListPtr in(list_utils::buildList({1, 2, 3}));
  ListPtr out(removeNthFromEnd(in.release(), 3));
  EXPECT_EQ(out.toVector(), (vector<int>{2, 3}));
}

TEST(RemoveNthFromEndTest, RemoveHeadOfTwo) {
  ListPtr in(list_utils::buildList({1, 2}));
  ListPtr out(removeNthFromEnd(in.release(), 2));
  EXPECT_EQ(out.toVector(), (vector<int>{2}));
}

TEST(RemoveNthFromEndTest, RemoveTailOfTwo) {
  ListPtr in(list_utils::buildList({1, 2}));
  ListPtr out(removeNthFromEnd(in.release(), 1));
  EXPECT_EQ(out.toVector(), (vector<int>{1}));
}

TEST(RemoveNthFromEndTest, NegativeValues) {
  // {-5, -1, 0, 3, 9} 倒数第 4 个是 -1
  ListPtr in(list_utils::buildList({-5, -1, 0, 3, 9}));
  ListPtr out(removeNthFromEnd(in.release(), 4));
  EXPECT_EQ(out.toVector(), (vector<int>{-5, 0, 3, 9}));
}

// ------------------------------------------------------------
// 易错用例
// ------------------------------------------------------------

TEST(RemoveNthFromEndTest, HeadDeletionNeedsDummy) {
  // n == 长度时目标是头结点：slow 必须从 dummy 出发，否则会误删第二个结点
  ListPtr in(list_utils::buildList({1, 2, 3, 4, 5}));
  ListPtr out(removeNthFromEnd(in.release(), 5));
  EXPECT_EQ(out.toVector(), (vector<int>{2, 3, 4, 5}));
}

TEST(RemoveNthFromEndTest, SecondFromEnd) {
  // n == 长度 - 1：slow 应停在头结点（即待删结点的前驱）
  ListPtr in(list_utils::buildList({1, 2, 3}));
  ListPtr out(removeNthFromEnd(in.release(), 2));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 3}));
}

// ------------------------------------------------------------
// 规模用例（验证不退化；超出题面 sz <= 30 约束，仅用于复杂度校验）
// ------------------------------------------------------------

TEST(RemoveNthFromEndTest, LargeInputLinearTime) {
  // n = 10000，删除尾结点后应剩 9999 个，尾元素为 9999
  vector<int> vals;
  vals.reserve(10000);
  for (int i = 1; i <= 10000; ++i) {
    vals.push_back(i);
  }
  ListPtr in(list_utils::buildList(vals));
  ListPtr out(removeNthFromEnd(in.release(), 1));
  EXPECT_EQ(list_utils::length(out.get()), 9999u);
  EXPECT_EQ(out.toVector().back(), 9999);
}
