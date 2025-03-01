#include <gtest/gtest.h>

#include "test_encoding_iter.h"
#include "test_fenwick_tree.h"
#include "test_forgor_chain.h"
#include "test_markov_chain.h"
#include "test_rember_chain.h"
#include "test_state_coder.h"
#include "test_utils.h"


int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
