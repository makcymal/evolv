#pragma once

#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "gtest/gtest.h"
#include "instantiate.h"


using namespace evolv::internal;


class EncodingIterTest : public testing::Test {
 protected:
  EncodingIterTest()
      : words{"It's", "a", "pain", "to", "debug", "a", "test"},
        coder(std::make_shared<StateCoder<std::string, int>>()),
        it(words.begin(), coder),
        end(words.end(), coder),
        end2(words.end(), coder) {
  }

  std::vector<std::string> words;
  std::shared_ptr<StateCoder<std::string, int>> coder;
  EncodingIter<int> it, end, end2;
};


TEST_F(EncodingIterTest, EndComparison) {
  EXPECT_NE(it, end);
  EXPECT_EQ(end, end2);
}


TEST_F(EncodingIterTest, RebuiltSequence) {
  std::stringstream rebuilt, target;
  for (int i = 0; it != end; ++it, ++i) {
    rebuilt << coder->Decode(*it) << ' ';
    target << words[i] << ' ';
  }
  EXPECT_EQ(rebuilt.str(), target.str());
}


TEST_F(EncodingIterTest, StateCodes) {
  EXPECT_EQ(*it, 0);
  EXPECT_EQ(*(++it), 1);
  EXPECT_EQ(*(++it), 2);
  EXPECT_EQ(*(++it), 3);
  EXPECT_EQ(*(++it), 4);
  EXPECT_EQ(*(++it), 1);
  EXPECT_EQ(*(++it), 5);
}


TEST_F(EncodingIterTest, RepeatedState) {
  int first_a = *(++it);
  for (int i = 0; i < 4; ++i, ++it) {
  }
  int second_a = *it;
  EXPECT_EQ(first_a, second_a);
}
