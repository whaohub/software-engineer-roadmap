// ============================================================
// [E] LC 21  合并两个有序链表 — 单元测试
// [M] LC 86  分隔链表 — 单元测试
// [M] LC 328 奇偶链表 — 单元测试
// [M] LC 61  旋转链表 — 单元测试
// [M] LC 143 重排链表 — 单元测试
// [M] LC 148 排序链表 — 单元测试
// [M] LC 147 对链表进行插入排序 — 单元测试
// 覆盖: mergeTwoLists / partition / oddEvenList / rotateRight /
//       reorderList / sortList / insertionSortList
// ============================================================

#include <gtest/gtest.h>
#include <vector>
using namespace std;

#include "../04_merge_sort.cpp"
#include "../list_utils.h"

// 说明：
// 1. mergeTwoLists 合并后两链结点交织成一条，无法再按原来的两个头分别
//    释放（会重复释放共享段），因此两条输入都 release()，只由返回值持有整条链。
// 2. partition / oddEvenList / rotateRight / sortList / insertionSortList
//    会原地改写 next 且可能更换头结点，同样 release() + 托管返回值。
// 3. reorderList 是 void 且头结点不变，链表仍从原头可达，
//    直接传 in.get()，由 ListPtr 兜底释放。

// ============================================================
// [E] LC 21 合并两个有序链表
// ============================================================

// ------------------------------------------------------------
// 常规用例
// ------------------------------------------------------------

TEST(MergeTwoListsTest, LeetCodeSample) {
  ListPtr a(list_utils::buildList({1, 2, 4}));
  ListPtr b(list_utils::buildList({1, 3, 4}));
  ListPtr out(mergeTwoLists(a.release(), b.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 1, 2, 3, 4, 4}));
}

TEST(MergeTwoListsTest, NonOverlapping) {
  ListPtr a(list_utils::buildList({1, 2, 3}));
  ListPtr b(list_utils::buildList({4, 5, 6}));
  ListPtr out(mergeTwoLists(a.release(), b.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 3, 4, 5, 6}));
}

TEST(MergeTwoListsTest, NegativeValues) {
  ListPtr a(list_utils::buildList({-3, -1}));
  ListPtr b(list_utils::buildList({-2, 0}));
  ListPtr out(mergeTwoLists(a.release(), b.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{-3, -2, -1, 0}));
}

// ------------------------------------------------------------
// 边界用例
// ------------------------------------------------------------

TEST(MergeTwoListsTest, BothEmpty) {
  ListPtr out(mergeTwoLists(nullptr, nullptr));
  EXPECT_EQ(out.get(), nullptr);
}

TEST(MergeTwoListsTest, FirstEmpty) {
  ListPtr b(list_utils::buildList({0}));
  ListPtr out(mergeTwoLists(nullptr, b.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{0}));
}

TEST(MergeTwoListsTest, SecondEmpty) {
  ListPtr a(list_utils::buildList({1}));
  ListPtr out(mergeTwoLists(a.release(), nullptr));
  EXPECT_EQ(out.toVector(), (vector<int>{1}));
}

TEST(MergeTwoListsTest, SingleNodes) {
  ListPtr a(list_utils::buildList({2}));
  ListPtr b(list_utils::buildList({1}));
  ListPtr out(mergeTwoLists(a.release(), b.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2}));
}

// ------------------------------------------------------------
// 易错用例
// ------------------------------------------------------------

TEST(MergeTwoListsTest, AllDuplicates) {
  ListPtr a(list_utils::buildList({1, 1, 1}));
  ListPtr b(list_utils::buildList({1, 1}));
  ListPtr out(mergeTwoLists(a.release(), b.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 1, 1, 1, 1}));
}

TEST(MergeTwoListsTest, RemainingTailAppendedAsWhole) {
  // 一条链先走完时，另一条的剩余部分应整体接上（长度必须完整）
  ListPtr a(list_utils::buildList({5}));
  ListPtr b(list_utils::buildList({1, 2, 3, 4}));
  ListPtr out(mergeTwoLists(a.release(), b.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 3, 4, 5}));
  EXPECT_EQ(list_utils::length(out.get()), 5u);
}

