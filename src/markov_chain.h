#pragma once

#include <concepts>
#include <iterator>
#include <memory>


namespace evolv {

template <class State, class Code = int>
  requires std::copy_constructible<State> && std::integral<Code>
class MarkovChain {
 public:
  MarkovChain(int memory = 0) : impl(new Impl(memory)) {
  }

  template <class Iter>
    requires std::is_same_v<typename std::iterator_traits<Iter>::value_type,
                            State>
  void FeedSequence(Iter begin, Iter end) {
    impl->FeedSequence(begin, end);
  }

  State PredictState(bool move_to_predicted = true) {
    return impl->PredictState(move_to_predicted);
  }

 private:
  class Impl;
  std::unique_ptr<Impl> impl;
};

}  // namespace evolv
