#include "../markov_chain.h"

#include <cassert>
#include <unordered_map>
#include <vector>

#include "chain_impl.h"


namespace evolv {

template <class StateT, class CodeT>
  requires std::copy_constructible<StateT> && std::integral<CodeT>
class MarkovChain<StateT, CodeT>::Impl {
 public:
  Impl(int memory) {
    assert(memory >= 0 && "Constructing MarkovChain with memory < 0");
    if (memory == 0) {
      chain_ = internal::ForgorChain<CodeT>();
    } else {
      chain_ = internal::RemberChain<CodeT>(memory);
    }
  }

  template <class IterT>
    requires internal::is_iterator<IterT, StateT>
  void FeedSequence(IterT begin, IterT end) {
    std::vector<CodeT> enc_seq;
    for (IterT it = begin; it != end; ++it) {
      enc_seq.push_back(EncodeState(*it));
    }
    chain_.FeedSequence(enc_seq);
  }

  StateT PredictState(bool move_to_predicted) {
    return DecodeState(chain_.PredictState(move_to_predicted));
  }

 private:
  internal::BaseChain<CodeT> chain_;
  std::unordered_map<StateT, CodeT> state_encoder_;
  std::vector<StateT> state_decoder_;


  CodeT EncodeState(const StateT &state) {
    if (state_encoder_.count(state) == 0) {
      state_encoder_[state] = static_cast<CodeT>(state_decoder_.size());
      state_decoder_.push_back(state);
    }
    return state_encoder_[state];
  }

  StateT DecodeState(CodeT code) {
    return state_decoder_[code];
  }
};

}  // namespace evolv