TEST(MergeTwoListsTest, SecondConsumedFirst) {
  // 对称情形：后一条链先走完
  ListPtr a(list_utils::buildList({1, 2, 3, 4}));
  ListPtr b(list_utils::buildList({5}));
  ListPtr out(mergeTwoLists(a.release(), b.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 3, 4, 5}));
}

// ------------------------------------------------------------
// 规模用例（验证不退化；两链各取题面上限 50）
// ------------------------------------------------------------

TEST(MergeTwoListsTest, LargeInputInterleaved) {
  vector<int> va;
  vector<int> vb;
  va.reserve(50);
  vb.reserve(50);
  for (int i = 0; i < 50; ++i) {
    va.push_back(2 * i);
    vb.push_back(2 * i + 1);
  }
  ListPtr a(list_utils::buildList(va));
  ListPtr b(list_utils::buildList(vb));
  ListPtr out(mergeTwoLists(a.release(), b.release()));

  EXPECT_EQ(list_utils::length(out.get()), 100u);
  vector<int> result = out.toVector();
  for (int i = 0; i < 100; ++i) {
    EXPECT_EQ(result[i], i);
  }
}

// ============================================================
// [M] LC 86 分隔链表
// ============================================================

// ------------------------------------------------------------
// 常规用例
// ------------------------------------------------------------

TEST(PartitionTest, LeetCodeSample) {
  ListPtr in(list_utils::buildList({1, 4, 3, 2, 5, 2}));
  ListPtr out(partition(in.release(), 3));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 2, 4, 3, 5}));
}

TEST(PartitionTest, PreserveRelativeOrder) {
  // 两个分区内部都必须保持原始相对顺序
  ListPtr in(list_utils::buildList({5, 1, 4, 2, 3, 0}));
  ListPtr out(partition(in.release(), 3));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 0, 5, 4, 3}));
}

// ------------------------------------------------------------
// 边界用例
// ------------------------------------------------------------

TEST(PartitionTest, EmptyList) {
  ListPtr out(partition(nullptr, 0));
  EXPECT_EQ(out.get(), nullptr);
}

TEST(PartitionTest, SingleNode) {
  ListPtr in(list_utils::buildList({1}));
  ListPtr out(partition(in.release(), 2));
  EXPECT_EQ(out.toVector(), (vector<int>{1}));
}

TEST(PartitionTest, AllLessThanX) {
  ListPtr in(list_utils::buildList({1, 2, 3}));
  ListPtr out(partition(in.release(), 4));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 3}));
}

TEST(PartitionTest, AllGreaterOrEqual) {
  // x 取最小值时 small 链为空，large 链必须完整且无环
  ListPtr in(list_utils::buildList({3, 4, 5}));
  ListPtr out(partition(in.release(), 0));
  EXPECT_EQ(out.toVector(), (vector<int>{3, 4, 5}));
}

TEST(PartitionTest, AllEqualX) {
  ListPtr in(list_utils::buildList({3, 3, 3}));
  ListPtr out(partition(in.release(), 3));
  EXPECT_EQ(out.toVector(), (vector<int>{3, 3, 3}));
}

// ------------------------------------------------------------
// 易错用例
// ------------------------------------------------------------

TEST(PartitionTest, LargeTailMustBeNullTerminated) {
  // large 链尾若不清空 next，会残留原链表后继形成环；
  // 用长度校验可以在成环时立即暴露（toVector 有上限保护）
  ListPtr in(list_utils::buildList({1, 4, 3, 2, 5, 2}));
  ListPtr out(partition(in.release(), 3));
  EXPECT_EQ(list_utils::length(out.get()), 6u);
  EXPECT_EQ(out.toVector().size(), 6u);
}

TEST(PartitionTest, SmallChainEmptyAtHead) {
  // 头结点就 >= x，small 链为空：结果应从原头开始
  ListPtr in(list_utils::buildList({5, 1, 2}));
  ListPtr out(partition(in.release(), 5));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 5}));
}

TEST(PartitionTest, LargeChainEmpty) {
  ListPtr in(list_utils::buildList({1, 2, 3}));
  ListPtr out(partition(in.release(), 100));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 3}));
}

// ------------------------------------------------------------
// 规模用例（验证不退化；n 取题面上限 200）
// ------------------------------------------------------------

