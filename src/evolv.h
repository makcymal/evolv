#pragma once

#include <cassert>
#include <chrono>
#include <concepts>
#include <deque>
#include <memory>
#include <vector>

#include "impl/base_chain.h"
#include "impl/forgor_chain.h"
#include "impl/rember_chain.h"
#include "impl/state_coder.h"
#include "impl/utils.h"

// todo: remove
#include "lib/dbg/dbg.h"


//! Entry-point namespace for the library
namespace evolv {

/*!
  \brief Class representing the Markov chain

  Chain states have type StateT that must be copy-constructible.
  Internally states are coded as integral CodeT (int by default).

  By definition, Markov chain is memoryless,
  which means that chains memory is limited to only one current state.
  In this case, ForgorChain is used as chain implementation.
  evolv proposes extension - Markov chain tracking some number > 0
  of previously visited states. This is called memory. In this case, RemberChain
  is used as chain implementation. When instantiating user can choose how many
  previous states to track. MarkovChain will manage implementation itself.

  After learning from sequences given in FeedSequence,
  while keeping track on currect state, chain can predict the subsequent state.
*/
template <class StateT, class CodeT = int>
  requires std::copy_constructible<StateT> && std::integral<CodeT>
class MarkovChain {
 public:
  explicit MarkovChain(int memorize_previous = 0) {
    int random_state =
        std::chrono::steady_clock::now().time_since_epoch().count();
    MarkovChain(memorize_previous, random_state);
  }

  //! Instantiate chain tracking the given number of previous states
  MarkovChain(int memorize_previous, int random_state) {
    assert(memorize_previous >= 0);
    if (memorize_previous == 0) {
      chain_ = std::make_unique<internal::ForgorChain<CodeT>>(random_state);
    } else {
      chain_ = std::make_unique<internal::RemberChain<CodeT>>(memorize_previous,
                                                              random_state);
    }
    state_coder_ = std::make_shared<internal::StateCoder<StateT, CodeT>>();
  }

  ~MarkovChain() = default;

  //! Learn from sequence and move to last state in sequence if needed
  template <class IterT>
    requires utils::is_iterator<IterT, StateT>
  void FeedSequence(IterT it, IterT end, bool update_memory = false) {
    chain_->FeedSequence(internal::EncodingIter<CodeT>(it, state_coder_),
                         internal::EncodingIter<CodeT>(end, state_coder_),
                         update_memory);
    dbg(chain_);
  }

  //! Predict the subsequent state based on current state and possibly memory,
  //! move to predicted state if needed
  StateT PredictState(bool update_memory = false) {
    return state_coder_->Decode(chain_->PredictState(update_memory));
  }

  //! Push a new state given as single value into memory forgetting the oldest
  //! states
  void UpdateMemory(StateT state) {
    chain_->UpdateMemory(state_coder_->Encode(state));
  }

  //! Push a new states given as pair of iterators into memory forgetting the
  //! oldest states
  template <class IterT>
    requires utils::is_iterator<IterT, StateT>
  void UpdateMemory(IterT it, IterT end) {
    chain_->UpdateMemory(internal::EncodingIter<CodeT>(it, state_coder_),
                         internal::EncodingIter<CodeT>(end, state_coder_));
  }

  //! Get deque of memory, where the first is the last seen state.
  std::deque<StateT> GetMemory() const {
    std::deque<CodeT> encoded_memory = chain_->GetMemory();
    std::deque<StateT> decoded_memory(encoded_memory.size());
    for (int i = 0; i < static_cast<int>(decoded_memory.size()); ++i) {
      decoded_memory[i] = state_coder_->Decode(encoded_memory[i]);
    }
    return decoded_memory;
  }

  // todo: remove
  DERIVE_DEBUG(chain_, state_coder_);

 private:
  //! Chain implementation, either ForgorChain or RemberChain
  std::unique_ptr<internal::BaseChain<CodeT>> chain_;
  //! State encoder and decoder (into and from CodeT)
  std::shared_ptr<internal::StateCoder<StateT, CodeT>> state_coder_;
};

}  // namespace evolv
