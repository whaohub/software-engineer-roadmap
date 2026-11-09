// ============================================================
// [E] LC 206 反转链表 — 单元测试（迭代 + 递归）
// [M] LC 92  反转链表 II — 单元测试
// [M] LC 24  两两交换链表中的节点 — 单元测试
// [E] LC 234 回文链表 — 单元测试
// 覆盖: reverseList / reverseListRecursive / reverseBetween /
//       swapPairs / isPalindrome
// ============================================================

#include <gtest/gtest.h>
#include <vector>
using namespace std;

#include "../03_reverse.cpp"
#include "../list_utils.h"

// 说明：
// 1. 反转会改变头结点指针：原头结点变成尾结点。若继续用旧头 ListPtr
//    释放，只会释放一个结点（其余全部泄漏），因此输入必须 release()，
//    返回值重新交给 ListPtr 托管。
// 2. isPalindrome 按题目要求应在比较后把后半段反转回原样，
//    链表结构不变，故直接传 in.get()，由 ListPtr 兜底释放。

// ============================================================
// [E] LC 206 反转链表（迭代）
// ============================================================

// ------------------------------------------------------------
// 常规用例
// ------------------------------------------------------------

TEST(ReverseListTest, BasicCase) {
  ListPtr in(list_utils::buildList({1, 2, 3, 4, 5}));
  ListPtr out(reverseList(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{5, 4, 3, 2, 1}));
}

TEST(ReverseListTest, TwoNodes) {
  ListPtr in(list_utils::buildList({1, 2}));
  ListPtr out(reverseList(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{2, 1}));
}

TEST(ReverseListTest, NegativeValues) {
  ListPtr in(list_utils::buildList({-1, 0, 3, -7}));
  ListPtr out(reverseList(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{-7, 3, 0, -1}));
}

// ------------------------------------------------------------
// 边界用例
// ------------------------------------------------------------

TEST(ReverseListTest, EmptyList) {
  ListPtr out(reverseList(nullptr));
  EXPECT_EQ(out.get(), nullptr);
}

TEST(ReverseListTest, SingleNode) {
  ListPtr in(list_utils::buildList({1}));
  ListPtr out(reverseList(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1}));
}

TEST(ReverseListTest, AllSameValue) {
  ListPtr in(list_utils::buildList({7, 7, 7}));
  ListPtr out(reverseList(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{7, 7, 7}));
}

// ------------------------------------------------------------
// 易错用例
// ------------------------------------------------------------

TEST(ReverseListTest, NewHeadIsOldTail) {
  // 返回值必须是原尾结点（值 5），而不是 nullptr 或原头结点
  ListPtr in(list_utils::buildList({1, 2, 3, 4, 5}));
  ListNode *oldHead = in.get();
  ListPtr out(reverseList(in.release()));
  EXPECT_EQ(out.get()->val, 5);
  EXPECT_EQ(out.get()->next->next->next->next, oldHead);
  EXPECT_EQ(oldHead->next, nullptr); // 原头变成尾，next 必须置空
}

TEST(ReverseListTest, LengthPreserved) {
  ListPtr in(list_utils::buildList({1, 2, 3, 4}));
  ListPtr out(reverseList(in.release()));
  EXPECT_EQ(list_utils::length(out.get()), 4u);
}

// ------------------------------------------------------------
// 规模用例（验证不退化；n 取题面上限 5000）
// ------------------------------------------------------------

TEST(ReverseListTest, LargeInput) {
  vector<int> vals;
  vals.reserve(5000);
  for (int i = 1; i <= 5000; ++i) {
    vals.push_back(i);
  }
  ListPtr in(list_utils::buildList(vals));
  ListPtr out(reverseList(in.release()));
  EXPECT_EQ(list_utils::length(out.get()), 5000u);
  EXPECT_EQ(out.toVector().front(), 5000);
  EXPECT_EQ(out.toVector().back(), 1);
}

// ============================================================
// [E] LC 206 反转链表（递归）
// ============================================================

TEST(ReverseListRecursiveTest, BasicCase) {
  ListPtr in(list_utils::buildList({1, 2, 3, 4, 5}));
  ListPtr out(reverseListRecursive(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{5, 4, 3, 2, 1}));
}

