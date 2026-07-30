// ============================================================
// 双指针 — 单元测试
// ============================================================

#include <gtest/gtest.h>
#include <vector>
using namespace std;

#include "../01_two_pointers.cpp"

// ---- [E] twoSum (LC 167) ----
TEST(TwoSumTest, BasicCase) {
  vector<int> nums = {2, 7, 11, 15};
  vector<int> expected = {0, 1};
  EXPECT_EQ(twoSum(nums, 9), expected);
}

TEST(TwoSumTest, WithNegativeNumbers) {
  vector<int> nums = {-3, -1, 0, 1, 2};
  vector<int> expected = {0, 3};
  EXPECT_EQ(twoSum(nums, -2), expected);
}

TEST(TwoSumTest, TargetAtEnds) {
  vector<int> nums = {1, 2, 3, 4, 5};
  vector<int> expected = {0, 4};
  EXPECT_EQ(twoSum(nums, 6), expected);
}

TEST(TwoSumTest, AdjacentElements) {
  vector<int> nums = {3, 4, 5, 6};
  vector<int> expected = {2, 3};
  EXPECT_EQ(twoSum(nums, 11), expected);
}

TEST(TwoSumTest, NoSolution) {
  vector<int> nums = {1, 2, 3, 4};
  vector<int> expected = {-1, -1};
  EXPECT_EQ(twoSum(nums, 100), expected);
}

// ---- [M] maxArea (LC 11, 待实现) ----
// TODO: 实现 maxArea 后补充测试
