#pragma once

#include <cstdint>
#include <random>
#include <unordered_map>

#include "base_chain.h"
#include "encoding_iter.h"
#include "fenwick_tree.h"


namespace evolv::internal {

/*!
  \brief The implementor of BaseChain, that forgets all previous states

  While learning from given sequences, for all seen states it counts
  transitions to each state. That's where inner class TransitCounter comes.
  Predicting next state is based on current state and counted transitions.
  The more transitions from state A to state B -> the more probability
  that standing in state A the chain will predict state B.
*/
template <class CodeT>
  requires std::integral<CodeT>
class ForgorChain : public BaseChain<CodeT> {
 public:
  //! Set curr_state_ to undefined and initialize rng_
  ForgorChain();

  //! Learn from the sequence given as pair of iterators,
  //! move to last state in sequence if needed
  void FeedSequence(EncodingIter<CodeT> begin, EncodingIter<CodeT> end,
                    bool move_to_last = false);

  //! Predict the subsequent state from the current state,
  //! move to predicted state if needed
  CodeT PredictState(bool move_to_predicted = false);

  //! Set new current state, forget the actual one
  void SetCurrentState(CodeT state);

  //! Set new current state, forget the actual one
  void SetCurrentState(std::vector<CodeT> state);

 private:
  /*!
    \brief Counts transitions from one state to all states
    
    The Fenwick tree is used internally to add new transitions and
    quering upper bound on prefix sums, both in O(logn). The last one 
    is used to predict next state according to how many time it occured.
  */
  class TransitCounter {
   public:
    TransitCounter() = default;
    
    //! Getter for total_transitions_
    int64_t TotalTransitions();

    //! Add <count> new transitions into state <dest>
    void Add(CodeT dest, int64_t count);

    //! Query upper bound on prefix sums of Fenwick tree
    CodeT UpperBound(int64_t x);

   private:
    //! Total number of transitions stored in Fenwick tree
    int64_t total_transitions_ = 0;
    //! Fenwick tree counts transitions
    FenwickTree<int64_t, CodeT> count_;
  };

  // Last state where the chain ends
  CodeT curr_state_;
  // For all states count transitions to each state
  std::unordered_map<CodeT, TransitCounter> transit_counters_;
  // Random number generator, used in predicting next state
  std::mt19937_64 rng_;
};

}  // namespace evolv::internal
