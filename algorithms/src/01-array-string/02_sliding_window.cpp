// ============================================================
// 滑动窗口 (Sliding Window) — 实现
// ============================================================
// 两种模式：
//   1. 可变窗口 — 右扩左缩
//   2. 固定窗口 — 固定大小滑动
// ============================================================

#include <algorithm>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

// [M] LC 3 — 无重复字符的最长子串 🟠
//
// 题目：
// 给定字符串 s，找出其中不含重复字符的最长子串，返回其长度。
// 子串必须是连续的字符序列。
//
// 示例：
// "abcabcbb" → 3   // "abc"
// "bbbbb"    → 1   // "b"
// "pwwkew"   → 3   // "wke"
// ""         → 0
//
// 要求：
// 时间复杂度：O(n)
// 空间复杂度：O(n) 以内

int lengthOfLongestSubstringBF(string s) {

  int max_len = 0;
  for (size_t i = 0; i < s.length(); ++i) {
    std::set<char> sub_str{s[i]};
    int j = i + 1;
    while (sub_str.count(s[j])) {
      sub_str.insert(s[j]);
    }
    max_len = std::max(max_len, static_cast<int>(sub_str.size()));
  }

  return max_len;
}

// [M] LC 209 — 长度最小的子数组 🟠
int minSubArrayLen(int target, vector<int> &nums) {
  // TODO: 实现
  return 0;
}

// [M] LC 424 — 替换后的最长重复字符 🟠
int characterReplacement(string s, int k) {
  // TODO: 实现
  return 0;
}

// [M] LC 438 — 找到字符串中所有字母异位词 🟠
vector<int> findAnagrams(string s, string p) {
  // TODO: 实现
  return {};
}
