#pragma once

#include <deque>

#include "../markov_chain.h"
#include "fenwick_tree.h"


namespace evolv::internal {

template <class DataT, class IterT>
concept is_iterator =
  std::is_same_v<typename std::iterator_traits<IterT>::value_type, DataT>;


template <class StateT>
  requires std::integral<StateT>
class BaseChain {
 public:
  template <class IterT>
    requires is_iterator<StateT, IterT>
  void FeedSequence(IterT begin, IterT end);

  int PredictState(bool move_to_predicted = false);
};


template <class StateT>
class ForgorChain : public BaseChain<StateT> {
 public:
  ForgorChain();

 private:
  StateT last_state_;
};


template <class StateT>
class RemberChain : public BaseChain<StateT> {
 public:
  RemberChain(int memory = 1);

 private:
  int memory_;
  std::deque<StateT> last_states_;
};

}  // namespace evolv::internal
