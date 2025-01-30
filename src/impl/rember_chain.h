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
  using typename BaseChain<CodeT>::CountT;
  using typename BaseChain<CodeT>::FenwickCounter;
  using BaseChain<CodeT>::memory_size_;
  using BaseChain<CodeT>::memory_;
  using BaseChain<CodeT>::rng_;

 public:
  using BaseChain<CodeT>::UpdateMemory;
  using BaseChain<CodeT>::GetMemory;

  //! Set curr_state_ and max_state_ to undefined, initialize memory_ and rng_
  RemberChain(int memorize_previous, int random_state)
      : BaseChain<CodeT>(1 + memorize_previous, random_state), max_state_(0) {
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

    std::deque<CodeT> last_states{*it};
    max_state_ = std::max(max_state_, *it);
    ++it;

    // iterate over sequence and add new transitions
    // given the sequence s[0]..s[i]s[i+1]..s[i+N]..
    // for each d (depth) = 0..N add new transition from s[i] to s[i+d+1]
    for (; it != end; ++it) {
      for (int depth = 0; depth < static_cast<int>(last_states.size());
           ++depth) {
        transitions_.Get(last_states[depth], depth).Add(*it, 1);
      }
      if (static_cast<int>(last_states.size()) >= memory_size_) {
        last_states.pop_back();
      }
      last_states.push_front(*it);
      max_state_ = std::max(max_state_, *it);
    }
    if (update_memory || memory_.empty()) {
      UpdateMemory(last_states.rbegin(), last_states.rend());
    }
  }

  //! Predict the subsequent state from the current state,
  //! move to predicted state if needed
  CodeT PredictState(bool update_memory = false) {
    assert(!memory_.empty() && "Call FeedSequence at least once");
    int64_t x = rng_() % transitions_.Sum(memory_.begin(), memory_.end());
    CodeT next_state = UpperBound(x);
    if (update_memory) {
      UpdateMemory(next_state);
    }
    return next_state;
  }

 private:
  class TransitCounters {
   public:
    FenwickCounter &Get(CodeT from, int depth) {
      if (static_cast<int>(counters_[from].size()) <= depth) {
        counters_[from].resize(depth + 1);
      }
      return counters_[from][depth];
    }

    template <class IterT>
    CountT Sum(IterT it, IterT end) {
      int depth = 0, sum = 0;
      for (; it != end; ++it, ++depth) {
        sum += Get(*it, depth).TotalSum();
      }
      return sum;
    }

   private:
    std::unordered_map<CodeT, std::vector<FenwickCounter>> counters_;
  };

  //! For all seen states count transitions into subsequent states come
  //! in less than memory_size_ steps
  TransitCounters transitions_;
  //! State with maximum number ever seen
  CodeT max_state_;

  //! Upper bound for the next state from the current one
  CodeT UpperBound(int64_t x) {
    // perform binary seach on answer space
    // from lowest to highest states the sum of transitions over depth
    int lb = 0, rb = max_state_;
    while (lb < rb) {
      int md = lb + (rb - lb) / 2;

      // count the sum of transitions over depth
      int sum = 0;
      for (int depth = 0; depth < static_cast<int>(memory_.size()); ++depth) {
        sum += transitions_.Get(memory_[depth], depth).Sum(md);
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
