#include "src/evolv.h"

#include <cassert>
#include <memory>

#include "src/h/encoding_iter.h"
#include "src/h/forgor_chain.h"
#include "src/h/rember_chain.h"
#include "src/h/state_coder.h"


namespace evolv {

//! Private implementation of MarkovChain
template <class StateT, class CodeT>
  requires std::copy_constructible<StateT> && std::integral<CodeT>
class MarkovChain<StateT, CodeT>::Impl {
 public:
  //! Instantiate Impl with the memory size representing number of
  //! previous states remembered. Create underlying BaseChain based
  //! on the memory value: ForgorChain if memory=0, else RemberChain.
  Impl(int memory) {
    assert(memory >= 0 && "Constructing MarkovChain with memory < 0");
    if (memory == 0) {
      chain_ = internal::ForgorChain<CodeT>();
    } else {
      chain_ = internal::RemberChain<CodeT>(memory);
    }
    state_coder_ = std::make_shared<StateCoder<StateT, CodeT>>();
  }

  //! Learn from the sequence given as std::vector,
  //! move to last state in sequence if needed
  void FeedSequence(const std::vector<StateT> &seq, bool move_to_last) {
    FeedSequence(seq.begin(), seq.end(), move_to_last);
  }

  //! Learn from the sequence given as pair of iterators,
  //! move to last state in sequence if needed
  template <class IterT>
    requires utils::is_iterator<IterT, StateT>
  void FeedSequence(IterT begin, IterT end, bool move_to_last) {
    chain_.FeedSequence(EncodingIter(begin, state_coder_),
                        EncodingIter(end, state_coder_), move_to_last);
  }

  //! Predict the subsequent state from the current state,
  //! move to predicted state if needed
  StateT PredictState(bool move_to_predicted) {
    return state_coder_.Decode[chain_.PredictState(move_to_predicted)];
  }

  //! Set new current state, forget the actual one
  void SetCurrentState(StateT state) {
    chain_.SetCurrentState(state);
  }

  //! Set new current state, forget the actual one
  void SetCurrentState(std::vector<StateT> state) {
    chain_.SetCurrentState(state);
  }

 private:
  //! Chain implementation, either ForgorChain or RemberChain
  internal::BaseChain<CodeT> chain_;
  //! State encoder and decoder (into and from CodeT)
  std::shared_ptr<StateCoder<StateT, CodeT>> state_coder_;
};

}  // namespace evolv
