#pragma once

#include <chrono>
#include <concepts>
#include <deque>
#include <random>
#include <vector>

#include "encoding_iter.h"
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
  BaseChain(int memory_size, int random_state) : memory_size_(memory_size) {
    if (random_state == -1) {
      rng_ = std::mt19937_64(
          std::chrono::steady_clock::now().time_since_epoch().count());
    } else {
      rng_ = std::mt19937_64(random_state);
    }
  }

  //! Get deque of memory, where the first is the last seen state.
  std::deque<CodeT> GetMemory() const {
    return memory_;
  }

  //! Push a new state given as single value into memory forgetting the oldest
  //! states
  void UpdateMemory(CodeT state) {
    if (static_cast<int>(memory_.size()) >= memory_size_) {
      memory_.pop_back();
    }
    memory_.push_front(state);
  }

  //! Push a new state given as pair of iterators into memory forgetting the
  //! oldest states
  void UpdateMemory(EncodingIter<CodeT> it, EncodingIter<CodeT> end) {
    for (; it != end; ++it) {
      if (static_cast<int>(memory_.size()) >= memory_size_) {
        memory_.pop_back();
      }
      memory_.push_front(*it);
    }
  }

  virtual ~BaseChain() = 0;

  //! Learn from sequence and move to last state in sequence if needed or if
  //! there is no memory
  virtual void FeedSequence(EncodingIter<CodeT> it, EncodingIter<CodeT> end,
                            bool update_memory = false) = 0;

  //! Predict the subsequent state based on current state and possibly memory,
  //! move to predicted state if needed
  virtual CodeT PredictState(bool update_memory = false) = 0;

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
