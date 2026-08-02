// ============================================================
// [M] LC 15 三数之和 — 单元测试
// ============================================================

#include <gtest/gtest.h>
#include <vector>
using namespace std;

#include "../01_two_pointers.cpp"

TEST(ThreeSumTest, BasicCase) {
  vector<int> nums = {-1, 0, 1, 2, -1, -4};
  vector<vector<int>> expected = {{-1, -1, 2}, {-1, 0, 1}};
  EXPECT_EQ(threeSum(nums), expected);
}

TEST(ThreeSumTest, NoSolution) {
  vector<int> nums = {1, 2, 3};
  vector<vector<int>> expected = {};
  EXPECT_EQ(threeSum(nums), expected);
}

TEST(ThreeSumTest, AllZero) {
  vector<int> nums = {0, 0, 0};
  vector<vector<int>> expected = {{0, 0, 0}};
  EXPECT_EQ(threeSum(nums), expected);
}

TEST(ThreeSumTest, DeduplicateDuplicates) {
  vector<int> nums = {-2, 0, 0, 2, 2};
  vector<vector<int>> expected = {{-2, 0, 2}};
  EXPECT_EQ(threeSum(nums), expected);
}

TEST(ThreeSumTest, EmptyArray) {
  vector<int> nums = {};
  vector<vector<int>> expected = {};
  EXPECT_EQ(threeSum(nums), expected);
}

TEST(ThreeSumTest, TooFewElements) {
  vector<int> nums = {1, 2};
  vector<vector<int>> expected = {};
  EXPECT_EQ(threeSum(nums), expected);
}

TEST(ThreeSumTest, NegativeOnly) {
  vector<int> nums = {-4, -3, -2, -1};
  vector<vector<int>> expected = {};
  EXPECT_EQ(threeSum(nums), expected);
}
