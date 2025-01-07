#pragma once

#include <concepts>
#include <vector>
#include <deque>

#include "encoding_iter.h"


//! Namespace to keep all implementations hidden
namespace evolv::internal {

/*!
  \brief Base class for chain implementation

  This is the abstract class, MarkovChain::Impl stores it's instance
  as chain implementation. Both Forgor and Rember chains inherits it. This
  operates on sequences encoded by StateCoder, which are always integral.
*/
template <class CodeT>
  requires std::integral<CodeT>
class BaseChain {
 public:
  virtual ~BaseChain() = default;

  //! Learn from the sequence given as pair of iterators,
  //! move to last state in sequence if needed
  virtual void FeedSequence(EncodingIter<CodeT> it, EncodingIter<CodeT> end,
                            bool move_to_last = false) = 0;

  //! Predict the subsequent state from the current state,
  //! move to predicted state if needed
  virtual CodeT PredictState(bool move_to_predicted = false) = 0;

  //! Set new state given as single value, forget the current one
  virtual void SetCurrentState(CodeT state) = 0;

  //! Set new state given as pair of iterators, forget the actual one.
  //! The current state is defined as last memory_ + 1 states
  //! when iterating from it to end.
  virtual void SetCurrentState(EncodingIter<CodeT> it,
                               EncodingIter<CodeT> end) = 0;
                          
  //! Get deque of current state, where the first is the last seen state.     
  virtual std::deque<CodeT> GetCurrentState() const = 0;
};

}  // namespace evolv::internal
