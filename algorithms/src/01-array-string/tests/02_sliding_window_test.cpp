// ============================================================
// [M] LC 3 无重复字符的最长子串 — 单元测试
// 覆盖: lengthOfLongestSubstring（可变窗口：右扩左缩）
// ============================================================

#include <gtest/gtest.h>
#include <string>
using namespace std;

#include "../02_sliding_window.cpp"

// ============================================================
// 常规用例
// ============================================================

TEST(LengthOfLongestSubstringTest, BasicCase) {
  EXPECT_EQ(lengthOfLongestSubstring("abcabcbb"), 3); // "abc"
}

TEST(LengthOfLongestSubstringTest, WindowInMiddle) {
  // 易错：答案是连续子串 "wke"，而非子序列 "pwke"
  EXPECT_EQ(lengthOfLongestSubstring("pwwkew"), 3);
}

TEST(LengthOfLongestSubstringTest, EmptyString) {
  EXPECT_EQ(lengthOfLongestSubstring(""), 0);
}

TEST(LengthOfLongestSubstringTest, SingleCharacter) {
  EXPECT_EQ(lengthOfLongestSubstring("a"), 1);
}

// ============================================================
// 边界用例
// ============================================================

TEST(LengthOfLongestSubstringTest, AllSameCharacters) {
  EXPECT_EQ(lengthOfLongestSubstring("bbbbb"), 1); // "b"
}

TEST(LengthOfLongestSubstringTest, AllUnique) {
  EXPECT_EQ(lengthOfLongestSubstring("abcdefghijklmnopqrstuvwxyz"), 26);
}

TEST(LengthOfLongestSubstringTest, FirstAndLastSame) {
  EXPECT_EQ(lengthOfLongestSubstring("abcda"), 4); // "abcd" / "bcda"
}

TEST(LengthOfLongestSubstringTest, HeadDuplicatePair) {
  EXPECT_EQ(lengthOfLongestSubstring("aab"), 2); // "ab"
}

// ============================================================
// 易错用例 — 左界处理（左界需跳跃，且不可回退）
// ============================================================

TEST(LengthOfLongestSubstringTest, LeftBoundaryMustJump) {
  // "dvdf" → "vdf"，左界须跳到重复字符的下一位而非只左移一格
  EXPECT_EQ(lengthOfLongestSubstring("dvdf"), 3);
}

TEST(LengthOfLongestSubstringTest, LeftBoundaryMustNotMoveBack) {
  // "abba" → "ab"/"ba"，处理第 2 个 'a' 时左界不能回退到 0
  EXPECT_EQ(lengthOfLongestSubstring("abba"), 2);
}

TEST(LengthOfLongestSubstringTest, DuplicateAtBothEnds) {
  EXPECT_EQ(lengthOfLongestSubstring("tmmzuxt"), 5); // "mzuxt"
}

TEST(LengthOfLongestSubstringTest, ShrinkThenRegrow) {
  EXPECT_EQ(lengthOfLongestSubstring("abbaa"), 2);
}

TEST(LengthOfLongestSubstringTest, RepeatedBlocks) {
  EXPECT_EQ(lengthOfLongestSubstring("aaabbbccc"), 2); // "ab" / "bc"
}

// ============================================================
// 字符集与规模
// ============================================================

TEST(LengthOfLongestSubstringTest, SpaceIsACharacter) {
  EXPECT_EQ(lengthOfLongestSubstring(" "), 1);
}

TEST(LengthOfLongestSubstringTest, Digits) {
  EXPECT_EQ(lengthOfLongestSubstring("12321"), 3); // "321"
}

TEST(LengthOfLongestSubstringTest, LargeInputLinearTime) {
  // 52 个不同字符循环 1000 次（n = 52000），最长无重复子串恒为 52
  string s;
  s.reserve(52000);
  for (int i = 0; i < 1000; ++i) {
    for (char c = 'a'; c <= 'z'; ++c) {
      s.push_back(c);
    }
    for (char c = 'A'; c <= 'Z'; ++c) {
      s.push_back(c);
    }
  }
  EXPECT_EQ(lengthOfLongestSubstring(s), 52);
}

// ============================================================
// [M] LC 209 长度最小的子数组
// ============================================================

// ------------------------------------------------------------
// 常规用例
// ------------------------------------------------------------

TEST(MinSubArrayLenTest, BasicCase) {
  vector<int> nums = {2, 3, 1, 2, 4, 3};
  EXPECT_EQ(minSubArrayLen(7, nums), 2); // [4,3]
}

TEST(MinSubArrayLenTest, SingleElementWindow) {
  vector<int> nums = {1, 4, 4};
  EXPECT_EQ(minSubArrayLen(4, nums), 1); // [4]
}

TEST(MinSubArrayLenTest, NoSuchSubArray) {
  vector<int> nums = {1, 1, 1, 1, 1, 1, 1, 1}; // 总和 8 < 11
  EXPECT_EQ(minSubArrayLen(11, nums), 0);
}

// ------------------------------------------------------------
// 边界用例
// ------------------------------------------------------------

TEST(MinSubArrayLenTest, SingleElementMiss) {
  vector<int> nums = {1};
  EXPECT_EQ(minSubArrayLen(5, nums), 0);
}

