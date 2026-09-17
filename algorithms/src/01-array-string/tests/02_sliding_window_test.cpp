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
