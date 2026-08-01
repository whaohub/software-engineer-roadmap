// ============================================================
// [E] LC 26 删除有序数组重复项 — 单元测试
// ============================================================

#include <gtest/gtest.h>
#include <vector>
using namespace std;

#include "../01_two_pointers.cpp"

TEST(RemoveDuplicatesTest, BasicCase) {
  vector<int> nums = {1, 1, 2};
  int k = removeDuplicates(nums);
  EXPECT_EQ(k, 2);
  vector<int> expected = {1, 2};
  for (int i = 0; i < k; i++)
    EXPECT_EQ(nums[i], expected[i]);
}

TEST(RemoveDuplicatesTest, ManyDuplicates) {
  vector<int> nums = {0, 0, 1, 1, 1, 2, 2, 3, 3, 4};
  int k = removeDuplicates(nums);
  EXPECT_EQ(k, 5);
  vector<int> expected = {0, 1, 2, 3, 4};
  for (int i = 0; i < k; i++)
    EXPECT_EQ(nums[i], expected[i]);
}

TEST(RemoveDuplicatesTest, NoDuplicates) {
  vector<int> nums = {1, 2, 3};
  int k = removeDuplicates(nums);
  EXPECT_EQ(k, 3);
  vector<int> expected = {1, 2, 3};
  for (int i = 0; i < k; i++)
    EXPECT_EQ(nums[i], expected[i]);
}

TEST(RemoveDuplicatesTest, SingleElement) {
  vector<int> nums = {1};
  int k = removeDuplicates(nums);
  EXPECT_EQ(k, 1);
  EXPECT_EQ(nums[0], 1);
}

TEST(RemoveDuplicatesTest, AllSame) {
  vector<int> nums = {5, 5, 5, 5};
  int k = removeDuplicates(nums);
  EXPECT_EQ(k, 1);
  EXPECT_EQ(nums[0], 5);
}

TEST(RemoveDuplicatesTest, EmptyArray) {
  vector<int> nums = {};
  EXPECT_EQ(removeDuplicates(nums), 0);
}
