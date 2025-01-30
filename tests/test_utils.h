#pragma once

#include <map>
#include <vector>

#include "gtest/gtest.h"
#include "src/impl/utils.h"


using namespace evolv::utils;


// IsIteratorTest is the suite for testing is_iterator concept

TEST(IsIteratorTest, TrulyIterators) {
  EXPECT_TRUE((is_iterator<std::vector<int>::iterator, int>));
  EXPECT_TRUE((is_iterator<std::map<std::string, int>::iterator,
                           std::pair<const std::string, int>>));
}


TEST(IsIteratorTest, FalseIterators) {
  EXPECT_FALSE((is_iterator<std::vector<int>::iterator, std::string>));
  EXPECT_FALSE((is_iterator<std::map<std::string, int>::iterator,
                            std::pair<const std::string &, int>>));
}
