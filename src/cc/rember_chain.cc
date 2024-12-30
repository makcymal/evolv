#include "src/h/rember_chain.h"

#include <cassert>
#include <chrono>


namespace evolv::internal {

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