TEST(PartitionTest, LargeInput) {
  vector<int> vals;
  vals.reserve(200);
  for (int i = 0; i < 200; ++i) {
    vals.push_back((i % 2 == 0) ? 100 : 0);
  }
  ListPtr in(list_utils::buildList(vals));
  ListPtr out(partition(in.release(), 50));

  vector<int> result = out.toVector();
  ASSERT_EQ(result.size(), 200u);
  for (size_t i = 0; i < 100; ++i) {
    EXPECT_EQ(result[i], 0);
  }
  for (size_t i = 100; i < 200; ++i) {
    EXPECT_EQ(result[i], 100);
  }
}

// ============================================================
// [M] LC 328 奇偶链表
// ============================================================

// ------------------------------------------------------------
// 常规用例
// ------------------------------------------------------------

TEST(OddEvenListTest, LeetCodeSample) {
  ListPtr in(list_utils::buildList({1, 2, 3, 4, 5}));
  ListPtr out(oddEvenList(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 3, 5, 2, 4}));
}

TEST(OddEvenListTest, LeetCodeSampleTwo) {
  ListPtr in(list_utils::buildList({2, 1, 3, 5, 6, 4, 7}));
  ListPtr out(oddEvenList(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{2, 3, 6, 7, 1, 5, 4}));
}

// ------------------------------------------------------------
// 边界用例
// ------------------------------------------------------------

TEST(OddEvenListTest, EmptyList) {
  ListPtr out(oddEvenList(nullptr));
  EXPECT_EQ(out.get(), nullptr);
}

TEST(OddEvenListTest, SingleNode) {
  ListPtr in(list_utils::buildList({1}));
  ListPtr out(oddEvenList(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1}));
}

TEST(OddEvenListTest, TwoNodes) {
  ListPtr in(list_utils::buildList({1, 2}));
  ListPtr out(oddEvenList(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2}));
}

TEST(OddEvenListTest, ThreeNodes) {
  ListPtr in(list_utils::buildList({1, 2, 3}));
  ListPtr out(oddEvenList(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 3, 2}));
}

// ------------------------------------------------------------
// 易错用例
// ------------------------------------------------------------

TEST(OddEvenListTest, EvenFourNodes) {
  ListPtr in(list_utils::buildList({1, 2, 3, 4}));
  ListPtr out(oddEvenList(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 3, 2, 4}));
}

TEST(OddEvenListTest, NoCycleAtJunction) {
  // 奇链尾必须指向偶链头，偶链尾必须置空
  ListPtr in(list_utils::buildList({1, 2, 3, 4}));
  ListPtr out(oddEvenList(in.release()));
  EXPECT_EQ(list_utils::length(out.get()), 4u);
  EXPECT_EQ(out.toVector().size(), 4u);
}

TEST(OddEvenListTest, NegativeValues) {
  ListPtr in(list_utils::buildList({-1, -2, -3, -4, -5}));
  ListPtr out(oddEvenList(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{-1, -3, -5, -2, -4}));
}

// ------------------------------------------------------------
// 规模用例（验证不退化；n 取题面上限 10^4）
// ------------------------------------------------------------

TEST(OddEvenListTest, LargeInput) {
  vector<int> vals;
  vals.reserve(10000);
  for (int i = 1; i <= 10000; ++i) {
    vals.push_back(i);
  }
  ListPtr in(list_utils::buildList(vals));
  ListPtr out(oddEvenList(in.release()));

  vector<int> result = out.toVector();
  ASSERT_EQ(result.size(), 10000u);
  EXPECT_EQ(result[0], 1);
  EXPECT_EQ(result[4999], 9999);  // 奇链末尾
  EXPECT_EQ(result[5000], 2);     // 偶链开头
  EXPECT_EQ(result[9999], 10000); // 偶链末尾
}

// ============================================================
// [M] LC 61 旋转链表
// ============================================================

// ------------------------------------------------------------
// 常规用例
// ------------------------------------------------------------

TEST(RotateRightTest, LeetCodeSample) {
  ListPtr in(list_utils::buildList({1, 2, 3, 4, 5}));
  ListPtr out(rotateRight(in.release(), 2));
  EXPECT_EQ(out.toVector(), (vector<int>{4, 5, 1, 2, 3}));
}

