// ============================================================
// 双指针 (Two Pointers) — 题目框架
// ============================================================
// 三种模式：
//   1. 左右指针 (相向而行) — 有序数组
//   2. 快慢指针 (同向而行) — 原地修改
//   3. 定一移二 (固定 + 双向) — 多元素组合
// ============================================================

#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

#ifdef USE_GTEST
#include <gtest/gtest.h>
#endif

// ============================================================
// 模式一：左右指针
// ============================================================

// LC 167 — 两数之和 II (有序数组) 🟢
// 题目：已按升序排列的整数数组，找出两个数使其和等于 target
// 要求：返回两数的下标（从 1 开始），唯一解，不得重复使用同一元素
// 约束：O(1) 额外空间，数组长度 [2, 3×10^4]
// 思路：左右指针向中间逼近，sum < target 左移左指针，> target 右移右指针
vector<int> twoSum(vector<int> &numbers, int target) {
  int left = 0, right = numbers.size() - 1;
  while (left < right) {
    auto result = numbers[left] + numbers[right];
    if (result == target) {
      return {left, right};
    }
    if (numbers[left] + numbers[right] > target) {
      right--;
    } else {
      left++;
    }
  }
  return {-1, -1};
};

// LC 11 — 盛最多水的容器 🟠
// 题目：数组 height 表示垂直线高度，选两条线与 x 轴构成容器
// 要求：返回容器能盛的最大水量（面积 = 宽度 × 较短线高度）
// 约束：n ∈ [2, 10^5]，不可倾斜容器
// 思路：左右指针从两端向中间移动，每次移动较短的那条线（因为移动长线水量只会减少）
int maxArea(vector<int> &height);

// ============================================================
// 模式二：快慢指针
// ============================================================

// LC 26 — 删除有序数组重复项 🟢
// 题目：原地删除有序数组中的重复项，使每个元素只出现一次
// 要求：返回新长度 k，前 k 个位置存放不重复元素（顺序不变）
// 约束：O(1) 额外空间，数组已非降序排列
// 思路：slow 标记不重复元素待插入位置，fast 遍历扫描新元素
int removeDuplicates(vector<int> &nums);

// LC 27 — 移除元素 🟢
// 题目：原地移除数组中所有等于 val 的元素
// 要求：返回新长度 k，前 k 个位置存放不等于 val 的元素（顺序不限）
// 约束：O(1) 额外空间，数组长度 [0, 100]
// 思路：slow 标记非 val 元素待插入位置，fast 遍历数组
int removeElement(vector<int> &nums, int val);

// LC 283 — 移动零 🟢
// 题目：将所有 0 移到数组末尾，同时保持非零元素的相对顺序
// 要求：原地操作，不得复制数组
// 约束：O(1) 额外空间，数组长度 [1, 10^4]
// 思路：快指针遍历数组，将非零元素交换到慢指针位置，遍历完成后所有 0 自然在末尾
void moveZeroes(vector<int> &nums);

// ============================================================
// 模式三：定一移二
// ============================================================

// LC 15 — 三数之和 🟠
// 题目：找出数组中所有和为 0 且不重复的三元组 [nums[i], nums[j], nums[k]]
// 要求：i、j、k 互不相同，返回所有满足条件的组合（不可包含重复三元组）
// 约束：数组长度 [3, 3000]，值范围 [-10^5, 10^5]
// 思路：排序 + 固定 i（最外层去重）+ 左右双指针找两数之和
//       三重去重：i 跳过同值，左指针跳过同值，右指针跳过同值
vector<vector<int>> threeSum(vector<int> &nums);

// ============================================================
// 测试用例 (gtest)
// ============================================================
#ifdef USE_GTEST

// ---- twoSum ----
TEST(TwoSumTest, BasicCase) {
  vector<int> nums = {2, 7, 11, 15};
  vector<int> expected = {0, 1};
  EXPECT_EQ(twoSum(nums, 9), expected);
}

TEST(TwoSumTest, WithNegativeNumbers) {
  vector<int> nums = {-3, -1, 0, 1, 2};
  vector<int> expected = {0, 3}; // -3 + 1 = -2
  EXPECT_EQ(twoSum(nums, -2), expected);
}

TEST(TwoSumTest, TargetAtEnds) {
  vector<int> nums = {1, 2, 3, 4, 5};
  vector<int> expected = {0, 4}; // 1 + 5 = 6
  EXPECT_EQ(twoSum(nums, 6), expected);
}

TEST(TwoSumTest, AdjacentElements) {
  vector<int> nums = {3, 4, 5, 6};
  vector<int> expected = {2, 3}; // 5 + 6 = 11
  EXPECT_EQ(twoSum(nums, 11), expected);
}

TEST(TwoSumTest, NoSolution) {
  vector<int> nums = {1, 2, 3, 4};
  vector<int> expected = {-1, -1};
  EXPECT_EQ(twoSum(nums, 100), expected);
}

// ---- maxArea (占位) ----
// TODO: 实现 maxArea 后补充测试

#endif

// ============================================================
// 主入口
// ============================================================
int main(int argc, char *argv[]) {
#ifdef USE_GTEST
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
#else
  cout << "双指针题目框架已就绪" << endl;
  return 0;
#endif
}
