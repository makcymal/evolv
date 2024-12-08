#pragma once

#include <chrono>
#include <cstdint>
#include <deque>
#include <map>
#include <random>
#include <unordered_map>

#include "../markov_chain.h"
#include "fenwick_tree.h"


namespace evolv::internal {

template <class StateT>
  requires std::integral<StateT>
class BaseChain {
 public:
  virtual void FeedSequence(const std::vector<StateT> &seq);

  virtual StateT PredictState(bool move_to_predicted = false);
};


template <class StateT>
class ForgorChain : public BaseChain<StateT> {
 public:
  class TransitCounter {
   public:
    TransitCounter() = default;

    int64_t TotalTransitions();

    void Add(StateT dest, int64_t count);

    StateT UpperBound(int64_t x);

   private:
    int64_t total_transitions_ = 0;
    FenwickTree<int64_t, StateT> count_;
  };

  ForgorChain();

  void FeedSequence(const std::vector<StateT> &seq);

  StateT PredictState(bool move_to_predicted = false);

 private:
  StateT last_state_;
  std::unordered_map<StateT, TransitCounter> transit_counters_;
  std::mt19937_64 rng;
};


template <class StateT>
class RemberChain : public BaseChain<StateT> {
 public:
  RemberChain(int memory = 1);

  void FeedSequence(const std::vector<StateT> &seq);

  StateT PredictState(bool move_to_predicted = false);

 private:
  using TransitCounter = std::vector<FenwickTree<int64_t, StateT>>;

  int memory_;
  std::deque<StateT> last_states_;
  std::unordered_map<StateT, TransitCounter> transit_counters_;
  int64_t total_transitions_;
  StateT max_state;
  std::mt19937_64 rng;

  StateT UpperBound(int64_t x);
};

}  // namespace evolv::internal