TEST(RotateRightTest, KGreaterThanLength) {
  ListPtr in(list_utils::buildList({0, 1, 2}));
  ListPtr out(rotateRight(in.release(), 4));
  EXPECT_EQ(out.toVector(), (vector<int>{2, 0, 1}));
}

// ------------------------------------------------------------
// 边界用例
// ------------------------------------------------------------

TEST(RotateRightTest, EmptyList) {
  ListPtr out(rotateRight(nullptr, 0));
  EXPECT_EQ(out.get(), nullptr);
}

TEST(RotateRightTest, EmptyListWithPositiveK) {
  ListPtr out(rotateRight(nullptr, 5));
  EXPECT_EQ(out.get(), nullptr);
}

TEST(RotateRightTest, SingleNode) {
  ListPtr in(list_utils::buildList({1}));
  ListPtr out(rotateRight(in.release(), 100));
  EXPECT_EQ(out.toVector(), (vector<int>{1}));
}

TEST(RotateRightTest, KZero) {
  ListPtr in(list_utils::buildList({1, 2}));
  ListPtr out(rotateRight(in.release(), 0));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2}));
}

TEST(RotateRightTest, KEqualsLength) {
  // k % n == 0，应原样返回而不是在环上多绕一圈
  ListPtr in(list_utils::buildList({1, 2, 3}));
  ListPtr out(rotateRight(in.release(), 3));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 3}));
}

// ------------------------------------------------------------
// 易错用例
// ------------------------------------------------------------

TEST(RotateRightTest, TwoNodesRotateOne) {
  ListPtr in(list_utils::buildList({1, 2}));
  ListPtr out(rotateRight(in.release(), 1));
  EXPECT_EQ(out.toVector(), (vector<int>{2, 1}));
}

TEST(RotateRightTest, HugeK) {
  // k 可达 2×10^9，必须先取模，不能真的走 20 亿步
  ListPtr in(list_utils::buildList({1, 2, 3}));
  ListPtr out(rotateRight(in.release(), 2000000000));
  EXPECT_EQ(out.toVector(), (vector<int>{2, 3, 1}));
}

TEST(RotateRightTest, NoCycleAfterRotation) {
  // 断环必须彻底：结果长度应与原长度一致
  ListPtr in(list_utils::buildList({1, 2, 3, 4, 5}));
  ListPtr out(rotateRight(in.release(), 2));
  EXPECT_EQ(list_utils::length(out.get()), 5u);
}

// ------------------------------------------------------------
// 规模用例（验证不退化；n 取题面上限 500）
// ------------------------------------------------------------

TEST(RotateRightTest, LargeInput) {
  vector<int> vals;
  vals.reserve(500);
  for (int i = 1; i <= 500; ++i) {
    vals.push_back(i);
  }
  ListPtr in(list_utils::buildList(vals));
  ListPtr out(rotateRight(in.release(), 123));

  vector<int> result = out.toVector();
  ASSERT_EQ(result.size(), 500u);
  EXPECT_EQ(result.front(), 500 - 123 + 1);
  EXPECT_EQ(result.back(), 500 - 123);
}

// ============================================================
// [M] LC 143 重排链表
// ============================================================

// ------------------------------------------------------------
// 常规用例
// ------------------------------------------------------------

TEST(ReorderListTest, EvenLength) {
  ListPtr in(list_utils::buildList({1, 2, 3, 4}));
  reorderList(in.get());
  EXPECT_EQ(in.toVector(), (vector<int>{1, 4, 2, 3}));
}

TEST(ReorderListTest, OddLength) {
  ListPtr in(list_utils::buildList({1, 2, 3, 4, 5}));
  reorderList(in.get());
  EXPECT_EQ(in.toVector(), (vector<int>{1, 5, 2, 4, 3}));
}

// ------------------------------------------------------------
// 边界用例
// ------------------------------------------------------------

TEST(ReorderListTest, SingleNode) {
  ListPtr in(list_utils::buildList({1}));
  reorderList(in.get());
  EXPECT_EQ(in.toVector(), (vector<int>{1}));
}

