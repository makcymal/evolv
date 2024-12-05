#include "../markov_chain.h"

#include <cassert>
#include <functional>
#include <unordered_map>
#include <vector>

#include "chain_impl.h"


namespace evolv {

template <class State, class Code>
  requires std::copy_constructible<State> && std::integral<Code>
class MarkovChain<State, Code>::Impl {
 public:
  Impl(int memory) {
    assert(memory >= 0 && "Constructing MarkovChain with memory < 0");
    if (memory == 0) {
      chain_ = internal::ForgorChain<Code>();
    } else {
      chain_ = internal::RemberChain<Code>(memory);
    }
  }

  template <class Iter>
    requires internal::is_iterator<State, Iter>
  void FeedSequence(Iter begin, Iter end) {
    chain_.FeedSequence(EncodeIterator(begin, EncodeState),
                        EncodeIterator(end, EncodeState));
  }

  State PredictState(bool move_to_predicted) {
    return DecodeState(chain_.PredictState(move_to_predicted));
  }


 private:
  internal::BaseChain<Code> chain_;
  std::unordered_map<State, Code> state_encoder_;
  std::vector<State> state_decoder_;


  Code EncodeState(const State &state) {
    if (state_encoder_.count(state) == 0) {
      state_encoder_[state] = static_cast<Code>(state_decoder_.size());
      state_decoder_.push_back(state);
    }
    return state_encoder_[state];
  }

  State DecodeState(Code code) {
    return state_decoder_[code];
  }


  template <class Iter>
    requires internal::is_iterator<State, Iter>
  class EncodeIterator {
   public:
    // postfix increment
    void operator++(Iter) {
      iter_++;
    }

    Code operator*() const {
      return encoder_func_(*iter_);
    }

   private:
    using encoder_func_t = std::function<Code(const State &state)>;

    EncodeIterator(Iter iter, encoder_func_t encoder_func)
    : iter_(iter), encoder_func_(encoder_func) {
    }

    Iter iter_;
    encoder_func_t encoder_func_;
  };
};

}  // namespace evolv
