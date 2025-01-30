#pragma once

#include <deque>
#include <memory>
#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "instantiate.h"

using namespace evolv;
using namespace std;


TEST(MarkovChainTest, StartSentenceWithoutMemory) {
  vector<string> sentenses{"The", "day",   "follows", "night", ".",
                           "The", "night", "follows", "day",   "."};
  MarkovChain<string> chain(0, RANDOM_STATE);
  chain.FeedSequence(sentenses.begin(), sentenses.end());
  EXPECT_EQ(chain.GetMemory(), deque{string(".")});
  EXPECT_EQ(chain.PredictState(), string("The"));

  chain.UpdateMemory("The");
  EXPECT_EQ(chain.GetMemory(), deque{string("The")});
  map<string, int> count;
  for (int i = 0; i < 100; ++i) {
    string pred = chain.PredictState();
    set<string> possible_val{"day", "night"};
    ASSERT_PRED1([&](const string &p) { return possible_val.contains(p); },
                 pred);
    count[pred]++;
  }
  EXPECT_LT(abs(count["day"] - count["night"]), 10);
}


TEST(MarkovChainTest, StartSentenceWithMemory1) {
  vector<string> sentenses{"The", "day",   "follows", "night", ".",
                           "The", "night", "follows", "day",   "."};
  MarkovChain<string> chain(1, RANDOM_STATE);
  chain.FeedSequence(sentenses.begin(), sentenses.end());
  deque<string> exp_mem{".", "day"};
  EXPECT_EQ(chain.GetMemory(), exp_mem);
  EXPECT_EQ(chain.PredictState(), string("The"));

  chain.UpdateMemory("The");
  exp_mem = {"The", "."};
  EXPECT_EQ(chain.GetMemory(), exp_mem);
  map<string, int> count;
  for (int i = 0; i < 100; ++i) {
    string pred = chain.PredictState();
    set<string> possible_val{"day", "night"};
    ASSERT_PRED1([&](const string &p) { return possible_val.contains(p); },
                 pred);
    count[pred]++;
  }
  EXPECT_GT(count["night"] - count["day"], 30);
}


TEST(MarkovChainTest, StartSentenceWithMemory2) {
  vector<string> sentenses{
      "The",     "day", "follows", "night",   ".",   "The",     "night",
      "follows", "day", ".",       "The",     "day", "follows", "night",
      ".",       "The", "night",   "follows", "day", ".",
  };
  MarkovChain<string> chain(2, RANDOM_STATE);
  chain.FeedSequence(sentenses.begin(), sentenses.end());
  deque<string> exp_mem{".", "day", "follows"};
  EXPECT_EQ(chain.GetMemory(), exp_mem);
  EXPECT_EQ(chain.PredictState(), string("The"));

  chain.UpdateMemory("The");
  exp_mem = {"The", ".", "day"};
  EXPECT_EQ(chain.GetMemory(), exp_mem);
  map<string, int> count;
  for (int i = 0; i < 100; ++i) {
    string pred = chain.PredictState();
    set<string> possible_val{"day", "night", "."};
    ASSERT_PRED1([&](const string &p) { return possible_val.contains(p); },
                 pred);
    count[pred]++;
  }
  EXPECT_GT(count["day"], count["night"]);
  EXPECT_GT(count["night"], count["."]);
}


TEST(MarkovChainTest, FinishSentenceWithoutMemory) {
  vector<string> sentenses{
      "The",     "morning", "follows", "night",   ".",       "The",     "day",
      "follows", "morning", ".",       "The",     "evening", "follows", "day",
      ".",       "The",     "night",   "follows", "evening", ".",

  };
  MarkovChain<string> chain(0, RANDOM_STATE);
  chain.FeedSequence(sentenses.begin(), sentenses.end());
  EXPECT_EQ(chain.GetMemory(), deque{string(".")});
  EXPECT_EQ(chain.PredictState(), string("The"));

  chain.UpdateMemory("The");
  EXPECT_EQ(chain.GetMemory(), deque{string("The")});
  map<string, int> count;
  for (int i = 0; i < 100; ++i) {
    string pred = chain.PredictState();
    set<string> possible_val{"morning", "day", "evening", "night"};
    ASSERT_PRED1([&](const string &p) { return possible_val.contains(p); },
                 pred);
    count[pred]++;
  }
  EXPECT_LT(abs(count["morning"] - 25), 6);
  EXPECT_LT(abs(count["day"] - 25), 6);
  EXPECT_LT(abs(count["evening"] - 25), 6);
  EXPECT_LT(abs(count["night"] - 25), 6);

  chain.UpdateMemory("follows");
  EXPECT_EQ(chain.GetMemory(), deque{string("follows")});
  count.clear();
  for (int i = 0; i < 100; ++i) {
    string pred = chain.PredictState();
    set<string> possible_val{"morning", "day", "evening", "night"};
    ASSERT_PRED1([&](const string &p) { return possible_val.contains(p); },
                 pred);
    count[pred]++;
  }
  EXPECT_LT(abs(count["morning"] - 25), 6);
  EXPECT_LT(abs(count["day"] - 25), 6);
  EXPECT_LT(abs(count["evening"] - 25), 6);
  EXPECT_LT(abs(count["night"] - 25), 6);
}


TEST(MarkovChainTest, FinishSentenceWithMemory2) {
  vector<string> sentenses{
      "The",     "morning", "follows", "night",   ".",       "The",     "day",
      "follows", "morning", ".",       "The",     "evening", "follows", "day",
      ".",       "The",     "night",   "follows", "evening", ".",

  };
  MarkovChain<string> chain(2, RANDOM_STATE + 1);
  chain.FeedSequence(sentenses.begin(), sentenses.end());
  deque<string> exp_mem{".", "evening", "follows"};
  EXPECT_EQ(chain.GetMemory(), exp_mem);
  EXPECT_EQ(chain.PredictState(), string("The"));

  deque<string> new_mem{"The", "evening", "follows"};
  exp_mem = {"follows", "evening", "The"};
  chain.UpdateMemory(new_mem.begin(), new_mem.end());
  EXPECT_EQ(chain.GetMemory(), exp_mem);
  map<string, int> count;
  for (int i = 0; i < 100; ++i) {
    string pred = chain.PredictState();
    set<string> possible_val{"morning", "day", "evening", "night"};
    ASSERT_PRED1([&](const string &p) { return possible_val.contains(p); },
                 pred);
    count[pred]++;
  }
  EXPECT_GT(count["day"], count["morning"]);
  EXPECT_GT(count["day"], count["evening"]);
  EXPECT_GT(count["day"], count["night"]);
}
