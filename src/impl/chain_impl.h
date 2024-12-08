#pragma once

#include <deque>

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
  ForgorChain();
  
  void FeedSequence(const std::vector<StateT> &seq);
  
  StateT PredictState(bool move_to_predicted = false);

 private:
  StateT last_state_;
};


template <class StateT>
class RemberChain : public BaseChain<StateT> {
 public:
  RemberChain(int memory = 1);
  
  void FeedSequence(const std::vector<StateT> &seq);

  StateT PredictState(bool move_to_predicted = false);

 private:
  int memory_;
  std::deque<StateT> last_states_;
};

}  // namespace evolv::internal
