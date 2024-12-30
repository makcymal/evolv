#pragma once

#include <cstdint>
#include <random>
#include <unordered_map>

#include "base_chain.h"
#include "fenwick_tree.h"


namespace evolv::internal {

template <class StateT>
class ForgorChain : public BaseChain<StateT> {
 public:
  class TransitCounter {
   public:
    TransitCounter() = default;

    int64_t TotalTransitions();

    void Add(StateT dest, int64_t count);

    StateT UpperBound(int64_t x);

   private:
    int64_t total_transitions_ = 0;
    FenwickTree<int64_t, StateT> count_;
  };

  ForgorChain();

  void FeedSequence(const std::vector<StateT> &seq);

  StateT PredictState(bool move_to_predicted = false);

 private:
  StateT last_state_;
  std::unordered_map<StateT, TransitCounter> transit_counters_;
  std::mt19937_64 rng;
};

}  // namespace evolv::internal
