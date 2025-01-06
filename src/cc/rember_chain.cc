#include "src/h/rember_chain.h"

#include <cassert>
#include <chrono>
#include <deque>


namespace evolv::internal {

//! Set curr_state_ and max_state_ to undefined, initialize memory_ and rng_
template <class CodeT>
  requires std::integral<CodeT>
RemberChain<CodeT>::RemberChain(int memory) {
  assert(memory >= 1 && "Constructing RemberChain with memory < 1");
  memory_ = memory;
  total_transitions_ = 0;
  max_state_ = -1;
  rng_ = std::mt19937_64(
      std::chrono::steady_clock::now().time_since_epoch().count());
}

//! Learn from the sequence given as pair of iterators,
//! move to last state in sequence if needed
template <class CodeT>
  requires std::integral<CodeT>
void RemberChain<CodeT>::FeedSequence(EncodingIter<CodeT> begin,
                                      EncodingIter<CodeT> end,
                                      bool move_to_last) {
  if (begin == end) {
    return;
  }

  auto it = begin;
  std::deque<CodeT> state{*it};
  max_state_ = std::max(max_state_, *it);
  ++it;
  
  // iterate over sequence and add new transitions
  // given the sequence s[0]..s[i]s[i+1]..s[i+N]..
  // for each d (depth) = 0..N add new transition from s[i] to s[i+d+1]
  for (; it != end; ++it) {
    for (int dep = 0; dep <= memory_ && dep < state.size(); ++dep) {
      transit_counters_[state[dep]][dep].Add(*it, 1);
      total_transitions_ += 1;
    }
    if (state.size() > memory_ + 1) {
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
template <class CodeT>
  requires std::integral<CodeT>
CodeT RemberChain<CodeT>::PredictState(bool move_to_predicted) {
  assert(curr_state_[0] != -1 &&
         "No FeedSequence called with sequence of length greater than memory");

  int64_t x = rng_() % total_transitions_;
  CodeT next_state = UpperBound(x);

  if (move_to_predicted) {
    curr_state_.pop_back();
    curr_state_.push_front(next_state);
  }

  return next_state;
}

//! Set new current state, forget the actual one
template <class CodeT>
  requires std::integral<CodeT>
void RemberChain<CodeT>::SetCurrentState(CodeT state) {
  assert(!"Unable to set a new state: incorrect size");
}

//! Set new current state, forget the actual one
template <class CodeT>
  requires std::integral<CodeT>
void RemberChain<CodeT>::SetCurrentState(std::vector<CodeT> state) {
  assert(state.size() < memory_ + 1 &&
         "Unable to set a new state: incorrect size");

  curr_state_ = std::deque<CodeT>(state.begin(), state.begin() + memory_ + 1);
}

//! Upper bound for the next state from the current one
template <class CodeT>
  requires std::integral<CodeT>
CodeT RemberChain<CodeT>::UpperBound(int64_t x) {
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

}  // namespace evolv::internal
