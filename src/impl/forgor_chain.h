#pragma once

#include <chrono>
#include <cstdint>
#include <deque>
#include <random>
#include <unordered_map>

#include "base_chain.h"
#include "fenwick_tree.h"

// todo: remove
#include "lib/dbg/dbg.h"


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
  using BaseChain<CodeT>::UpdateMemory;
  using BaseChain<CodeT>::GetMemory;

  explicit ForgorChain(int random_state) : BaseChain<CodeT>(1, random_state) {
  }

  //! Learn from sequence and move to last state in sequence if needed or if
  //! there is no memory. This is the implementation of virtual FeedSequence 
  //! in BaseChain
  void FeedSequence(EncodingIter<CodeT> it, EncodingIter<CodeT> end,
                    bool update_memory) {
    FeedSequenceImpl(std::move(it), std::move(end), update_memory);
  }

  //! Learn from sequence and move to last state in sequence if needed or if
  //! there is no memory. This is the implementation called either from
  //! virtual FeedSequence or directly (in tests, for example)
  template <class IterT>
  void FeedSequenceImpl(IterT it, IterT end, bool update_memory = false) {
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
    int64_t x = rng_() % transitions_.Get(memory_[0]).TotalSum();
    CodeT next_state = transitions_.Get(memory_[0]).UpperBound(x);
    if (update_memory) {
      UpdateMemory(next_state);
    }
    return next_state;
  }

  // todo: remove
  DERIVE_DEBUG(memory_size_, memory_, transitions_)

 private:
  class TransitCounters {
   public:
    FenwickCounter &Get(CodeT from) {
      return counters_[from];
    }

    // todo: remove
    DERIVE_DEBUG(counters_)

   private:
    std::unordered_map<CodeT, FenwickCounter> counters_;
  };
  // For all states count transitions to each state
  TransitCounters transitions_;
};

}  // namespace evolv::internal
