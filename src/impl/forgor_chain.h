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
 public:
  //! Set curr_state_ to undefined and initialize rng_
  ForgorChain() {
    curr_state_ = -1;
    rng_ = std::mt19937_64(
        std::chrono::steady_clock::now().time_since_epoch().count());
  }

  //! Learn from the sequence given as pair of iterators,
  //! move to last state in sequence if needed
  void FeedSequence(EncodingIter<CodeT> it, EncodingIter<CodeT> end,
                    bool move_to_last = false) {
    if (it == end) {
      return;
    }

    CodeT state = *it;
    // std::cout << "fed sequence: " << *it << ' ';
    ++it;
    for (; it != end; ++it) {
      // std::cout << *it << ' ';
      transit_counters_[state].Add(*it, 1);
      state = *it;
    }
    // std::cout << std::endl;

    if (move_to_last) {
      curr_state_ = state;
    }
  }

  //! Predict the subsequent state from the current state,
  //! move to predicted state if needed
  CodeT PredictState(bool move_to_predicted = false) {
    assert(curr_state_ != -1 && "No current state available");

    // std::cout << "current code = " << curr_state_ << std::endl;
    // transit_counters_[curr_state_].dbg();
    int64_t x = rng_() % transit_counters_[curr_state_].TotalTransitions();
    // std::cout << "seeking code with prefix sum " << x << std::endl;
    CodeT next_state = transit_counters_[curr_state_].UpperBound(x);

    if (move_to_predicted) {
      curr_state_ = next_state;
    }
    // std::cout << "predicted: " << next_state << ' ';
    return next_state;
  }

  //! Set new state given as single value, forget the current one
  void SetCurrentState(CodeT state) {
    curr_state_ = state;
  }

  //! Set new state given as pair of iterators, forget the actual one
  void SetCurrentState(EncodingIter<CodeT> it, EncodingIter<CodeT> end) {
    for (; it != end; ++it) {
      curr_state_ = *it;
    }
  }

  //! Get deque of current state, where the first is the last seen state.
  std::deque<CodeT> GetCurrentState() const {
    return {curr_state_};
  }

 private:
  /*!
    \brief Counts transitions from one state to all states

    The Fenwick tree is used internally to add new transitions and
    quering upper bound on prefix sums, both in O(logn). The last one
    is used to predict next state according to how many time it occured.
  */
  class TransitCounter {
   public:
    TransitCounter() = default;

    //! Getter for total_transitions_
    int64_t TotalTransitions() {
      return total_transitions_;
    }

    //! Add <cnt> new transitions into state <idx>
    void Add(CodeT idx, int64_t cnt) {
      count_.Add(idx, cnt);
      total_transitions_ += cnt;
    }

    //! Query upper bound on prefix sums of Fenwick tree
    CodeT UpperBound(int64_t x) {
      return count_.UpperBound(x);
    }
    
    void dbg() {
      std::cout << "total transitions is " << total_transitions_ << std::endl;
      count_.dbg();
    }

   private:
    //! Total number of transitions stored in Fenwick tree
    int64_t total_transitions_ = 0;
    //! Fenwick tree counts transitions
    FenwickTree<int64_t, CodeT> count_;
  };

  // Last state where the chain ends
  CodeT curr_state_;
  // For all states count transitions to each state
  std::unordered_map<CodeT, TransitCounter> transit_counters_;
  // Random number generator, used in predicting next state
  std::mt19937_64 rng_;
};

}  // namespace evolv::internal
