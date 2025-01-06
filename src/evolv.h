#pragma once

#include <concepts>
#include <iterator>
#include <memory>
#include <vector>


//! Entry-point namespace for the library
namespace evolv {

//! Namespace for utilities that cannot be placed in evolv::internal
namespace utils {

//! Concept for checking if IterT is iterator on elements
//! of type DataT, DataT & or const DataT &
template <class IterT, class DataT>
concept is_iterator =
    std::is_same_v<typename std::iterator_traits<IterT>::value_type, DataT> ||
    std::is_same_v<typename std::iterator_traits<IterT>::value_type, DataT &> ||
    std::is_same_v<typename std::iterator_traits<IterT>::value_type,
                   const DataT &>;

}  // namespace utils


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
  MarkovChain(int memory = 0) : impl(new Impl(memory)) {
  }

  //! Learn from sequence given as std::vector,
  //! move to last state in sequence if needed
  void FeedSequence(const std::vector<StateT> &seq, bool move_to_last = false) {
    impl->FeedSequence(seq, move_to_last);
  }

  //! Learn from sequence given as pair of iterators,
  //! move to last state in sequence if needed
  template <class IterT>
    requires utils::is_iterator<IterT, StateT>
  void FeedSequence(IterT begin, IterT end, bool move_to_last = false) {
    impl->FeedSequence(begin, end, move_to_last);
  }

  //! Predict the subsequent state from the current state,
  //! move to predicted state if needed
  StateT PredictState(bool move_to_predicted = false) {
    return impl->PredictState(move_to_predicted);
  }

  //! Set new current state, forget the actual one
  void SetCurrentState(StateT state) {
    impl->SetCurrentState(state);
  }

  //! Set new current state, forget the actual one
  void SetCurrentState(std::vector<StateT> state) {
    impl->SetCurrentState(state);
  }

 private:
  //! Private implementation class
  class Impl;
  
  //! Private implementation object
  std::unique_ptr<Impl> impl;
};

}  // namespace evolv
