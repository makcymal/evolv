#pragma once

#include <deque>
#include <memory>
#include <vector>

#include "gtest/gtest.h"
#include "instantiate.h"

using namespace evolv::internal;


TEST(RemberChainTest, GetMemory1AfterFeedingSequence) {
  std::vector<int> seq{0, 1, 0, 1, 0, 1, 0};
  RemberChain<int> chain(1, RANDOM_STATE);
  chain.FeedSequenceImpl(seq.begin(), seq.end(), true);
  std::deque mem{0, 1};
  EXPECT_EQ(chain.GetMemory(), mem);

  chain.FeedSequenceImpl(seq.begin(), seq.begin() + 1, true);
  mem = {0, 0};
  EXPECT_EQ(chain.GetMemory(), mem);
}


TEST(RemberChainTest, GetMemory2AfterFeedingSequence) {
  std::vector<int> seq{0, 2, 1, 4, 2, 4, 3, 2, 2, 3, 0};
  RemberChain<int> chain(2, RANDOM_STATE);
  chain.FeedSequenceImpl(seq.begin(), seq.end(), true);
  std::deque mem{0, 3, 2};
  EXPECT_EQ(chain.GetMemory(), mem);

  chain.FeedSequenceImpl(seq.begin() + 1, seq.begin() + 5, true);
  mem = {2, 4, 1};
  EXPECT_EQ(chain.GetMemory(), mem);

  chain.FeedSequenceImpl(seq.begin() + 5, seq.begin() + 8, true);
  mem = {2, 3, 4};
  EXPECT_EQ(chain.GetMemory(), mem);

  chain.FeedSequenceImpl(seq.begin() + 8, seq.begin() + 10, true);
  mem = {3, 2, 2};
  EXPECT_EQ(chain.GetMemory(), mem);

  chain.FeedSequenceImpl(seq.begin() + 10, seq.begin() + 11, true);
  mem = {0, 3, 2};
  EXPECT_EQ(chain.GetMemory(), mem);

  chain.FeedSequenceImpl(seq.begin(), seq.end());  // with update_memory = false
  EXPECT_EQ(chain.GetMemory(), mem);
}


TEST(RemberChainTest, GetMemoryAfterUpdateMemory) {
  RemberChain<int> chain(2, RANDOM_STATE);
  std::deque update_mem{0, 3, 2}, expect_mem{2, 3, 0};
  chain.UpdateMemory(update_mem.begin(), update_mem.end());
  EXPECT_EQ(chain.GetMemory(), expect_mem);

  update_mem = {4, 5}, expect_mem = {5, 4, 2};
  chain.UpdateMemory(update_mem.begin(), update_mem.end());
  EXPECT_EQ(chain.GetMemory(), expect_mem);

  update_mem = {1}, expect_mem = {1, 5, 4};
  chain.UpdateMemory(update_mem.begin(), update_mem.end());
  EXPECT_EQ(chain.GetMemory(), expect_mem);

  update_mem = {0}, expect_mem = {0, 1, 5};
  chain.UpdateMemory(update_mem[0]);
  EXPECT_EQ(chain.GetMemory(), expect_mem);
}


TEST(RemberChainTest, GetMemoryAfterFeedSequenceAndUpdateMemory) {
  std::vector<int> seq{0, 2, 1, 4, 2, 4, 3, 2, 2, 3, 0};
  RemberChain<int> chain(2, RANDOM_STATE);
  std::deque update_mem{0, 1, 2}, expect_mem{2, 1, 0};
  chain.UpdateMemory(update_mem.begin(), update_mem.end());
  EXPECT_EQ(chain.GetMemory(), expect_mem);

  chain.FeedSequenceImpl(seq.begin(), seq.end(), true);
  expect_mem = {0, 3, 2};
  EXPECT_EQ(chain.GetMemory(), expect_mem);

  update_mem = {1, 2}, expect_mem = {2, 1, 0};
  chain.UpdateMemory(update_mem.begin(), update_mem.end());
  EXPECT_EQ(chain.GetMemory(), expect_mem);

  chain.FeedSequenceImpl(seq.begin() + 2, seq.begin() + 4, true);
  expect_mem = {4, 1, 2};
  EXPECT_EQ(chain.GetMemory(), expect_mem);
}


TEST(RemberChainTest, Predict2010WithMemory1) {
  std::vector<int> seq{2, 0, 1, 0, 2, 0, 1, 0, 2, 0};
  RemberChain<int> chain(1, RANDOM_STATE);
  chain.FeedSequenceImpl(seq.begin(), seq.end());
  std::deque<int> expect_mem{0, 2};
  EXPECT_EQ(chain.GetMemory(), expect_mem);

  int count_1 = 0, count_2 = 0;
  for (int i = 0; i < 100; ++i) {
    int pred = chain.PredictState();
    ASSERT_TRUE(pred == 1 or pred == 2);
    if (pred == 1) {
      count_1 += 1;
    } else {
      count_2 += 1;
    }
  }
  EXPECT_GT(count_1, count_2);
  EXPECT_GT(count_2, 0);
}

TEST(RemberChainTest, PredictHillsWithMemory1) {
  std::vector<int> seq{0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0};
  RemberChain<int> chain(1, RANDOM_STATE);
  chain.FeedSequenceImpl(seq.begin(), seq.end(), true);
  EXPECT_EQ(chain.GetMemory(), (std::deque{0, 1}));

  int count_1 = 0, count_3 = 0;
  for (int i = 0; i < 100; ++i) {
    int pred = chain.PredictState();
    ASSERT_TRUE(pred == 1 or pred == 3);
    switch (pred) {
      case 1:
        count_1 += 1;
        break;
      case 3:
        count_3 += 1;
    }
  }
  EXPECT_GT(count_1, count_3);
  EXPECT_GT(count_1 - count_3, 20);
  EXPECT_GT(count_3, 0);
}

