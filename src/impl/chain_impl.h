#pragma once

#include <deque>

#include "../markov_chain.h"


namespace evolv::internal {
  
template <class T, class Iter>
concept is_iterator =
  std::is_same_v<typename std::iterator_traits<Iter>::value_type, T>;


template <class T>
  requires std::integral<T>
class BaseChain {
 public:
  template <class Iter>
    requires is_iterator<T, Iter>
  void FeedSequence(Iter begin, Iter end);

  int PredictState(bool move_to_predicted = false);
};


template <class T>
class ForgorChain : public BaseChain<T> {
 public:
  ForgorChain();

 private:
  T last_state_;
};


template <class T>
class RemberChain : public BaseChain<T> {
 public:
  RemberChain(int memory = 1);

 private:
  int memory_;
  std::deque<T> last_states_;
};

}  // namespace evolv::internal
