// ============================================================
// 滑动窗口 — 单元测试
// ============================================================

#include <gtest/gtest.h>
#include <string>
#include <vector>
using namespace std;

#include "../02_sliding_window.cpp"

// [M] LC 3
TEST(LengthOfLongestSubstringTest, BasicCase) {
  EXPECT_EQ(lengthOfLongestSubstring("abcabcbb"), 3);
}

TEST(LengthOfLongestSubstringTest, AllSame) {
  EXPECT_EQ(lengthOfLongestSubstring("bbbbb"), 1);
}

TEST(LengthOfLongestSubstringTest, EmptyString) {
  EXPECT_EQ(lengthOfLongestSubstring(""), 0);
}

// [M] LC 209
TEST(MinSubArrayLenTest, BasicCase) {
  vector<int> nums = {2, 3, 1, 2, 4, 3};
  EXPECT_EQ(minSubArrayLen(7, nums), 2);
}

TEST(MinSubArrayLenTest, NoSolution) {
  vector<int> nums = {1, 1, 1, 1};
  EXPECT_EQ(minSubArrayLen(100, nums), 0);
}

// [M] LC 424
TEST(CharacterReplacementTest, BasicCase) {
  EXPECT_EQ(characterReplacement("ABAB", 2), 4);
}

// [M] LC 438
TEST(FindAnagramsTest, BasicCase) {
  vector<int> expected = {0, 6};
  EXPECT_EQ(findAnagrams("cbaebabacd", "abc"), expected);
}
