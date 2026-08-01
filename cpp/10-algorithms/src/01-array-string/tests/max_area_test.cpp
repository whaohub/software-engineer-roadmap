// ============================================================
// [M] LC 11 盛最多水的容器 — 单元测试
// ============================================================

#include <gtest/gtest.h>
#include <vector>
using namespace std;

#include "../01_two_pointers.cpp"

TEST(MaxAreaTest, BasicCase) {
  vector<int> height = {1, 8, 6, 2, 5, 4, 8, 3, 7};
  EXPECT_EQ(maxArea(height), 49);
}

TEST(MaxAreaTest, TwoElements) {
  vector<int> height = {1, 1};
  EXPECT_EQ(maxArea(height), 1);
}

TEST(MaxAreaTest, Ascending) {
  vector<int> height = {1, 2, 4, 3};
  EXPECT_EQ(maxArea(height), 4);
}

TEST(MaxAreaTest, Descending) {
  vector<int> height = {4, 3, 2, 1};
  EXPECT_EQ(maxArea(height), 4);
}

TEST(MaxAreaTest, AllSame) {
  vector<int> height = {1, 1, 1, 1};
  EXPECT_EQ(maxArea(height), 3);
}

TEST(MaxAreaTest, PeakInMiddle) {
  vector<int> height = {1, 2, 1};
  EXPECT_EQ(maxArea(height), 2);
}

TEST(MaxAreaTest, TallShortTall) {
  vector<int> height = {10, 1, 1, 10};
  EXPECT_EQ(maxArea(height), 30);
}
