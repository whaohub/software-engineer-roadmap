// ============================================================
// [E] LC 27 移除元素 — 单元测试
// ============================================================

#include <gtest/gtest.h>
#include <vector>
using namespace std;

#include "../01_two_pointers.cpp"

TEST(RemoveElementTest, BasicCase) {
  vector<int> nums = {3, 2, 2, 3};
  int k = removeElement(nums, 3);
  EXPECT_EQ(k, 2);
  EXPECT_EQ(nums[0], 2);
  EXPECT_EQ(nums[1], 2);
}

TEST(RemoveElementTest, MultipleOccurrences) {
  vector<int> nums = {0, 1, 2, 2, 3, 0, 4, 2};
  int k = removeElement(nums, 2);
  EXPECT_EQ(k, 5);
  vector<int> expected = {0, 1, 3, 0, 4};
  for (int i = 0; i < k; i++)
    EXPECT_EQ(nums[i], expected[i]);
}

TEST(RemoveElementTest, NoMatch) {
  vector<int> nums = {1, 2, 3};
  int k = removeElement(nums, 4);
  EXPECT_EQ(k, 3);
  for (int i = 0; i < k; i++)
    EXPECT_EQ(nums[i], i + 1);
}

TEST(RemoveElementTest, AllMatch) {
  vector<int> nums = {3, 3, 3};
  EXPECT_EQ(removeElement(nums, 3), 0);
}

TEST(RemoveElementTest, SingleMatch) {
  vector<int> nums = {1, 2, 3};
  int k = removeElement(nums, 2);
  EXPECT_EQ(k, 2);
  for (int i = 0; i < k; i++)
    EXPECT_NE(nums[i], 2);
}

TEST(RemoveElementTest, EmptyArray) {
  vector<int> nums = {};
  EXPECT_EQ(removeElement(nums, 0), 0);
}
