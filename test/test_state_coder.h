#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "src/impl/state_coder.h"

using namespace evolv::internal;


// StateCoderTest is the suite to test StateCoder

TEST(StateCoderTest, StringToInt) {
  StateCoder<std::string, int> coder;
  std::vector<std::string> words{"Tests",       "should", "be",
                                 "independent", "and",    "repeatable"};
  EXPECT_EQ(coder.Encode(words[0]), 0);
  EXPECT_EQ(coder.Encode(words[1]), 1);
  EXPECT_EQ(coder.Decode(1), words[1]);
  EXPECT_EQ(coder.Encode(words[2]), 2);
  EXPECT_EQ(coder.Encode(words[1]), 1);
  EXPECT_EQ(coder.Decode(0), "Tests");
  EXPECT_EQ(coder.Encode(words[5]), 3);
}
