// ============================================================
// [E] LC 283 移动零 — 单元测试
// ============================================================

#include <gtest/gtest.h>
#include <vector>
using namespace std;

#include "../01_two_pointers.cpp"

TEST(MoveZeroesTest, BasicCase) {
  vector<int> nums = {0, 1, 0, 3, 12};
  vector<int> expected = {1, 3, 12, 0, 0};
  moveZeroes(nums);
  EXPECT_EQ(nums, expected);
}

TEST(MoveZeroesTest, AllZero) {
  vector<int> nums = {0, 0, 0};
  vector<int> expected = {0, 0, 0};
  moveZeroes(nums);
  EXPECT_EQ(nums, expected);
}

TEST(MoveZeroesTest, NoZero) {
  vector<int> nums = {1, 2, 3};
  vector<int> expected = {1, 2, 3};
  moveZeroes(nums);
  EXPECT_EQ(nums, expected);
}

TEST(MoveZeroesTest, SingleZero) {
  vector<int> nums = {0};
  vector<int> expected = {0};
  moveZeroes(nums);
  EXPECT_EQ(nums, expected);
}

TEST(MoveZeroesTest, LeadingAndTrailing) {
  vector<int> nums = {0, 1, 0, 2};
  vector<int> expected = {1, 2, 0, 0};
  moveZeroes(nums);
  EXPECT_EQ(nums, expected);
}

TEST(MoveZeroesTest, PreserveRelativeOrder) {
  vector<int> nums = {4, 0, 2, 0, 1, 0};
  vector<int> expected = {4, 2, 1, 0, 0, 0};
  moveZeroes(nums);
  EXPECT_EQ(nums, expected);
}