TEST(ReorderListTest, TwoNodes) {
  ListPtr in(list_utils::buildList({1, 2}));
  reorderList(in.get());
  EXPECT_EQ(in.toVector(), (vector<int>{1, 2}));
}

TEST(ReorderListTest, ThreeNodes) {
  ListPtr in(list_utils::buildList({1, 2, 3}));
  reorderList(in.get());
  EXPECT_EQ(in.toVector(), (vector<int>{1, 3, 2}));
}

// ------------------------------------------------------------
// 易错用例
// ------------------------------------------------------------

TEST(ReorderListTest, SixNodes) {
  ListPtr in(list_utils::buildList({1, 2, 3, 4, 5, 6}));
  reorderList(in.get());
  EXPECT_EQ(in.toVector(), (vector<int>{1, 6, 2, 5, 3, 4}));
}

TEST(ReorderListTest, NoCycleAfterMerge) {
  // 前半段尾结点若不置空，两段仍相连会形成环
  ListPtr in(list_utils::buildList({1, 2, 3, 4, 5}));
  reorderList(in.get());
  EXPECT_EQ(list_utils::length(in.get()), 5u);
  EXPECT_EQ(in.toVector().size(), 5u);
}

TEST(ReorderListTest, NegativeValues) {
  ListPtr in(list_utils::buildList({-5, -4, -3, -2, -1}));
  reorderList(in.get());
  EXPECT_EQ(in.toVector(), (vector<int>{-5, -1, -4, -2, -3}));
}

// ------------------------------------------------------------
// 规模用例（验证不退化；n 取 10^4 以控制耗时）
// ------------------------------------------------------------

TEST(ReorderListTest, LargeInput) {
  vector<int> vals;
  vals.reserve(10000);
  for (int i = 1; i <= 10000; ++i) {
    vals.push_back(i);
  }
  ListPtr in(list_utils::buildList(vals));
  reorderList(in.get());

  vector<int> result = in.toVector();
  ASSERT_EQ(result.size(), 10000u);
  EXPECT_EQ(result[0], 1);
  EXPECT_EQ(result[1], 10000);
  EXPECT_EQ(result[2], 2);
  EXPECT_EQ(result[9999], 5001);
}

// ============================================================
// [M] LC 148 排序链表
// ============================================================

// ------------------------------------------------------------
// 常规用例
// ------------------------------------------------------------

TEST(SortListTest, BasicCase) {
  ListPtr in(list_utils::buildList({4, 2, 1, 3}));
  ListPtr out(sortList(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 3, 4}));
}

TEST(SortListTest, LeetCodeSample) {
  ListPtr in(list_utils::buildList({-1, 5, 3, 4, 0}));
  ListPtr out(sortList(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{-1, 0, 3, 4, 5}));
}

// ------------------------------------------------------------
// 边界用例
// ------------------------------------------------------------

TEST(SortListTest, EmptyList) {
  ListPtr out(sortList(nullptr));
  EXPECT_EQ(out.get(), nullptr);
}

TEST(SortListTest, SingleNode) {
  ListPtr in(list_utils::buildList({1}));
  ListPtr out(sortList(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1}));
}

TEST(SortListTest, TwoNodes) {
  ListPtr in(list_utils::buildList({2, 1}));
  ListPtr out(sortList(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2}));
}

TEST(SortListTest, AlreadySorted) {
  ListPtr in(list_utils::buildList({1, 2, 3}));
  ListPtr out(sortList(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 3}));
}

// ------------------------------------------------------------
// 易错用例
// ------------------------------------------------------------

TEST(SortListTest, ReverseSorted) {
  ListPtr in(list_utils::buildList({5, 4, 3, 2, 1}));
  ListPtr out(sortList(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 3, 4, 5}));
}

TEST(SortListTest, WithDuplicates) {
  ListPtr in(list_utils::buildList({3, 3, 1, 1, 2, 2}));
  ListPtr out(sortList(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 1, 2, 2, 3, 3}));
}

TEST(SortListTest, OddLengthSplit) {
  // 奇数长度时快慢指针切分会让前半段短一个结点，两段都必须完整排序
  ListPtr in(list_utils::buildList({7, 1, 5, 3, 9}));
  ListPtr out(sortList(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 3, 5, 7, 9}));
}

