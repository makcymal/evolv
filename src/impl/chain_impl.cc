
#include "chain_impl.h"

#include <cassert>

namespace evolv::internal {
template <class T>
ForgorChain<T>::ForgorChain() {
  last_state_ = -1;
}

template <class T>
RemberChain<T>::RemberChain(int memory) {
  assert(memory >= 1 && "Constructing RemberChain with memory < 1");
  memory_ = memory;
}
}  // namespace evolv::internal
