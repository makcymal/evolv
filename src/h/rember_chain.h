#pragma once

#include <cstdint>
#include <deque>
#include <random>
#include <unordered_map>
#include <vector>

#include "base_chain.h"
#include "fenwick_tree.h"


namespace evolv::internal {

/*!
  \brief The implementor of BaseChain, that remembers N previous states

  While learning from given sequences, for all states it counts transitions
  to each subsequent state that comes in less that N + 1 steps.
  That's where inner class TransitCounter comes.
  Predicting next state is based on current state and counted transitions.
*/
template <class CodeT>
  requires std::integral<CodeT>
class RemberChain : public BaseChain<CodeT> {
 public:
  //! Set curr_state_ and max_state_ to undefined, initialize memory_ and rng_
  RemberChain(int memory = 1);

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
  //! For i = 0..N count transitions to subsequent states come in i steps
  using TransitCounter = std::vector<FenwickTree<int64_t, CodeT>>;

  //! Fixed number of previous states to remember
  int memory_;
  //! memory_ + 1 last states where the chain ends
  std::deque<CodeT> curr_state_;
  //! For all seen states count transitions into subsequent states come
  //! in less than memory_ + 1 steps
  std::unordered_map<CodeT, TransitCounter> transit_counters_;
  //! Total number of all transitions
  int64_t total_transitions_;
  //! State with maximum number ever seen
  CodeT max_state_;
  // Random number generator, used in predicting next state
  std::mt19937_64 rng_;

  //! Upper bound for the next state from the current one
  CodeT UpperBound(int64_t x);
};

}  // namespace evolv::internal