TEST(ReverseListRecursiveTest, EmptyList) {
  ListPtr out(reverseListRecursive(nullptr));
  EXPECT_EQ(out.get(), nullptr);
}

TEST(ReverseListRecursiveTest, SingleNode) {
  ListPtr in(list_utils::buildList({1}));
  ListPtr out(reverseListRecursive(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1}));
}

TEST(ReverseListRecursiveTest, SameResultAsIterative) {
  // 两种实现必须完全等价
  ListPtr a(list_utils::buildList({3, 1, 4, 1, 5, 9, 2, 6}));
  ListPtr b(list_utils::buildList({3, 1, 4, 1, 5, 9, 2, 6}));
  ListPtr outA(reverseList(a.release()));
  ListPtr outB(reverseListRecursive(b.release()));
  EXPECT_EQ(outA.toVector(), outB.toVector());
}

TEST(ReverseListRecursiveTest, LargeInput) {
  vector<int> vals;
  vals.reserve(1000);
  for (int i = 1; i <= 1000; ++i) {
    vals.push_back(i);
  }
  ListPtr in(list_utils::buildList(vals));
  ListPtr out(reverseListRecursive(in.release()));
  EXPECT_EQ(list_utils::length(out.get()), 1000u);
  EXPECT_EQ(out.toVector().front(), 1000);
}

// ============================================================
// [M] LC 92 反转链表 II
// ============================================================

// ------------------------------------------------------------
// 常规用例
// ------------------------------------------------------------

TEST(ReverseBetweenTest, MiddleSegment) {
  ListPtr in(list_utils::buildList({1, 2, 3, 4, 5}));
  ListPtr out(reverseBetween(in.release(), 2, 4));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 4, 3, 2, 5}));
}

TEST(ReverseBetweenTest, FromHead) {
  // left == 1：dummy 让「头结点参与反转」不必特判
  ListPtr in(list_utils::buildList({1, 2, 3}));
  ListPtr out(reverseBetween(in.release(), 1, 3));
  EXPECT_EQ(out.toVector(), (vector<int>{3, 2, 1}));
}

TEST(ReverseBetweenTest, ToTail) {
  ListPtr in(list_utils::buildList({1, 2, 3, 4, 5}));
  ListPtr out(reverseBetween(in.release(), 4, 5));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 3, 5, 4}));
}

// ------------------------------------------------------------
// 边界用例
// ------------------------------------------------------------

TEST(ReverseBetweenTest, SingleNodeList) {
  ListPtr in(list_utils::buildList({5}));
  ListPtr out(reverseBetween(in.release(), 1, 1));
  EXPECT_EQ(out.toVector(), (vector<int>{5}));
}

TEST(ReverseBetweenTest, LeftEqualsRight) {
  // 区间长度为 1，链表应原样返回
  ListPtr in(list_utils::buildList({1, 2, 3, 4, 5}));
  ListPtr out(reverseBetween(in.release(), 3, 3));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 3, 4, 5}));
}

TEST(ReverseBetweenTest, WholeList) {
  ListPtr in(list_utils::buildList({1, 2}));
  ListPtr out(reverseBetween(in.release(), 1, 2));
  EXPECT_EQ(out.toVector(), (vector<int>{2, 1}));
}

TEST(ReverseBetweenTest, FirstTwoNodes) {
  ListPtr in(list_utils::buildList({1, 2, 3, 4, 5}));
  ListPtr out(reverseBetween(in.release(), 1, 2));
  EXPECT_EQ(out.toVector(), (vector<int>{2, 1, 3, 4, 5}));
}

TEST(ReverseBetweenTest, LastTwoNodes) {
  ListPtr in(list_utils::buildList({1, 2, 3, 4, 5}));
  ListPtr out(reverseBetween(in.release(), 4, 5));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 3, 5, 4}));
}

// ------------------------------------------------------------
// 易错用例
// ------------------------------------------------------------

TEST(ReverseBetweenTest, WalkingStepsMustBeLeftMinusOne) {
  // prev 应停在 left 的前驱（值 2），多走一步就会把 3 也卷进反转区
  ListPtr in(list_utils::buildList({1, 2, 3, 4, 5}));
  ListPtr out(reverseBetween(in.release(), 3, 4));
  EXPECT_EQ(out.toVector(), (vector<int>{1, 2, 4, 3, 5}));
}

