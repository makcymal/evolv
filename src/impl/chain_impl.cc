
#include "chain_impl.h"

#include <cassert>

namespace evolv::internal {

template <class StateT>
ForgorChain<StateT>::ForgorChain() {
  last_state_ = -1;
}

template <class StateT>
void ForgorChain<StateT>::FeedSequence(const std::vector<StateT> &seq) {
  
}

template <class StateT>
StateT ForgorChain<StateT>::PredictState(bool move_to_predicted) {
  
}


template <class StateT>
RemberChain<StateT>::RemberChain(int memory) {
  assert(memory >= 1 && "Constructing RemberChain with memory < 1");
  memory_ = memory;
}

template <class StateT>
void RemberChain<StateT>::FeedSequence(const std::vector<StateT> &seq) {
  
}

template <class StateT>
StateT RemberChain<StateT>::PredictState(bool move_to_predicted) {
  
}

}  // namespace evolv::internal
