#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>


namespace evolv::internal {

/*!
  \brief State encoder and decoder (into and from CodeT)

  This is used with BaseChain that can operate only on integral
  types. This ensures biection between arbitrary states and integral codes.
  The StateT must be hashable.
*/
template <class StateT, class CodeT>
  requires std::copy_constructible<StateT> && std::integral<CodeT>
class StateCoder {
 public:
  StateCoder() = default;

  //! Map state to code
  CodeT Encode(const StateT &state) {
    if (!encoder_.contains(state)) {
      encoder_[state] = decoder_.size();
      decoder_.push_back(state);
    }
    return encoder_[state];
  }

  //! Map code to state
  StateT Decode(CodeT code) {
    std::cout << "decoded to: " << decoder_[code] << std::endl;
    return decoder_[code];
  }

 private:
  //! Stores mapping from state to code
  std::unordered_map<StateT, CodeT> encoder_;
  //! Stores mapping from code to state
  std::vector<StateT> decoder_;
};

}  // namespace evolv::internal
