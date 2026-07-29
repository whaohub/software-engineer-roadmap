// ============================================================
// 前缀和 (Prefix Sum) — 题目框架
// ============================================================

#include <iostream>
#include <unordered_map>
#include <vector>
using namespace std;

// LC 53 — 最大子数组和 🟠
// 思路：Kadane 算法（DP），记录以 i 结尾的最大子数组和
int maxSubArray(vector<int> &nums);

// LC 238 — 除自身以外数组的乘积 🟠
// 思路：前缀积 × 后缀积
vector<int> productExceptSelf(vector<int> &nums);

// LC 560 — 和为 K 的子数组 🟠
// 思路：前缀和 + 哈希表计数
int subarraySum(vector<int> &nums, int k);

int main() {
  cout << "前缀和题目框架已就绪" << endl;
  return 0;
}
