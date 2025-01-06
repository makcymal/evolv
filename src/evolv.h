#pragma once

#include <cassert>
#include <concepts>
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
    state_coder_ = std::make_shared<StateCoder<StateT, CodeT>>();
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
  void FeedSequence(IterT begin, IterT end, bool move_to_last = false) {
    chain_->FeedSequence(EncodingIter(begin, state_coder_),
                        EncodingIter(end, state_coder_), move_to_last);
  }

  //! Predict the subsequent state from the current state,
  //! move to predicted state if needed
  StateT PredictState(bool move_to_predicted = false) {
    return state_coder_.Decode[chain_->PredictState(move_to_predicted)];
  }

  //! Set new current state, forget the actual one
  void SetCurrentState(StateT state) {
    chain_->SetCurrentState(state);
  }

  //! Set new current state, forget the actual one
  void SetCurrentState(std::vector<StateT> state) {
    chain_->SetCurrentState(state);
  }

 private:
  //! Chain implementation, either ForgorChain or RemberChain
  std::unique_ptr<internal::BaseChain<CodeT>> chain_;
  //! State encoder and decoder (into and from CodeT)
  std::shared_ptr<StateCoder<StateT, CodeT>> state_coder_;
};

}  // namespace evolv
