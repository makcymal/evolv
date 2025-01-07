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
        coder(std::make_shared<StateCoder<int, short>>()),
        chain() {
  }

  std::vector<int> seq1, seq2;
  std::shared_ptr<StateCoder<int, short>> coder;
  ForgorChain<short> chain;
};

TEST_F(ForgorChainTest, DISABLED_CurrentState) {
  EXPECT_EQ(chain.GetCurrentState(), std::deque<short>{-1});
  chain.SetCurrentState(13);
  EXPECT_EQ(chain.GetCurrentState(), std::deque<short>{13});
  std::vector<int> state{3, 5, 7};
  chain.SetCurrentState(EncodingIter(state.begin(), coder),
                        EncodingIter(state.end(), coder));
  EXPECT_EQ(chain.GetCurrentState(), std::deque<short>{coder->Encode(7)});
  chain.FeedSequence(EncodingIter(seq1.begin(), coder),
                     EncodingIter(seq1.end(), coder), true);
  EXPECT_EQ(chain.GetCurrentState(), std::deque<short>{coder->Encode(1)});
}

TEST_F(ForgorChainTest, DefinitePredictState) {
  chain.FeedSequence(EncodingIter(seq1.begin(), coder),
                     EncodingIter(seq1.end(), coder), true);
  EXPECT_EQ(coder->Decode(chain.PredictState()), 2);

  chain.FeedSequence(EncodingIter(seq1.begin(), coder),
                     EncodingIter(seq1.end(), coder), true);
  EXPECT_EQ(coder->Decode(chain.PredictState()), 2);

  chain.SetCurrentState(coder->Encode(2));
  EXPECT_EQ(coder->Decode(chain.PredictState()), 1);

  chain.FeedSequence(EncodingIter(seq2.begin(), coder),
                     EncodingIter(seq2.end(), coder), true);
  EXPECT_EQ(coder->Decode(chain.GetCurrentState()[0]), 3);
  EXPECT_EQ(coder->Decode(chain.PredictState(true)), 1);
  EXPECT_EQ(coder->Decode(chain.PredictState(true)), 2);
  EXPECT_EQ(coder->Decode(chain.GetCurrentState()[0]), 2);
  
  // int count_1 = 0, count_3 = 0;
  // for (int i = 0; i < 100; ++i) {
  //   int pred = chain.PredictState();
  //   pred = coder->Decode(pred);
  //   EXPECT_TRUE(pred == 1 or pred == 3);
  //   if (pred == 1) {
  //     count_1 += 1;
  //   } else {
  //     count_3 += 1;
  //   }
  // }
  // std::cout << std::endl << count_1 << ' ' << count_3 << std::endl;
  // EXPECT_GT(count_1, count_3);
  // EXPECT_GT(count_3, 0);
}
