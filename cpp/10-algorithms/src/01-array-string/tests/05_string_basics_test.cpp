// ============================================================
// 字符串高频技巧 — 单元测试
// ============================================================

#include <gtest/gtest.h>
#include <string>
#include <vector>
using namespace std;

#include "../05_string_basics.cpp"

// [E] LC 125
TEST(IsPalindromeTest, BasicCase) {
  EXPECT_TRUE(isPalindrome("A man, a plan, a canal: Panama"));
}

TEST(IsPalindromeTest, NotPalindrome) {
  EXPECT_FALSE(isPalindrome("race a car"));
}

TEST(IsPalindromeTest, EmptyString) { EXPECT_TRUE(isPalindrome("")); }

// [E] LC 242
TEST(IsAnagramTest, BasicCase) { EXPECT_TRUE(isAnagram("anagram", "nagaram")); }

TEST(IsAnagramTest, NotAnagram) { EXPECT_FALSE(isAnagram("rat", "car")); }

// [E] LC 14
TEST(LongestCommonPrefixTest, BasicCase) {
  vector<string> strs = {"flower", "flow", "flight"};
  EXPECT_EQ(longestCommonPrefix(strs), "fl");
}

TEST(LongestCommonPrefixTest, NoCommon) {
  vector<string> strs = {"dog", "racecar", "car"};
  EXPECT_EQ(longestCommonPrefix(strs), "");
}

// [E] LC 20
TEST(IsValidTest, BasicCase) { EXPECT_TRUE(isValid("()[]{}")); }

TEST(IsValidTest, InvalidOrder) { EXPECT_FALSE(isValid("(]")); }

TEST(IsValidTest, Nested) { EXPECT_TRUE(isValid("({[]})")); }

// [M] LC 5
TEST(LongestPalindromeTest, BasicCase) {
  EXPECT_EQ(longestPalindrome("babad"), "bab");
}

TEST(LongestPalindromeTest, EvenLength) {
  EXPECT_EQ(longestPalindrome("cbbd"), "bb");
}
