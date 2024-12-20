#pragma once

#include <concepts>
#include <iterator>
#include <memory>


namespace evolv::internal {
template <class IterT, class DataT>
concept is_iterator =
  std::is_same_v<typename std::iterator_traits<IterT>::value_type, DataT>;
}


namespace evolv {

template <class StateT, class CodeT = int>
  requires std::copy_constructible<StateT> && std::integral<CodeT>
class MarkovChain {
 public:
  MarkovChain(int memory = 0) : impl(new Impl(memory)) {
  }

  template <class IterT>
    requires internal::is_iterator<IterT, StateT>
  void FeedSequence(IterT begin, IterT end) {
    impl->FeedSequence(begin, end);
  }

  StateT PredictState(bool move_to_predicted = true) {
    return impl->PredictState(move_to_predicted);
  }

 private:
  class Impl;
  std::unique_ptr<Impl> impl;
};

}  // namespace evolv