TEST(ReverseBetweenTest, OutsideSegmentUnchanged) {
  // 反转区两侧的结点顺序与内容都必须保持不变
  ListPtr in(list_utils::buildList({9, 8, 1, 2, 3, 7, 6}));
  ListPtr out(reverseBetween(in.release(), 3, 5));
  EXPECT_EQ(out.toVector(), (vector<int>{9, 8, 3, 2, 1, 7, 6}));
}

TEST(ReverseBetweenTest, NegativeValues) {
  ListPtr in(list_utils::buildList({-1, -2, -3, -4}));
  ListPtr out(reverseBetween(in.release(), 2, 3));
  EXPECT_EQ(out.toVector(), (vector<int>{-1, -3, -2, -4}));
}

// ------------------------------------------------------------
// 规模用例（验证不退化；n 取题面上限 500）
// ------------------------------------------------------------

TEST(ReverseBetweenTest, LargeInput) {
  vector<int> vals;
  vals.reserve(500);
  for (int i = 1; i <= 500; ++i) {
    vals.push_back(i);
  }
  ListPtr in(list_utils::buildList(vals));
  ListPtr out(reverseBetween(in.release(), 100, 400));

  vector<int> result = out.toVector();
  ASSERT_EQ(result.size(), 500u);
  EXPECT_EQ(result[0], 1);
  EXPECT_EQ(result[98], 99);
  EXPECT_EQ(result[99], 400);   // 反转段左端
  EXPECT_EQ(result[399], 100);  // 反转段右端
  EXPECT_EQ(result[400], 401);
  EXPECT_EQ(result[499], 500);
}

// ============================================================
// [M] LC 24 两两交换链表中的节点
// ============================================================

// ------------------------------------------------------------
// 常规用例
// ------------------------------------------------------------

TEST(SwapPairsTest, EvenLength) {
  ListPtr in(list_utils::buildList({1, 2, 3, 4}));
  ListPtr out(swapPairs(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{2, 1, 4, 3}));
}

TEST(SwapPairsTest, TwoNodes) {
  ListPtr in(list_utils::buildList({1, 2}));
  ListPtr out(swapPairs(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{2, 1}));
}

// ------------------------------------------------------------
// 边界用例
// ------------------------------------------------------------

TEST(SwapPairsTest, EmptyList) {
  ListPtr out(swapPairs(nullptr));
  EXPECT_EQ(out.get(), nullptr);
}

TEST(SwapPairsTest, SingleNode) {
  ListPtr in(list_utils::buildList({1}));
  ListPtr out(swapPairs(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{1}));
}

// ------------------------------------------------------------
// 易错用例
// ------------------------------------------------------------

TEST(SwapPairsTest, OddLengthTailUntouched) {
  // 奇数个结点时最后一个结点不成对，保持原位
  ListPtr in(list_utils::buildList({1, 2, 3}));
  ListPtr out(swapPairs(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{2, 1, 3}));
}

TEST(SwapPairsTest, OddLengthFive) {
  ListPtr in(list_utils::buildList({1, 2, 3, 4, 5}));
  ListPtr out(swapPairs(in.release()));
  EXPECT_EQ(out.toVector(), (vector<int>{2, 1, 4, 3, 5}));
}

TEST(SwapPairsTest, NodesSwappedNotValues) {
  // 题目要求实际交换结点：原第 1 个结点的 next 应指向原第 4 个结点
  ListPtr in(list_utils::buildList({1, 2, 3, 4}));
  ListNode *first = list_utils::nodeAt(in.get(), 0);
  ListNode *fourth = list_utils::nodeAt(in.get(), 3);
  ListPtr out(swapPairs(in.release()));
  EXPECT_EQ(out.get()->val, 2);       // 新头是原第 2 个结点
  EXPECT_EQ(first->next, fourth);      // 原第 1 个结点现在指向原第 4 个结点
}

// ------------------------------------------------------------
// 规模用例（验证不退化；n 取题面上限 100）
// ------------------------------------------------------------

