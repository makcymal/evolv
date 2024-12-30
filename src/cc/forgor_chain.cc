#include "../h/forgor_chain.h"

#include <chrono>


namespace evolv::internal {

template <class StateT>
ForgorChain<StateT>::ForgorChain() {
  last_state_ = -1;
  rng = std::mt19937_64(
      std::chrono::steady_clock::now().time_since_epoch().count());
}

template <class StateT>
void ForgorChain<StateT>::FeedSequence(const std::vector<StateT> &seq) {
  for (int idx = 1; idx < seq.size(); ++idx) {
    transit_counters_[seq[idx - 1]].Add(seq[idx], 1);
  }

  if (!seq.empty()) {
    last_state_ = seq.back();
  }
}

template <class StateT>
StateT ForgorChain<StateT>::PredictState(bool move_to_predicted) {
  assert(last_state_ != -1 && "No FeedSequence called");

  int64_t x = rng() % transit_counters_[last_state_].TotalTransitions();
  StateT next_state = transit_counters_[last_state_].UpperBound(x);

  if (move_to_predicted) {
    last_state_ = next_state;
  }

  return next_state;
}


template <class StateT>
int64_t ForgorChain<StateT>::TransitCounter::TotalTransitions() {
  return total_transitions_;
}

template <class StateT>
void ForgorChain<StateT>::TransitCounter::Add(StateT dest, int64_t count) {
  count_.Add(dest, 1);
  total_transitions_ += 1;
}

template <class StateT>
StateT ForgorChain<StateT>::TransitCounter::UpperBound(int64_t x) {
  return count_.UpperBound(x);
}


}  // namespace evolv::internal
