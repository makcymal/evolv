#include <vector>

#include "gtest/gtest.h"
#include "src/impl/fenwick_tree.h"

using namespace evolv::internal;


// FenwickTreeTest is the suite without particular values

TEST(FenwickTreeTest, ConstructEmpty) {
  FenwickTree<int> ft;
  EXPECT_EQ(ft.Size(), 0);
}

TEST(FenwickTreeTest, ConstructZero) {
  FenwickTree<int> ft(10);
  EXPECT_EQ(ft.Size(), 10);
  EXPECT_EQ(ft.Sum(0, 0), 0);
  EXPECT_EQ(ft.Sum(9, 9), 0);
  EXPECT_EQ(ft.Sum(0, 9), 0);
}

TEST(FenwickTreeTest, AddValues) {
  FenwickTree<int> ft(1);
  EXPECT_EQ(ft.Sum(0, 0), 0);
  ft.Add(0, 7);
  EXPECT_EQ(ft.Sum(0, 0), 7);
}

TEST(FenwickTreeTest, AddSquares) {
  FenwickTree<int> ft(10);
  for (int i = 0; i < ft.Size(); ++i) {
    ft.Add(i, i * i);
  }
  EXPECT_EQ(ft.Sum(0, 0), 0);
  EXPECT_EQ(ft.Sum(1, 1), 1);
  EXPECT_EQ(ft.Sum(2, 2), 4);
  EXPECT_EQ(ft.Sum(4, 4), 16);
  EXPECT_EQ(ft.Sum(9, 9), 81);
  EXPECT_EQ(ft.Sum(1, 5), 1 + 4 + 9 + 16 + 25);
  EXPECT_EQ(ft.Sum(8, 9), 64 + 81);
}

TEST(FenwickTreeTest, AddCubics) {
  FenwickTree<int> ft(10);
  for (int i = 0; i < ft.Size(); ++i) {
    ft.Add(i, i * i * i);
  }
  EXPECT_EQ(ft.Sum(0, 9), 2025);
  EXPECT_EQ(ft.Sum(1, 9), 2025);
}


// FenwickTreeFixtTest is the suite for FenwickTree built on NUMS

//                    index 0  1  2  3  4  5  6  7  8  9
//               prefix sum 0  3  8  9 13 16 18 22 22 23
const std::vector<int> NUMS{0, 3, 5, 1, 4, 3, 2, 4, 0, 1};

class FenwickTreeFixtTest : public testing::Test {
 protected:
  FenwickTreeFixtTest()
      : nums(NUMS.begin(), NUMS.end()),
        pref(nums.begin(), nums.end()),
        ft(nums.size()) {
    ft.Add(0, nums[0]);
    for (int i = 1; i < (int)pref.size(); ++i) {
      pref[i] += pref[i - 1];
      ft.Add(i, nums[i]);
    }
  }

  std::vector<int> nums, pref;
  FenwickTree<int> ft;
};

TEST_F(FenwickTreeFixtTest, SumOverPrefix) {
  EXPECT_EQ(ft.Sum(0), pref[0]);
  EXPECT_EQ(ft.Sum(1), pref[1]);
  EXPECT_EQ(ft.Sum(2), pref[2]);
  EXPECT_EQ(ft.Sum(3), pref[3]);
  EXPECT_EQ(ft.Sum(4), pref[4]);
  EXPECT_EQ(ft.Sum(5), pref[5]);
  EXPECT_EQ(ft.Sum(6), pref[6]);
  EXPECT_EQ(ft.Sum(7), pref[7]);
  EXPECT_EQ(ft.Sum(8), pref[8]);
  EXPECT_EQ(ft.Sum(9), pref[9]);
}

TEST_F(FenwickTreeFixtTest, SumOverSegment) {
  EXPECT_EQ(ft.Sum(0, 0), pref[0]);
  EXPECT_EQ(ft.Sum(0, 1), pref[1]);
  EXPECT_EQ(ft.Sum(0, 5), pref[5]);
  EXPECT_EQ(ft.Sum(1, 7), pref[7] - pref[0]);
  EXPECT_EQ(ft.Sum(2, 4), pref[4] - pref[1]);
  EXPECT_EQ(ft.Sum(6, 9), pref[9] - pref[5]);
}

TEST_F(FenwickTreeFixtTest, UpperBound) {
  EXPECT_EQ(ft.UpperBound(-1), 0);
  EXPECT_EQ(ft.UpperBound(0), 1);
  EXPECT_EQ(ft.UpperBound(1), 1);
  EXPECT_EQ(ft.UpperBound(3), 2);
  EXPECT_EQ(ft.UpperBound(15), 5);
  EXPECT_EQ(ft.UpperBound(18), 7);
  EXPECT_EQ(ft.UpperBound(21), 7);
  EXPECT_EQ(ft.UpperBound(22), 9);
  EXPECT_EQ(ft.UpperBound(23), 10);
}
