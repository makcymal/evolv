#include "chain_impl.h"

#include <cassert>
#include <cstdint>
#include <random>


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


template <class StateT>
RemberChain<StateT>::RemberChain(int memory) {
  assert(memory >= 1 && "Constructing RemberChain with memory < 1");
  memory_ = memory;
  total_transitions_ = 0;
  max_state = -1;
  rng = std::mt19937_64(
    std::chrono::steady_clock::now().time_since_epoch().count());
}

template <class StateT>
void RemberChain<StateT>::FeedSequence(const std::vector<StateT> &seq) {
  if (seq.empty()) {
    return;
  }

  max_state = std::max(max_state, seq[0]);

  for (int idx = 1; idx < seq.size(); ++idx) {
    max_state = std::max(max_state, seq[idx]);

    for (int dep = 0; dep <= memory_ && idx - 1 - dep >= 0; ++dep) {
      transit_counters_[seq[idx - 1 - dep]][dep].Add(seq[idx], 1);
      total_transitions_ += 1;
    }
  }

  if (seq.size() >= memory_ + 1) {
    for (int dep = 0, idx = seq.size() - 1; dep <= memory_; ++dep, --idx) {
      last_states_.push_back(seq[idx]);
    }
  }
}

template <class StateT>
StateT RemberChain<StateT>::PredictState(bool move_to_predicted) {
  assert(last_states_[0] != -1 &&
         "No FeedSequence called with sequence of length greater than memory");

  int64_t x = rng() % total_transitions_;
  StateT next_state = UpperBound(x);

  if (move_to_predicted) {
    last_states_.pop_back();
    last_states_.push_front(next_state);
  }

  return next_state;
}

template <class StateT>
StateT RemberChain<StateT>::UpperBound(int64_t x) {
  int lb = 0, rb = max_state;
  while (lb < rb) {
    int md = lb + (rb - lb) / 2;

    int sum = 0;
    for (int dep = 0; dep <= memory_; ++dep) {
      sum += transit_counters_[last_states_[dep]][dep].Sum(md);
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