TEST(MinSubArrayLenTest, ExactTotalSum) {
  vector<int> nums = {1, 2, 3}; // 必须取满整个数组
  EXPECT_EQ(minSubArrayLen(6, nums), 3);
}

TEST(MinSubArrayLenTest, WholeArrayRequired) {
  vector<int> nums = {1, 2, 3, 4, 5}; // 总和 15，只有整段达标
  EXPECT_EQ(minSubArrayLen(15, nums), 5);
}

TEST(MinSubArrayLenTest, TotalSumBelowTarget) {
  vector<int> nums = {1, 2, 3}; // 总和 6 < 100
  EXPECT_EQ(minSubArrayLen(100, nums), 0);
}

// ------------------------------------------------------------
// 易错用例
// ------------------------------------------------------------

TEST(MinSubArrayLenTest, BestWindowAtTheEnd) {
  // 最优窗口恰好在最后一个元素处才闭合：[4,3]
  // 若收缩逻辑放在"加入新元素之前"，该窗口永远不会被校验
  vector<int> nums = {1, 1, 1, 1, 4, 3};
  EXPECT_EQ(minSubArrayLen(7, nums), 2);
}

TEST(MinSubArrayLenTest, BestWindowAtHead) {
  // 最优解在头部，验证 left 只增不减不会漏掉前缀答案
  vector<int> nums = {10, 1, 10};
  EXPECT_EQ(minSubArrayLen(11, nums), 2); // [10,1]
}

TEST(MinSubArrayLenTest, ShrinkMultipleTimes) {
  // 单个窗口需连续收缩两次才回到 < target：4 个元素和 12 才达标
  vector<int> nums = {3, 3, 3, 3, 3, 3};
  EXPECT_EQ(minSubArrayLen(10, nums), 4);
}

TEST(MinSubArrayLenTest, WindowMustBeContiguous) {
  // 答案 [3,4] 长度为 2；若误按"子序列"思路会算出更短的组合
  vector<int> nums = {1, 2, 3, 4, 5};
  EXPECT_EQ(minSubArrayLen(6, nums), 2);
}

// ------------------------------------------------------------
// 规模用例（验证不退化）
// ------------------------------------------------------------

TEST(MinSubArrayLenTest, LargeInputLinearTime) {
  // n = 100000，暴力枚举 O(n²) 会超时，正确实现应线性完成
  vector<int> nums(100000, 1);
  EXPECT_EQ(minSubArrayLen(1000, nums), 1000);
}

// ============================================================
// [M] LC 424 替换后的最长重复字符
// ============================================================

// ------------------------------------------------------------
// 常规用例
// ------------------------------------------------------------

TEST(CharacterReplacementTest, BasicCase) {
  EXPECT_EQ(characterReplacement("ABAB", 2), 4); // 全替换成 A 或 B
}

TEST(CharacterReplacementTest, LeetCodeSample) {
  EXPECT_EQ(characterReplacement("AABABBA", 1), 4); // "AABA" / "BABB"
}

TEST(CharacterReplacementTest, ShortWindowExtended) {
  EXPECT_EQ(characterReplacement("ABBB", 1), 4); // A→B，need = 4-3 = 1
}

// ------------------------------------------------------------
// 边界用例
// ------------------------------------------------------------

TEST(CharacterReplacementTest, EmptyString) {
  EXPECT_EQ(characterReplacement("", 0), 0);
}

TEST(CharacterReplacementTest, SingleCharacter) {
  EXPECT_EQ(characterReplacement("A", 0), 1); // 无需替换
}

TEST(CharacterReplacementTest, AllSameCharacters) {
  EXPECT_EQ(characterReplacement("AAAA", 2), 4); // 本来就全同，k 用不上
}

TEST(CharacterReplacementTest, KReplacesAll) {
  EXPECT_EQ(characterReplacement("ABCDE", 5), 5); // need = 5-1 = 4 <= 5
}

// ------------------------------------------------------------
// 易错用例
// ------------------------------------------------------------

TEST(CharacterReplacementTest, BestWindowStartsAtSecondChar) {
  // 最优窗口是 [1..4]="BBBA"（need = 4-3 = 1）；若 max_cnt 随收缩回退会得到 3
  EXPECT_EQ(characterReplacement("ABBBAAA", 1), 4);
}

TEST(CharacterReplacementTest, MajorityLetterChanges) {
  // 最优窗口跨越 A/B 分界："AAAAABB" 长度 7，need = 7-5 = 2
  EXPECT_EQ(characterReplacement("AAAAABBBBB", 2), 7);
}

TEST(CharacterReplacementTest, SingleReplacementUnifies) {
  EXPECT_EQ(characterReplacement("AABAA", 1), 5); // 一个 B 换掉即全 A
}

TEST(CharacterReplacementTest, ZeroKKeepsLongestRun) {
  EXPECT_EQ(characterReplacement("AABBA", 0), 2); // k=0 退化为最长连续同字符
}

// ------------------------------------------------------------
// 规模用例（验证不退化）
// ------------------------------------------------------------

TEST(CharacterReplacementTest, LargeInputLinearTime) {
  // n = 100000，暴力枚举 O(n²) 会超时；末尾一个 'B' 被替换掉
  string s = string(99999, 'A') + "B";
  EXPECT_EQ(characterReplacement(s, 1), 100000);
}
