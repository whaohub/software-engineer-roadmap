// ============================================================
// 字符串高频技巧 — 单元测试
// 覆盖: isPalindrome / isAnagram / longestCommonPrefix
//       isValid / longestPalindrome
// ============================================================

#include <gtest/gtest.h>
#include <string>
#include <vector>
using namespace std;

#include "../05_string_basics.cpp"

// ============================================================
// [E] LC 125 验证回文串
// ============================================================
TEST(IsPalindromeTest, ClassicPhrase) {
  EXPECT_TRUE(isPalindrome("A man, a plan, a canal: Panama"));
}

TEST(IsPalindromeTest, NotPalindrome) {
  EXPECT_FALSE(isPalindrome("race a car"));
}

TEST(IsPalindromeTest, PunctuationOnly) {
  EXPECT_TRUE(isPalindrome(".,")); // 过滤后为空 → 回文
}

TEST(IsPalindromeTest, EmptyString) {
  EXPECT_TRUE(isPalindrome("")); // 空串 → 回文
}

TEST(IsPalindromeTest, SingleCharacter) { EXPECT_TRUE(isPalindrome("a")); }

TEST(IsPalindromeTest, CaseInsensitive) { EXPECT_TRUE(isPalindrome("aA")); }

TEST(IsPalindromeTest, Digits) {
  EXPECT_FALSE(isPalindrome("0P")); // '0'(48) vs 'p'(112)
  EXPECT_TRUE(isPalindrome("0P0"));
}

TEST(IsPalindromeTest, AlreadyLowerEven) { EXPECT_TRUE(isPalindrome("abba")); }

TEST(IsPalindromeTest, AlreadyLowerOdd) { EXPECT_TRUE(isPalindrome("aba")); }

TEST(IsPalindromeTest, MixedSpaces) {
  EXPECT_TRUE(isPalindrome("Never odd or even"));
}

// ============================================================
// [E] LC 242 有效字母异位词
// ============================================================
TEST(IsAnagramTest, BasicTrue) { EXPECT_TRUE(isAnagram("anagram", "nagaram")); }

TEST(IsAnagramTest, BasicFalse) { EXPECT_FALSE(isAnagram("rat", "car")); }

TEST(IsAnagramTest, BothEmpty) { EXPECT_TRUE(isAnagram("", "")); }

TEST(IsAnagramTest, DifferentLength) { EXPECT_FALSE(isAnagram("a", "ab")); }

TEST(IsAnagramTest, SameLettersDifferentCount) {
  EXPECT_FALSE(isAnagram("aabb", "ab"));
}

TEST(IsAnagramTest, SameCharsReordered) {
  EXPECT_TRUE(isAnagram("aacc", "ccaa"));
}

// ============================================================
// [E] LC 14 最长公共前缀
// ============================================================
TEST(LongestCommonPrefixTest, CommonPrefix) {
  vector<string> strs = {"flower", "flow", "flight"};
  EXPECT_EQ(longestCommonPrefix(strs), "fl");
}

TEST(LongestCommonPrefixTest, NoCommonPrefix) {
  vector<string> strs = {"dog", "racecar", "car"};
  EXPECT_EQ(longestCommonPrefix(strs), "");
}

TEST(LongestCommonPrefixTest, SingleString) {
  vector<string> strs = {"a"};
  EXPECT_EQ(longestCommonPrefix(strs), "a");
}

TEST(LongestCommonPrefixTest, EmptyStringElement) {
  vector<string> strs = {"", "b"};
  EXPECT_EQ(longestCommonPrefix(strs), "");
}

TEST(LongestCommonPrefixTest, OneIsPrefixOfOther) {
  vector<string> strs = {"ab", "a"};
  EXPECT_EQ(longestCommonPrefix(strs), "a");
}

TEST(LongestCommonPrefixTest, AllSame) {
  vector<string> strs = {"aaa", "aa", "aaa"};
  EXPECT_EQ(longestCommonPrefix(strs), "aa");
}

TEST(LongestCommonPrefixTest, EmptyVector) {
  vector<string> strs;
  EXPECT_EQ(longestCommonPrefix(strs), "");
}

// ============================================================
// [E] LC 20 有效括号
// ============================================================
TEST(IsValidTest, SinglePair) { EXPECT_TRUE(isValid("()")); }

TEST(IsValidTest, AllPairs) { EXPECT_TRUE(isValid("()[]{}")); }

TEST(IsValidTest, WrongOrder) { EXPECT_FALSE(isValid("(]")); }

TEST(IsValidTest, InterleavedMismatch) { EXPECT_FALSE(isValid("([)]")); }

TEST(IsValidTest, NestedValid) { EXPECT_TRUE(isValid("{[]}")); }

TEST(IsValidTest, EmptyString) { EXPECT_TRUE(isValid("")); }

TEST(IsValidTest, Unclosed) { EXPECT_FALSE(isValid("(")); }

TEST(IsValidTest, ExtraClosing) { EXPECT_FALSE(isValid("]")); }

TEST(IsValidTest, LongValid) { EXPECT_TRUE(isValid("{[()()]()}")); }

// ============================================================
// [M] LC 5 最长回文子串
// ============================================================
TEST(LongestPalindromeTest, UniqueEvenAnswer) {
  EXPECT_EQ(longestPalindrome("cbbd"), "bb");
}

TEST(LongestPalindromeTest, WholeStringPalindrome) {
  EXPECT_EQ(longestPalindrome("racecar"), "racecar");
}

TEST(LongestPalindromeTest, SingleChar) {
  EXPECT_EQ(longestPalindrome("a"), "a");
}

TEST(LongestPalindromeTest, TwoSameChars) {
  EXPECT_EQ(longestPalindrome("bb"), "bb");
}

TEST(LongestPalindromeTest, NoPalOverOne) {
  string s = longestPalindrome("ab");
  EXPECT_EQ(s.length(), 1u); // "a" 或 "b" 均合法
}

TEST(LongestPalindromeTest, LongestAtEnd) {
  EXPECT_EQ(longestPalindrome("abbc"), "bb");
}
