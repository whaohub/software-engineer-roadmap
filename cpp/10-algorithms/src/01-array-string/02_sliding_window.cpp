// ============================================================
// 滑动窗口 (Sliding Window) — 题目框架
// ============================================================
// 两种模式：
//   1. 可变窗口 — 右扩左缩，寻找最优解
//   2. 固定窗口 — 固定大小滑动
// ============================================================

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
using namespace std;

// LC 3 — 无重复字符的最长子串 🟠
// 思路：可变窗口 + 哈希集合记录字符
int lengthOfLongestSubstring(string s);

// LC 209 — 长度最小的子数组 🟠
// 思路：可变窗口，窗口和 ≥ target 时收缩左边界
int minSubArrayLen(int target, vector<int> &nums);

// LC 424 — 替换后的最长重复字符 🟠
// 思路：可变窗口，窗口内最多替换 k 次
int characterReplacement(string s, int k);

// LC 438 — 找到字符串中所有字母异位词 🟠
// 思路：固定窗口 + 字符计数匹配
vector<int> findAnagrams(string s, string p);

int main() {
  cout << "滑动窗口题目框架已就绪" << endl;
  return 0;
}
