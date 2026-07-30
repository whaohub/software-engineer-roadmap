// ============================================================
// 前缀和 — 单元测试
// ============================================================

#include <gtest/gtest.h>
#include <vector>
using namespace std;

#include "../03_prefix_sum.cpp"

// [M] LC 53
TEST(MaxSubArrayTest, BasicCase) {
  vector<int> nums = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
  EXPECT_EQ(maxSubArray(nums), 6);
}

TEST(MaxSubArrayTest, AllNegative) {
  vector<int> nums = {-1, -2, -3};
  EXPECT_EQ(maxSubArray(nums), -1);
}

// [M] LC 238
TEST(ProductExceptSelfTest, BasicCase) {
  vector<int> nums = {1, 2, 3, 4};
  vector<int> expected = {24, 12, 8, 6};
  EXPECT_EQ(productExceptSelf(nums), expected);
}

// [M] LC 560
TEST(SubarraySumTest, BasicCase) {
  vector<int> nums = {1, 1, 1};
  EXPECT_EQ(subarraySum(nums, 2), 2);
}

TEST(SubarraySumTest, WithNegative) {
  vector<int> nums = {1, -1, 0};
  EXPECT_EQ(subarraySum(nums, 0), 3);
}
