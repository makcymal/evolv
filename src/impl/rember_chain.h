#pragma once

#include <cassert>
#include <chrono>
#include <cstdint>
#include <deque>
#include <random>
#include <unordered_map>
#include <vector>

#include "base_chain.h"
#include "fenwick_tree.h"


namespace evolv::internal {

/*!
  \brief The implementor of BaseChain, that remembers N previous states

  While learning from given sequences, for all states it counts transitions
  to each subsequent state that comes in less that N + 1 steps.
  That's where inner class TransitCounter comes.
  Predicting next state is based on current state and counted transitions.
*/
template <class CodeT>
  requires std::integral<CodeT>
class RemberChain : public BaseChain<CodeT> {
 public:
  //! Set curr_state_ and max_state_ to undefined, initialize memory_ and rng_
  explicit RemberChain(int memory = 1) {
    assert(memory >= 1 && "Constructing RemberChain with memory < 1");
    memory_ = memory;
    max_state_ = -1;
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

    std::deque<CodeT> state{*it};
    max_state_ = std::max(max_state_, *it);
    ++it;

    // iterate over sequence and add new transitions
    // given the sequence s[0]..s[i]s[i+1]..s[i+N]..
    // for each d (depth) = 0..N add new transition from s[i] to s[i+d+1]
    for (; it != end; ++it) {
      for (int dep = 0; dep <= memory_ && dep < static_cast<int>(state.size());
           ++dep) {
        GetTransitCounter(state[dep], dep).Add(*it, 1);
      }
      if (static_cast<int>(state.size()) >= memory_ + 1) {
        state.pop_back();
      }
      state.push_front(*it);
      max_state_ = std::max(max_state_, *it);
    }

    if (move_to_last) {
      curr_state_ = state;
    }
  }

  //! Predict the subsequent state from the current state,
  //! move to predicted state if needed
  CodeT PredictState(bool move_to_predicted = false) {
    assert(curr_state_[0] != -1 && "No current state available");

    int64_t x = rng_() % TotalTransitions();
    // std::cout << "seeking for upper bound on answer space with x = " << x
    // << std::endl;
    CodeT next_state = UpperBound(x);

    if (move_to_predicted) {
      curr_state_.pop_back();
      curr_state_.push_front(next_state);
    }

    return next_state;
  }

  //! Set new state given as single value, forget the current one
  void SetCurrentState(CodeT state) {
    if (static_cast<int>(curr_state_.size()) >= memory_ + 1) {
      curr_state_.pop_back();
    }
    curr_state_.push_back(state);
  }

  //! Set new state given as pair of iterators, forget the actual one
  void SetCurrentState(EncodingIter<CodeT> it, EncodingIter<CodeT> end) {
    for (; it != end; ++it) {
      if (static_cast<int>(curr_state_.size()) == memory_ + 1) {
        curr_state_.pop_back();
      }
      curr_state_.push_front(*it);
    }
  }

  //! Get deque of current state, where the first is the last seen state.
  std::deque<CodeT> GetCurrentState() const {
    return curr_state_;
  }

 private:
  //! For i = 0..N count transitions to subsequent states come in i steps
  using TransitCounter = std::vector<FenwickTree<int64_t, CodeT>>;

  //! Fixed number of previous states to remember
  int memory_;
  //! memory_ + 1 last states where the chain ends
  std::deque<CodeT> curr_state_;
  //! For all seen states count transitions into subsequent states come
  //! in less than memory_ + 1 steps
  std::unordered_map<CodeT, TransitCounter> transit_counters_;
  //! State with maximum number ever seen
  CodeT max_state_;
  // Random number generator, used in predicting next state
  std::mt19937_64 rng_;

  FenwickTree<int64_t, CodeT> &GetTransitCounter(CodeT from, int depth) {
    if (static_cast<int>(transit_counters_[from].size()) <= depth) {
      transit_counters_[from].resize(depth + 1);
    }
    return transit_counters_[from][depth];
  }

  int64_t TotalTransitions() {
    int total_transitions = 0;
    for (int dep = 0;
         dep <= memory_ && dep < static_cast<int>(curr_state_.size()); ++dep) {
      total_transitions += GetTransitCounter(curr_state_[dep], dep).Total();
    }
    return total_transitions;
  }

  //! Upper bound for the next state from the current one
  CodeT UpperBound(int64_t x) {
    // perform binary seach on answer space
    // from lowest to highest states the sum of transitions over depth
    int lb = 0, rb = max_state_;
    while (lb < rb) {
      int md = lb + (rb - lb) / 2;

      // count the sum of transitions over depth
      int sum = 0;
      for (int dep = 0; dep <= memory_; ++dep) {
        sum += transit_counters_[curr_state_[dep]][dep].Sum(md);
      }

      if (sum <= x) {
        lb = md + 1;
      } else {
        rb = md;
      }
    }

    return lb;
  }
};

}  // namespace evolv::internal
