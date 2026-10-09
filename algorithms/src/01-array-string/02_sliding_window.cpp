// ============================================================
// 滑动窗口 (Sliding Window) — 实现
// ============================================================
// 两种模式：
//   1. 可变窗口 — 右扩左缩
//   2. 固定窗口 — 固定大小滑动
// ============================================================

#include <algorithm>
#include <climits>
#include <cstddef>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
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
    std::unordered_set<char> seen;
    for (int j = i; j < s.length(); ++j) {
      if (seen.count(s[j])) {
        break;
      }
      seen.insert(s[j]);
    }
    max_len = std::max(max_len, static_cast<int>(seen.size()));
  }

  return max_len;
}

// BF: 第一轮计算bc后,下一轮依然计算,每次会重复计算
// 优化: s[left...right] 中没有重复字符，并且 seen 恰好保存这个窗口里的字符。
// - 窗口有效：没有重复字符。
// - 遇到重复字符：收缩左边界，直到恢复有效。
// - 窗口有效时：更新最大长度
int lengthOfLongestSubstring(string s) {
  int max_len = 0;

  size_t left = 0;

  std::unordered_set<char> seen;
  for (size_t right = 0; right < s.length(); ++right) {
    while (seen.count(s[right])) {
      seen.erase(left);
      left++;
    }
    seen.insert(s[right]);
    max_len = std::max(max_len, static_cast<int>(seen.size()));
  }

  return max_len;
}

// [M] LC 209 — 长度最小的子数组 🟠
// 找出该数组中满足其总和 大于等于 target 的长度最小的 连续子数组
// 输入：target = 7, nums = [2,3,1,2,4,3]
// 输出：2
// 解释：子数组 [4,3] 是该条件下的长度最小的子数组。
// - 窗口有效: sum >= target
// - 遇到满足: 收缩左边界
int minSubArrayLen(int target, vector<int> &nums) {

  int min_len = INT_MAX;
  int sum = 0;
  size_t left = 0;
  for (size_t right = 0; right < nums.size(); ++right) {
    sum += nums[right];
    while (sum >= target) {
      min_len = std::min(min_len, static_cast<int>(right - left + 1));
      sum -= nums[left];
      left++;
    }
  }
  return min_len == INT_MAX ? 0 : min_len;
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
