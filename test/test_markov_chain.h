#include <deque>
#include <memory>
#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "src/evolv.h"
#include "src/impl/encoding_iter.h"
#include "src/impl/forgor_chain.h"
#include "src/impl/rember_chain.h"
#include "src/impl/state_coder.h"

using namespace evolv::internal;


class ForgorChainTest : public testing::Test {
 protected:
  ForgorChainTest()
      : seq1{1, 2, 1, 2, 1, 2, 1},
        seq2{1, 2, 3, 1, 2, 3, 1, 2, 3},
        coder(std::make_shared<StateCoder<int, int>>()),
        chain() {
  }

  std::vector<int> seq1, seq2;
  std::shared_ptr<StateCoder<int, int>> coder;
  ForgorChain<int> chain;
};

TEST_F(ForgorChainTest, CurrentState) {
  EXPECT_EQ(chain.GetCurrentState(), std::deque<int>{-1});
  chain.SetCurrentState(13);
  EXPECT_EQ(chain.GetCurrentState(), std::deque<int>{13});
  std::vector<int> state{3, 5, 7};
  chain.SetCurrentState(EncodingIter(state.begin(), coder),
                        EncodingIter(state.end(), coder));
  EXPECT_EQ(chain.GetCurrentState(), std::deque<int>{coder->Encode(7)});
  chain.FeedSequence(EncodingIter(seq1.begin(), coder),
                     EncodingIter(seq1.end(), coder), true);
  EXPECT_EQ(chain.GetCurrentState(), std::deque<int>{coder->Encode(1)});
}

TEST_F(ForgorChainTest, Seq12PredictState) {
  chain.FeedSequence(EncodingIter(seq1.begin(), coder),
                     EncodingIter(seq1.end(), coder), true);
  EXPECT_EQ(coder->Decode(chain.PredictState()), 2);

  chain.FeedSequence(EncodingIter(seq1.begin(), coder),
                     EncodingIter(seq1.end(), coder), true);
  EXPECT_EQ(coder->Decode(chain.PredictState()), 2);

  chain.SetCurrentState(coder->Encode(2));
  EXPECT_EQ(coder->Decode(chain.PredictState()), 1);
}

TEST_F(ForgorChainTest, Seq123PredictState) {
  chain.FeedSequence(EncodingIter(seq2.begin(), coder),
                     EncodingIter(seq2.end(), coder), true);
  EXPECT_EQ(coder->Decode(chain.PredictState()), 1);
  EXPECT_EQ(coder->Decode(chain.PredictState(true)), 1);
  EXPECT_EQ(coder->Decode(chain.PredictState(true)), 2);
  EXPECT_EQ(coder->Decode(chain.PredictState(true)), 3);
  EXPECT_EQ(coder->Decode(chain.PredictState(true)), 1);
  EXPECT_EQ(coder->Decode(chain.GetCurrentState()[0]), 1);
}

TEST_F(ForgorChainTest, Seq123StateCoding) {
  chain.FeedSequence(EncodingIter(seq2.begin(), coder),
                     EncodingIter(seq2.end(), coder), true);
  EXPECT_EQ(coder->Encode(1), 0);
  EXPECT_EQ(coder->Encode(2), 1);
  EXPECT_EQ(coder->Encode(3), 2);
  EXPECT_EQ(coder->Decode(0), 1);
  EXPECT_EQ(coder->Decode(1), 2);
  EXPECT_EQ(coder->Decode(2), 3);
}

TEST_F(ForgorChainTest, Seq12And123StateCoding) {
  chain.FeedSequence(EncodingIter(seq1.begin(), coder),
                     EncodingIter(seq1.end(), coder), true);
  EXPECT_EQ(coder->Encode(1), 0);
  EXPECT_EQ(coder->Encode(2), 1);
  EXPECT_EQ(coder->Decode(0), 1);
  EXPECT_EQ(coder->Decode(1), 2);
  chain.FeedSequence(EncodingIter(seq2.begin(), coder),
                     EncodingIter(seq2.end(), coder), true);
  EXPECT_EQ(coder->Encode(1), 0);
  EXPECT_EQ(coder->Encode(2), 1);
  EXPECT_EQ(coder->Decode(0), 1);
  EXPECT_EQ(coder->Decode(1), 2);
  EXPECT_EQ(coder->Encode(3), 2);
  EXPECT_EQ(coder->Decode(2), 3);
}

TEST_F(ForgorChainTest, Seq12PredictAnd123StateCoding) {
  chain.FeedSequence(EncodingIter(seq1.begin(), coder),
                     EncodingIter(seq1.end(), coder), true);
  EXPECT_EQ(coder->Decode(chain.PredictState()), 2);
  EXPECT_EQ(coder->Decode(chain.PredictState(true)), 2);
  EXPECT_EQ(coder->Decode(chain.PredictState(true)), 1);
  chain.FeedSequence(EncodingIter(seq2.begin(), coder),
                     EncodingIter(seq2.end(), coder), true);
  EXPECT_EQ(coder->Encode(1), 0);
  EXPECT_EQ(coder->Encode(2), 1);
  EXPECT_EQ(coder->Encode(3), 2);
  EXPECT_EQ(coder->Decode(0), 1);
  EXPECT_EQ(coder->Decode(1), 2);
  EXPECT_EQ(coder->Decode(2), 3);
}

TEST_F(ForgorChainTest, Seq12And123Predict) {
  chain.FeedSequence(EncodingIter(seq1.begin(), coder),
                     EncodingIter(seq1.end(), coder), true);
  EXPECT_EQ(coder->Decode(chain.PredictState()), 2);
  EXPECT_EQ(coder->Decode(chain.PredictState(true)), 2);
  EXPECT_EQ(coder->Decode(chain.PredictState(true)), 1);
  chain.FeedSequence(EncodingIter(seq2.begin(), coder),
                     EncodingIter(seq2.end(), coder), true);
  EXPECT_EQ(coder->Decode(chain.PredictState()), 1);
  EXPECT_EQ(coder->Decode(chain.PredictState(true)), 1);
  EXPECT_EQ(coder->Decode(chain.PredictState(true)), 2);
}

TEST_F(ForgorChainTest, IndefiniteForgorPredict) {
  chain.FeedSequence(EncodingIter(seq1.begin(), coder),
                     EncodingIter(seq1.end(), coder), true);
  chain.FeedSequence(EncodingIter(seq2.begin(), coder),
                     EncodingIter(seq2.end(), coder), true);

  chain.SetCurrentState(coder->Encode(2));
  int count_1 = 0, count_3 = 0;
  for (int i = 0; i < 100; ++i) {
    int next_state = coder->Decode(chain.PredictState());
    EXPECT_TRUE(next_state == 1 or next_state == 3);
    if (next_state == 1) {
      count_1 += 1;
    } else {
      count_3 += 1;
    }
  }
  EXPECT_LT(abs(count_1 - count_3), 20);

  chain.FeedSequence(EncodingIter(seq1.begin(), coder),
                     EncodingIter(seq1.end(), coder));
  count_1 = 0, count_3 = 0;
  for (int i = 0; i < 100; ++i) {
    int next_state = coder->Decode(chain.PredictState());
    EXPECT_TRUE(next_state == 1 or next_state == 3);
    if (next_state == 1) {
      count_1 += 1;
    } else {
      count_3 += 1;
    }
  }
  EXPECT_GT(count_1, count_3);
  EXPECT_GT(count_3, 0);
}
