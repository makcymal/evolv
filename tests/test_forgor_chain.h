#pragma once

#include <deque>
#include <memory>
#include <vector>

#include "gtest/gtest.h"
#include "instantiate.h"


using namespace evolv::internal;



class ForgorChainTest : public testing::Test {
 protected:
  ForgorChainTest()
      : seq1{0, 1, 0, 1, 0, 1, 0},
        seq2{0, 1, 2, 0, 1, 2, 0, 1, 2},
        coder(std::make_shared<StateCoder<int, int>>()),
        chain(RANDOM_STATE) {
  }

  std::vector<int> seq1, seq2;
  std::shared_ptr<StateCoder<int, int>> coder;
  ForgorChain<int> chain;
};


TEST_F(ForgorChainTest, CurrentState) {
  EXPECT_EQ(chain.GetMemory(), std::deque<int>{});
  chain.UpdateMemory(13);
  EXPECT_EQ(chain.GetMemory(), std::deque<int>{13});

  std::deque<int> state{3, 5, 7};
  chain.UpdateMemory(state.begin(), state.end());
  EXPECT_EQ(chain.GetMemory(), std::deque<int>{7});

  chain.FeedSequenceImpl(seq1.begin(), seq1.end(), true);
  EXPECT_EQ(chain.GetMemory(), std::deque<int>{0});
}


TEST_F(ForgorChainTest, Seq01PredictState) {
  chain.FeedSequenceImpl(seq1.begin(), seq1.end(), true);
  EXPECT_EQ(chain.PredictState(), 1);

  chain.FeedSequenceImpl(seq1.begin(), seq1.end(), true);
  EXPECT_EQ(chain.PredictState(), 1);

  chain.UpdateMemory(1);
  EXPECT_EQ(chain.PredictState(), 0);
}


TEST_F(ForgorChainTest, Seq012PredictState) {
  chain.FeedSequenceImpl(seq2.begin(), seq2.end(), true);
  EXPECT_EQ(chain.PredictState(), 0);
  EXPECT_EQ(chain.PredictState(true), 0);
  EXPECT_EQ(chain.PredictState(true), 1);
  EXPECT_EQ(chain.PredictState(true), 2);
  EXPECT_EQ(chain.PredictState(true), 0);
  EXPECT_EQ(chain.GetMemory()[0], 0);
}


TEST_F(ForgorChainTest, Seq012StateCoding) {
  chain.FeedSequenceImpl(EncodingIter(seq2.begin(), coder),
                     EncodingIter(seq2.end(), coder), true);
  EXPECT_EQ(coder->Encode(0), 0);
  EXPECT_EQ(coder->Encode(1), 1);
  EXPECT_EQ(coder->Encode(2), 2);
  EXPECT_EQ(coder->Decode(0), 0);
  EXPECT_EQ(coder->Decode(1), 1);
  EXPECT_EQ(coder->Decode(2), 2);
}


TEST_F(ForgorChainTest, Seq01And012StateCoding) {
  chain.FeedSequenceImpl(EncodingIter(seq1.begin(), coder),
                     EncodingIter(seq1.end(), coder), true);
  EXPECT_EQ(coder->Encode(0), 0);
  EXPECT_EQ(coder->Encode(1), 1);
  EXPECT_EQ(coder->Decode(0), 0);
  EXPECT_EQ(coder->Decode(1), 1);

  chain.FeedSequenceImpl(EncodingIter(seq2.begin(), coder),
                     EncodingIter(seq2.end(), coder), true);
  EXPECT_EQ(coder->Encode(0), 0);
  EXPECT_EQ(coder->Encode(1), 1);
  EXPECT_EQ(coder->Decode(0), 0);
  EXPECT_EQ(coder->Decode(1), 1);
  EXPECT_EQ(coder->Encode(2), 2);
  EXPECT_EQ(coder->Decode(2), 2);
}


TEST_F(ForgorChainTest, Seq01PredictAnd012StateCoding) {
  chain.FeedSequenceImpl(EncodingIter(seq1.begin(), coder),
                     EncodingIter(seq1.end(), coder), true);
  EXPECT_EQ(coder->Decode(chain.PredictState()), 1);
  EXPECT_EQ(coder->Decode(chain.PredictState(true)), 1);
  EXPECT_EQ(coder->Decode(chain.PredictState(true)), 0);

  chain.FeedSequenceImpl(EncodingIter(seq2.begin(), coder),
                     EncodingIter(seq2.end(), coder), true);
  EXPECT_EQ(coder->Encode(0), 0);
  EXPECT_EQ(coder->Encode(1), 1);
  EXPECT_EQ(coder->Encode(2), 2);
  EXPECT_EQ(coder->Decode(0), 0);
  EXPECT_EQ(coder->Decode(1), 1);
  EXPECT_EQ(coder->Decode(2), 2);
}


TEST_F(ForgorChainTest, Seq01And012Predict) {
  chain.FeedSequenceImpl(EncodingIter(seq1.begin(), coder),
                     EncodingIter(seq1.end(), coder), true);
  EXPECT_EQ(coder->Decode(chain.PredictState()), 1);
  EXPECT_EQ(coder->Decode(chain.PredictState(true)), 1);
  EXPECT_EQ(coder->Decode(chain.PredictState(true)), 0);

  chain.FeedSequenceImpl(EncodingIter(seq2.begin(), coder),
                     EncodingIter(seq2.end(), coder), true);
  EXPECT_EQ(coder->Decode(chain.PredictState()), 0);
  EXPECT_EQ(coder->Decode(chain.PredictState(true)), 0);
  EXPECT_EQ(coder->Decode(chain.PredictState(true)), 1);
}


TEST_F(ForgorChainTest, IndefiniteForgorPredict) {
  chain.FeedSequenceImpl(seq1.begin(), seq1.end(), true);
  chain.FeedSequenceImpl(seq2.begin(), seq2.end(), true);
  chain.UpdateMemory(1);

  int count_0 = 0, count_2 = 0;
  for (int i = 0; i < 100; ++i) {
    int next_state = chain.PredictState();
    ASSERT_TRUE(next_state == 0 or next_state == 2);
    if (next_state == 0) {
      count_0 += 1;
    } else {
      count_2 += 1;
    }
  }
  EXPECT_LE(abs(count_0 - count_2), 20);

  chain.FeedSequenceImpl(seq1.begin(), seq1.end());
  count_0 = 0, count_2 = 0;
  for (int i = 0; i < 100; ++i) {
    int next_state = chain.PredictState();
    EXPECT_TRUE(next_state == 0 or next_state == 2);
    if (next_state == 0) {
      count_0 += 1;
    } else {
      count_2 += 1;
    }
  }
  EXPECT_GT(count_0, count_2);
  EXPECT_GT(count_2, 0);
}
