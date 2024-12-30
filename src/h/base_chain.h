#pragma once

#include <concepts>
#include <vector>


namespace evolv::internal {

template <class StateT>
  requires std::integral<StateT>
class BaseChain {
 public:
  virtual void FeedSequence(const std::vector<StateT> &seq);

  virtual StateT PredictState(bool move_to_predicted = false);
};

}  // namespace evolv::internal