TEST(SwapPairsTest, LargeEvenInput) {
  vector<int> vals;
  vals.reserve(100);
  for (int i = 1; i <= 100; ++i) {
    vals.push_back(i);
  }
  ListPtr in(list_utils::buildList(vals));
  ListPtr out(swapPairs(in.release()));
  vector<int> result = out.toVector();
  ASSERT_EQ(result.size(), 100u);
  for (size_t i = 0; i < result.size(); i += 2) {
    EXPECT_EQ(result[i], static_cast<int>(i) + 2);
    EXPECT_EQ(result[i + 1], static_cast<int>(i) + 1);
  }
}

// ============================================================
// [E] LC 234 回文链表
// ============================================================

// ------------------------------------------------------------
// 常规用例
// ------------------------------------------------------------

TEST(IsPalindromeTest, EvenPalindromic) {
  ListPtr in(list_utils::buildList({1, 2, 2, 1}));
  EXPECT_TRUE(isPalindrome(in.get()));
}

TEST(IsPalindromeTest, OddPalindromic) {
  ListPtr in(list_utils::buildList({1, 2, 3, 2, 1}));
  EXPECT_TRUE(isPalindrome(in.get()));
}

TEST(IsPalindromeTest, NotPalindromicEven) {
  ListPtr in(list_utils::buildList({1, 2, 3, 4}));
  EXPECT_FALSE(isPalindrome(in.get()));
}

// ------------------------------------------------------------
// 边界用例
// ------------------------------------------------------------

TEST(IsPalindromeTest, SingleNode) {
  ListPtr in(list_utils::buildList({1}));
  EXPECT_TRUE(isPalindrome(in.get()));
}

TEST(IsPalindromeTest, TwoSameNodes) {
  ListPtr in(list_utils::buildList({1, 1}));
  EXPECT_TRUE(isPalindrome(in.get()));
}

TEST(IsPalindromeTest, TwoDifferentNodes) {
  ListPtr in(list_utils::buildList({1, 2}));
  EXPECT_FALSE(isPalindrome(in.get()));
}

TEST(IsPalindromeTest, NegativeValues) {
  ListPtr in(list_utils::buildList({-1, 0, -1}));
  EXPECT_TRUE(isPalindrome(in.get()));
}

// ------------------------------------------------------------
// 易错用例
// ------------------------------------------------------------

TEST(IsPalindromeTest, OddLengthMiddleDiffers) {
  // 奇数长度时前半段比后半段多一个结点，比较循环只应走完后半段
  ListPtr in(list_utils::buildList({1, 2, 3, 1}));
  EXPECT_FALSE(isPalindrome(in.get()));
}

TEST(IsPalindromeTest, DifferentAtMiddle) {
  ListPtr in(list_utils::buildList({1, 2, 3, 3, 1}));
  EXPECT_FALSE(isPalindrome(in.get()));
}

TEST(IsPalindromeTest, OnlyFirstAndLastDiffer) {
  ListPtr in(list_utils::buildList({9, 2, 3, 2, 8}));
  EXPECT_FALSE(isPalindrome(in.get()));
}

TEST(IsPalindromeTest, ListStructurePreserved) {
  // 比较结束后应把后半段反转回去，调用方链表结构保持不变
  ListPtr in(list_utils::buildList({1, 2, 2, 1}));
  EXPECT_TRUE(isPalindrome(in.get()));
  EXPECT_EQ(in.toVector(), (vector<int>{1, 2, 2, 1}));
}

// ------------------------------------------------------------
// 规模用例（验证不退化；n 取题面上限 10^5）
// ------------------------------------------------------------

TEST(IsPalindromeTest, LargePalindrome) {
  vector<int> vals;
  vals.reserve(100000);
  for (int i = 0; i < 50000; ++i) {
    vals.push_back(i % 10);
  }
  for (int i = 49999; i >= 0; --i) {
    vals.push_back(i % 10);
  }
  ListPtr in(list_utils::buildList(vals));
  EXPECT_TRUE(isPalindrome(in.get()));
}

TEST(IsPalindromeTest, LargeNonPalindrome) {
  vector<int> vals;
  vals.reserve(100000);
  for (int i = 0; i < 99999; ++i) {
    vals.push_back(1);
  }
  vals[99998] = 2; // 末位不同，前期所有比较都相等
  ListPtr in(list_utils::buildList(vals));
  EXPECT_FALSE(isPalindrome(in.get()));
}
