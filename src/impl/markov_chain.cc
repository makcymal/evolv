#include "../markov_chain.h"

#include <cassert>
#include <functional>
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
    requires internal::is_iterator<StateT, IterT>
  void FeedSequence(IterT begin, IterT end) {
    chain_.FeedSequence(EncodeIterator(begin, EncodeState),
                        EncodeIterator(end, EncodeState));
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


  template <class IterT>
    requires internal::is_iterator<StateT, IterT>
  class EncodeIterator {
   public:
    // postfix increment
    void operator++(IterT) {
      iter_++;
    }

    CodeT operator*() const {
      return encoder_func_(*iter_);
    }

   private:
    using encoder_func_t = std::function<CodeT(const StateT &state)>;

    EncodeIterator(IterT iter, encoder_func_t encoder_func)
    : iter_(iter), encoder_func_(encoder_func) {
    }

    IterT iter_;
    encoder_func_t encoder_func_;
  };
};

}  // namespace evolv
