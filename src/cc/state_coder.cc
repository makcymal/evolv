#include "src/h/state_coder.h"

//! Map state to code
template <class StateT, class CodeT>
  requires std::copy_constructible<StateT> && std::integral<CodeT>
CodeT StateCoder<StateT, CodeT>::Encode(const StateT &state) {
  if (!encoder_.contains(state)) {
    encoder_[state] = decoder_.size();
    decoder_.push_back(state);
  }
  return encoder_[state];
}


//! Map code to state
template <class StateT, class CodeT>
  requires std::copy_constructible<StateT> && std::integral<CodeT>
StateT StateCoder<StateT, CodeT>::Decode(CodeT code) {
  return decoder_[code];
}