TEST(SortListTest, NoCycleAfterSplit) {
  // 找中点后两段必须断开，否则归并时成环
  ListPtr in(list_utils::buildList({4, 2, 6, 1}));
  ListPtr out(sortList(in.release()));
  EXPECT_EQ(list_utils::length(out.get()), 4u);
  EXPECT_EQ(out.toVector().size(), 4u);
}

// ------------------------------------------------------------
// 规模用例（验证不退化；n 取 10^4 以控制耗时）
// ------------------------------------------------------------

TEST(SortListTest, LargeInput) {
  vector<int> vals;
  vals.reserve(10000);
  for (int i = 0; i < 10000; ++i) {
    vals.push_back(10000 - i); // 完全逆序
  }
  ListPtr in(list_utils::buildList(vals));
  ListPtr out(sortList(in.release()));

  vector<int> result = out.toVector();
  ASSERT_EQ(result.size(), 10000u);
  for (int i = 0; i < 10000; ++i) {
    ASSERT_EQ(result[i], i + 1);
  }
}

// ============================================================
// [M] LC 147 对链表进行插入排序
// ============================================================

// ------------------------------------------------------------
// 常规用例
// ------------------------------------------------------------

TEST(InsertionSortListTest, BasicCase) {
  ListPtr in(list_utils::buildList({4, 2, 1, 3}));
  ListPtr out(insertionSortList(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 3, 4}));
}

TEST(InsertionSortListTest, LeetCodeSample) {
  ListPtr in(list_utils::buildList({-1, 5, 3, 4, 0}));
  ListPtr out(insertionSortList(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{-1, 0, 3, 4, 5}));
}

// ------------------------------------------------------------
// 边界用例
// ------------------------------------------------------------

TEST(InsertionSortListTest, EmptyList) {
  ListPtr out(insertionSortList(nullptr));
  EXPECT_EQ(out.get(), nullptr);
}

TEST(InsertionSortListTest, SingleNode) {
  ListPtr in(list_utils::buildList({1}));
  ListPtr out(insertionSortList(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1}));
}

TEST(InsertionSortListTest, AlreadySorted) {
  // 全程走 lastSorted 后移分支，不触发插入
  ListPtr in(list_utils::buildList({1, 2, 3}));
  ListPtr out(insertionSortList(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 3}));
}

TEST(InsertionSortListTest, TwoNodesReverse) {
  // 每个结点都要插到头部
  ListPtr in(list_utils::buildList({2, 1}));
  ListPtr out(insertionSortList(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2}));
}

// ------------------------------------------------------------
// 易错用例
// ------------------------------------------------------------

TEST(InsertionSortListTest, InsertAtHead) {
  // 新结点比已排序段所有值都小 → 必须插到 dummy 之后
  ListPtr in(list_utils::buildList({2, 3, 1}));
  ListPtr out(insertionSortList(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 3}));
}

TEST(InsertionSortListTest, InsertInMiddle) {
  ListPtr in(list_utils::buildList({1, 2, 4, 3, 5}));
  ListPtr out(insertionSortList(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 3, 4, 5}));
}

TEST(InsertionSortListTest, WithDuplicates) {
  ListPtr in(list_utils::buildList({3, 1, 2, 3, 1}));
  ListPtr out(insertionSortList(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 1, 2, 3, 3}));
}

TEST(InsertionSortListTest, NegativeValues) {
  ListPtr in(list_utils::buildList({0, -3, 2, -1}));
  ListPtr out(insertionSortList(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{-3, -1, 0, 2}));
}

// ------------------------------------------------------------
// 规模用例（O(n²)，n 取 1000 控制耗时；题面上限 5000）
// ------------------------------------------------------------

TEST(InsertionSortListTest, LargeInput) {
  vector<int> vals;
  vals.reserve(1000);
  for (int i = 0; i < 1000; ++i) {
    vals.push_back(1000 - i); // 完全逆序（最坏情况）
  }
  ListPtr in(list_utils::buildList(vals));
  ListPtr out(insertionSortList(in.release()));

  vector<int> result = out.toVector();
  ASSERT_EQ(result.size(), 1000u);
  for (int i = 0; i < 1000; ++i) {
    ASSERT_EQ(result[i], i + 1);
  }
}
