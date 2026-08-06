// ============================================================
// 字符串高频技巧 — 实现
// ============================================================

#include <cctype>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

// [E] LC 125 — 验证回文串 🟢
// 如果在将所有大写字符转换为小写字符、并移除所有非字母数字字符之后，短语正着读和反着读都一样。则可以认为该短语是一个
// 回文串 。
// 复杂度：时间 O(n) | 空间 O(1)，双指针一次扫描，n 为字符串长度

/* 字母和数字都属于字母数字字符。

给你一个字符串 s，如果它是 回文串 ，返回 true ；否则，返回 false 。

示例 1：

输入: s = "A man, a plan, a canal: Panama"
输出：true
解释："amanaplanacanalpanama" 是回文串。 */

bool isPalindrome(string s) {

  int left = 0;
  int right = s.length() - 1;

  while (left < right) {
    while (left < right && !isalnum(s[left]))
      left++;

    while (left < right && !isalnum(s[right]))
      right--;

    if (tolower(s[left]) != tolower(s[right]))
      return false;

    left++;
    right--;
  }

  return true;
}

// [E] LC 242 — 有效字母异位词 🟢
// 给定两个字符串 s 和 t，判断 t 是否是 s 的字母异位词
// 复杂度：时间 O(n) | 空间 O(1)，字符频率计数，n 为较长字符串长度

/* 若 s 和 t 中每个字符出现的次数都相同，则称 s 和 t 互为字母异位词。

示例 1：
输入: s = "anagram", t = "nagaram"
输出: true

示例 2：
输入: s = "rat", t = "car"
输出: false */
// 思路:
bool isAnagram(string s, string t) {
  if (s.size() != t.size()) {
    return false;
  }

  int count[26] = {0};

  for (const auto &c : s) {
    count[c - 'a']++;
  }

  for (const auto &c : t) {
    count[c - 'a']--;
  }

  for (int i = 0; i < 26; ++i) {
    if (count[i] != 0)
      return false;
  }
  return true;
}

// [E] LC 14 — 最长公共前缀 🟢
// 编写一个函数来查找字符串数组中的最长公共前缀
// 复杂度：时间 O(m·n) | 空间 O(1)，m 为最短字符串长度，n 为字符串个数

/* 如果不存在公共前缀，返回空字符串 ""。

示例 1：
输入: strs = ["flower", "flow", "flight"]
输出: "fl"

示例 2：
输入: strs = ["dog", "racecar", "car"]
输出: "" */

string longestCommonPrefix(vector<string> &strs) {
  if (strs.empty())
    return "";

  for (int i = 0; i < strs[0].size(); ++i) {
    char c = strs[0][i];

    int len = strs.size();
    for (int j = 1; j < len; ++j) {
      if (i >= strs[j].size() || strs[j][i] != c) {
        return strs[0].substr(0, i);
      }
    }
  }

  return strs[0];
}

// [E] LC 20 — 有效括号 🟢
// 给定一个只包括 '('、')'、'{'、'}'、'['、']' 的字符串 s，判断字符串是否有效
// 复杂度：时间 O(n) | 空间 O(n)，栈辅助，n 为字符串长度

/* 有效字符串需满足：
  1. 左括号必须用相同类型的右括号闭合。
  2. 左括号必须以正确的顺序闭合。
  3. 每个右括号都有一个对应的相同类型的左括号。

示例 1：
输入: s = "()"
输出: true

示例 2：
输入: s = ""([])"
输出: true

示例 3：
输入: s = "(]"
输出: false */

// bug: stack empty 访问
bool isValidV1(string s) {
  std::stack<char> stack;
  for (auto c : s) {
    if (c == '(' || c == '[' || c == '{')
      stack.push(c);
    else if ((c == ')' && stack.top() == '(') ||
             (c == ']' && stack.top() == '[') ||
             (c == '}' && stack.top() == '{'))
      stack.pop();
    else
      return false;
  }

  return stack.empty();
}

bool isValid(string s) {
  stack<char> st;
  std::unordered_map<char, char> pairs = {{')', '('}, {']', '['}, {'}', '{'}};

  for (const auto c : s) {
    if (!pairs[c])
      st.push(c);
    else {
      if (st.empty() || pairs[c] != st.top())
        return false;
      st.pop();
    }
  }
  return st.empty();
}

// [M] LC 5 — 最长回文子串 🟠
// 给你一个字符串 s，找到 s 中最长的回文子串
// 复杂度：时间 O(n²) | 空间 O(1)，中心扩展法；更优：Manacher 算法 O(n)

/* 示例 1：
输入: s = "babad"
输出: "bab"
解释: "aba" 同样是符合题意的答案。

示例 2：
输入: s = "cbbd"
输出: "bb" */

string longestPalindrome(string s) {
  // TODO: 实现
  return "";
}