TEST(RemberChainTest, PredictHillsWithMemory2) {
  std::vector<int> seq{0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0};
  RemberChain<int> chain(2, RANDOM_STATE);
  chain.FeedSequenceImpl(seq.begin(), seq.end(), true);
  EXPECT_EQ(chain.GetMemory(), (std::deque{0, 1, 2}));

  int count_1 = 0, count_3 = 0;
  for (int i = 0; i < 100; ++i) {
    int pred = chain.PredictState();
    ASSERT_PRED1([](int pred) { return pred == 1 or pred == 3; }, pred);
    if (pred == 1) {
      count_1 += 1;
    } else if (pred == 3) {
      count_3 += 1;
    }
  }
  EXPECT_GT(count_1, count_3);
  EXPECT_GT(count_1 - count_3, 20);
  EXPECT_GT(count_3, 0);
}

TEST(RemberChainTest, PredictHillsWithMemory3) {
  std::vector<int> seq{0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0};
  RemberChain<int> chain(3, RANDOM_STATE);
  chain.FeedSequenceImpl(seq.begin(), seq.end(), true);
  EXPECT_EQ(chain.GetMemory(), (std::deque{0, 1, 2, 3}));

  int count_1 = 0, count_3 = 0;
  for (int i = 0; i < 100; ++i) {
    int pred = chain.PredictState();
    ASSERT_PRED1([](int pred) { return pred == 1 or pred == 3; }, pred);
    if (pred == 1) {
      count_1 += 1;
    } else if (pred == 3) {
      count_3 += 1;
    }
  }
  EXPECT_GT(count_1, count_3);
  EXPECT_GT(count_1 - count_3, 20);
  EXPECT_GT(count_3, 0);
}

TEST(RemberChainTest, PredictRhytmWithMemory1) {
  std::vector<int> seq{3, 3, 3, 4, 5, 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 3,
                       3, 4, 5, 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 3, 3};
  RemberChain<int> chain(1, RANDOM_STATE);
  chain.FeedSequenceImpl(seq.begin(), seq.end(), true);
  EXPECT_EQ(chain.GetMemory(), (std::deque{3, 3}));

  int count[6] = {0};
  for (int i = 0; i < 100; ++i) {
    int pred = chain.PredictState();
    std::set<int> possible_val{1, 2, 3, 4, 5};
    ASSERT_PRED1([&](int pred) { return possible_val.contains(pred); }, pred);
    count[pred]++;
  }
  
  ASSERT_GT(count[3], count[4]);
  
  ASSERT_GT(count[4], count[1]);
  ASSERT_GT(count[4], count[2]);
  ASSERT_GT(count[4], count[5]);
  
  ASSERT_GT(count[1], 0);
  ASSERT_GT(count[2], 0);
  ASSERT_GT(count[5], 0);
}

TEST(RemberChainTest, PredictRhytmWithMemory2) {
  std::vector<int> seq{3, 3, 3, 4, 5, 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 3,
                       3, 4, 5, 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 3, 3};
  RemberChain<int> chain(2, RANDOM_STATE);
  chain.FeedSequenceImpl(seq.begin(), seq.end(), true);
  EXPECT_EQ(chain.GetMemory(), (std::deque{3, 3, 3}));

  std::vector<int> count(7, 0);
  for (int i = 0; i < 100; ++i) {
    int pred = chain.PredictState();
    std::set<int> possible_val{0, 1, 2, 3, 4, 5, 6};
    ASSERT_PRED1([&](int pred) { return possible_val.contains(pred); }, pred);
    count[pred]++;
  }
  
  ASSERT_GT(count[3], count[4]);
  
  ASSERT_GT(count[4], count[5]);
  
  ASSERT_GT(count[5], count[0]);
  ASSERT_GT(count[5], count[1]);
  ASSERT_GT(count[5], count[2]);
  ASSERT_GT(count[5], count[6]);
  
  ASSERT_GT(count[0], 0);
  ASSERT_GT(count[1], 0);
  ASSERT_GT(count[2], 0);
  ASSERT_GT(count[6], 0);
}

TEST(RemberChainTest, PredictRhytmWithMemory3) {
  std::vector<int> seq{3, 3, 3, 4, 5, 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 3,
                       3, 4, 5, 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 3, 3};
  RemberChain<int> chain(3, RANDOM_STATE);
  chain.FeedSequenceImpl(seq.begin(), seq.end(), true);
  EXPECT_EQ(chain.GetMemory(), (std::deque{3, 3, 3, 2}));

  std::vector<int> count(7, 0);
  for (int i = 0; i < 100; ++i) {
    int pred = chain.PredictState();
    std::set<int> possible_val{0, 1, 2, 3, 4, 5, 6};
    ASSERT_PRED1([&](int pred) { return possible_val.contains(pred); }, pred);
    count[pred]++;
  }
  
  ASSERT_GT(count[3], count[4]);
  
  ASSERT_GT(count[4], count[5]);
  
  ASSERT_GT(count[5], count[0]);
  ASSERT_GT(count[5], count[1]);
  ASSERT_GT(count[5], count[2]);
  ASSERT_GT(count[5], count[6]);
  
  ASSERT_GT(count[0], 0);
  ASSERT_GT(count[1], 0);
  ASSERT_GT(count[2], 0);
  ASSERT_GT(count[6], 0);
}
