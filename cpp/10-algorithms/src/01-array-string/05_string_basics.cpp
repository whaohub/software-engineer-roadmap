// ============================================================
// 字符串高频技巧 — 题目框架
// ============================================================

#include <iostream>
#include <stack>
#include <string>
#include <vector>
using namespace std;

// LC 125 — 验证回文串 🟢
// 思路：前后双指针，跳过非字母数字
bool isPalindrome(string s);

// LC 242 — 有效字母异位词 🟢
// 思路：int[26] 字符计数数组
bool isAnagram(string s, string t);

// LC 14 — 最长公共前缀 🟢
// 思路：纵向扫描，逐列比较所有字符串
string longestCommonPrefix(vector<string> &strs);

// LC 20 — 有效括号 🟢
// 思路：栈匹配，左括号入栈，右括号匹配则出栈
bool isValid(string s);

// LC 5 — 最长回文子串 🟠
// 思路：中心扩展法，奇偶两种中心向两边扩散
string longestPalindrome(string s);

int main() {
  cout << "字符串高频技巧题目框架已就绪" << endl;
  return 0;
}
