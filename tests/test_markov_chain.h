#pragma once

#include <deque>
#include <memory>
#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "instantiate.h"

// todo: remove
#include "lib/dbg/dbg.h"


using namespace evolv;
using namespace std;


TEST(MarkovChainTest, StartSentenceWithoutMemory) {
  vector<string> sentenses{"The", "day",   "follows", "night", ".",
                           "The", "night", "follows", "day",   "."};
  MarkovChain<string> chain(0, RANDOM_STATE);
  chain.FeedSequence(sentenses.begin(), sentenses.end());
  dbg(chain);
  EXPECT_EQ(chain.GetMemory(), deque{string(".")});
  // EXPECT_EQ(chain.PredictState(), string("The"));

  // chain.UpdateMemory("The");
  // EXPECT_EQ(chain.GetMemory(), deque{string("The")});
  // map<string, int> count;
  // for (int i = 0; i < 100; ++i) {
  //   string pred = chain.PredictState();
  //   set<string> possible_val{"day", "night"};
  //   ASSERT_PRED1([&](string pred) { return possible_val.contains(pred); },
  //                pred);
  //   count[pred]++;
  // }
  // EXPECT_LT(abs(count["day"] - count["night"]), 10);
}
