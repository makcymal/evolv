#pragma once

#include <chrono>
#include <cstdint>
#include <deque>
#include <random>
#include <unordered_map>

#include "base_chain.h"
#include "fenwick_tree.h"


namespace evolv::internal {

/*!
  \brief The implementor of BaseChain, that forgets all previous states

  While learning from given sequences, for all seen states it counts
  transitions to each state. That's where inner class TransitCounter comes.
  Predicting next state is based on current state and counted transitions.
  The more transitions from state A to state B -> the more probability
  that standing in state A the chain will predict state B.
*/
template <class CodeT>
  requires std::integral<CodeT>
class ForgorChain : public BaseChain<CodeT> {
  using typename BaseChain<CodeT>::FenwickCounter;
  using BaseChain<CodeT>::memory_size_;
  using BaseChain<CodeT>::memory_;
  using BaseChain<CodeT>::rng_;

 public:
  explicit ForgorChain(int random_state) : BaseChain<CodeT>(1, random_state) {
  }

  //! Learn from sequence and move to last state in sequence if needed or if
  //! there is no memory
  void FeedSequence(EncodingIter<CodeT> it, EncodingIter<CodeT> end,
                    bool update_memory = false) {
    if (it == end) {
      return;
    }

    CodeT state = *it;
    ++it;
    for (; it != end; ++it) {
      transitions_.Get(state).Add(*it, 1);
      state = *it;
    }
    if (update_memory || memory_.empty()) {
      UpdateMemory(state);
    }
  }

  //! Predict the subsequent state based on current state and possibly memory,
  //! move to predicted state if needed
  CodeT PredictState(bool update_memory = false) {
    assert(!memory_.empty() && "Call FeedSequence at least once");
    int64_t x = rng_() % transitions_.Get(memory_[0]).TotalTransitions();
    CodeT next_state = transitions_.Get(memory_[0]).UpperBound(x);
    if (update_memory) {
      UpdateMemory(next_state);
    }
    return next_state;
  }

 private:
  class TransitCounters {
   public:
    FenwickCounter &Get(CodeT from) {
      return counters_[from];
    }

   private:
    std::unordered_map<CodeT, FenwickCounter> counters_;
  };
  // For all states count transitions to each state
  TransitCounters transitions_;
};

}  // namespace evolv::internal
