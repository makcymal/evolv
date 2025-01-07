#pragma once

#include <cassert>
#include <concepts>
#include <deque>
#include <memory>
#include <vector>

#include "impl/base_chain.h"
#include "impl/forgor_chain.h"
#include "impl/rember_chain.h"
#include "impl/state_coder.h"
#include "impl/utils.h"


//! Entry-point namespace for the library
namespace evolv {

/*!
  \brief Class representing the Markov chain

  Chain states have type StateT that must be copy-constructible.
  Internally states are coded as integral CodeT (int by default).
  When instantiating user can choose how many previous states to track.
  After learning from sequences given in FeedSequence, while keeping
  track on currect state, chain can predict the subsequent state.
*/
template <class StateT, class CodeT = int>
  requires std::copy_constructible<StateT> && std::integral<CodeT>
class MarkovChain {
 public:
  //! Instantiate chain tracking the given number of previous states
  explicit MarkovChain(int memory = 0) {
    assert(memory >= 0 && "Constructing MarkovChain with memory < 0");
    if (memory == 0) {
      chain_ = std::make_unique<internal::ForgorChain<CodeT>>();
    } else {
      chain_ = std::make_unique<internal::RemberChain<CodeT>>(memory);
    }
    state_coder_ = std::make_shared<internal::StateCoder<StateT, CodeT>>();
  }

  ~MarkovChain() = default;

  //! Learn from sequence given as std::vector,
  //! move to last state in sequence if needed
  void FeedSequence(const std::vector<StateT> &seq, bool move_to_last = false) {
    FeedSequence(seq.begin(), seq.end(), move_to_last);
  }

  //! Learn from sequence given as pair of iterators,
  //! move to last state in sequence if needed
  template <class IterT>
    requires utils::is_iterator<IterT, StateT>
  void FeedSequence(IterT it, IterT end, bool move_to_last = false) {
    chain_->FeedSequence(EncodingIter(it, state_coder_),
                         EncodingIter(end, state_coder_), move_to_last);
  }

  //! Predict the subsequent state from the current state,
  //! move to predicted state if needed
  StateT PredictState(bool move_to_predicted = false) {
    return state_coder_.Decode[chain_->PredictState(move_to_predicted)];
  }

  //! Set new state given as single value, forget the current one
  void SetCurrentState(StateT state) {
    chain_->SetCurrentState(state_coder_->Encode(state));
  }

  //! Set new state given as pair of iterators, forget the actual one.
  //! The current state is defined as last memory_ + 1 states
  //! when iterating from it to end.
  template <class IterT>
    requires utils::is_iterator<IterT, StateT>
  void SetCurrentState(IterT it, IterT end) {
    chain_->SetCurrentState(EncodingIter(it, state_coder_),
                            EncodingIter(end, state_coder_));
  }

  //! Get deque of current state, where the first is the last seen state.
  std::deque<StateT> GetCurrentState() const {
    std::deque<CodeT> encoded_state = chain_->GetCurrentState();
    std::deque<StateT> decoded_state(encoded_state.size());
    for (int i = 0; i < static_cast<int>(decoded_state.size()); ++i) {
      decoded_state[i] = state_coder_->Decode(encoded_state[i]);
    }
    return decoded_state;
  }

 private:
  //! Chain implementation, either ForgorChain or RemberChain
  std::unique_ptr<internal::BaseChain<CodeT>> chain_;
  //! State encoder and decoder (into and from CodeT)
  std::shared_ptr<internal::StateCoder<StateT, CodeT>> state_coder_;
};

}  // namespace evolv
