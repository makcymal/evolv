#include "src/h/forgor_chain.h"

#include <chrono>


namespace evolv::internal {

//! Sets current state to undefined and initialize random number generator
template <class CodeT>
  requires std::integral<CodeT>
ForgorChain<CodeT>::ForgorChain() {
  curr_state_ = -1;
  rng_ = std::mt19937_64(
      std::chrono::steady_clock::now().time_since_epoch().count());
}

//! Learn from the sequence given as pair of iterators,
//! move to last state in sequence if needed
template <class CodeT>
  requires std::integral<CodeT>
void ForgorChain<CodeT>::FeedSequence(EncodingIter<CodeT> begin,
                                      EncodingIter<CodeT> end,
                                      bool move_to_last) {
  if (begin == end) {
    return;
  }

  auto it = begin;
  CodeT state = *it;
  ++it;
  for (; it != end; ++it) {
    transit_counters_[state].Add(*it, 1);
    state = *it;
  }

  if (move_to_last) {
    curr_state_ = state;
  }
}

//! Predict the subsequent state from the current state,
//! move to predicted state if needed
template <class CodeT>
  requires std::integral<CodeT>
CodeT ForgorChain<CodeT>::PredictState(bool move_to_predicted) {
  assert(curr_state_ != -1 && "No FeedSequence called");

  int64_t x = rng_() % transit_counters_[curr_state_].TotalTransitions();
  CodeT next_state = transit_counters_[curr_state_].UpperBound(x);

  if (move_to_predicted) {
    curr_state_ = next_state;
  }

  return next_state;
}

//! Set new current state, forget the actual one
template <class CodeT>
  requires std::integral<CodeT>
void ForgorChain<CodeT>::SetCurrentState(CodeT state) {
  curr_state_ = state;
}

//! Set new current state, forget the actual one
template <class CodeT>
  requires std::integral<CodeT>
void ForgorChain<CodeT>::SetCurrentState(std::vector<CodeT> state) {
  assert(!state.empty() && "Vector of new current set is empty");
  curr_state_ = state.back();
}


//! Getter for total_transitions_
template <class CodeT>
  requires std::integral<CodeT>
int64_t ForgorChain<CodeT>::TransitCounter::TotalTransitions() {
  return total_transitions_;
}

//! Add <count> new transitions into state <dest>
template <class CodeT>
  requires std::integral<CodeT>
void ForgorChain<CodeT>::TransitCounter::Add(CodeT dest, int64_t count) {
  count_.Add(dest, 1);
  total_transitions_ += 1;
}

//! Query upper bound on prefix sums of Fenwick tree
template <class CodeT>
  requires std::integral<CodeT>
CodeT ForgorChain<CodeT>::TransitCounter::UpperBound(int64_t x) {
  return count_.UpperBound(x);
}


}  // namespace evolv::internal
