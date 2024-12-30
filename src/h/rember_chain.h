#pragma once

#include <cstdint>
#include <deque>
#include <random>
#include <unordered_map>
#include <vector>

#include "base_chain.h"
#include "fenwick_tree.h"


namespace evolv::internal {

template <class StateT>
class RemberChain : public BaseChain<StateT> {
 public:
  RemberChain(int memory = 1);

  void FeedSequence(const std::vector<StateT> &seq);

  StateT PredictState(bool move_to_predicted = false);

 private:
  using TransitCounter = std::vector<FenwickTree<int64_t, StateT>>;

  int memory_;
  std::deque<StateT> last_states_;
  std::unordered_map<StateT, TransitCounter> transit_counters_;
  int64_t total_transitions_;
  StateT max_state;
  std::mt19937_64 rng;

  StateT UpperBound(int64_t x);
};

}  // namespace evolv::internal
