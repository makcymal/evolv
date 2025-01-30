#pragma once

#include <chrono>
#include <concepts>
#include <deque>
#include <random>
#include <vector>

#include "encoding_iter.h"
#include "lib/dbg/dbg.h"
#include "src/impl/fenwick_tree.h"


//! Namespace to keep all implementations hidden
namespace evolv::internal {

/*!
  \brief Base class for chain implementation

  This is the abstract class, MarkovChain stores it's instance
  as chain implementation. Both Forgor and Rember chains inherits it. This
  operates on sequences encoded by StateCoder, which are always integral.
*/
template <class CodeT>
  requires std::integral<CodeT>
class BaseChain {
 public:
  BaseChain(int memory_size, int random_state)
      : memory_size_(memory_size), rng_(random_state) {
  }

  int GetMemorySize() const {
    return memory_size_;
  }

  //! Get deque of memory, where the first is the last seen state.
  std::deque<CodeT> GetMemory() const {
    return memory_;
  }

  //! Push a new state given as single value into memory forgetting the oldest
  //! states
  void UpdateMemory(CodeT state) {
    // dbg("UpdateMemory called with:", state, memory_);
    if (static_cast<int>(memory_.size()) >= memory_size_) {
      memory_.pop_back();
    }
    memory_.push_front(state);
    // dbg(memory_);
  }

  //! Push a new state given as pair of iterators into memory forgetting the
  //! oldest states
  template <class IterT>
  void UpdateMemory(IterT it, IterT end) {
    for (; it != end; ++it) {
      // dbg(*it);
      if (static_cast<int>(memory_.size()) >= memory_size_) {
        memory_.pop_back();
      }
      memory_.push_front(*it);
    }
  }

  virtual ~BaseChain() = default;

  //! Learn from sequence and move to last state in sequence if needed or if
  //! there is no memory
  virtual void FeedSequence(EncodingIter<CodeT> it, EncodingIter<CodeT> end,
                            bool update_memory = false) = 0;

  //! Predict the subsequent state based on current state and possibly memory,
  //! move to predicted state if needed
  virtual CodeT PredictState(bool update_memory = false) = 0;

  DERIVE_DEBUG(memory_size_, memory_);

 protected:
  using CountT = int64_t;
  using FenwickCounter = FenwickTree<CountT, CodeT>;

  int memory_size_;
  // Last states where the chain ends
  std::deque<CodeT> memory_;
  // Random number generator, used in predicting next state
  mutable std::mt19937_64 rng_;
};

}  // namespace evolv::internal
