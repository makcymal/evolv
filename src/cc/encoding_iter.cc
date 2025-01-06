#include "src/h/encoding_iter.h"

#include <concepts>
#include <memory>

#include "src/h/state_coder.h"


namespace evolv::internal {

//! Inititalize underlying iterator and state coder
template <class CodeT, class IterT, class StateT>
  requires std::integral<CodeT> && utils::is_iterator<IterT, StateT> &&
               std::copy_constructible<StateT>
EncodingIterImpl<CodeT, IterT, StateT>::EncodingIterImpl(
    IterT iter, std::shared_ptr<StateCoder<StateT, CodeT>> state_coder)
    : iter_(iter), state_coder_(state_coder){};

//! Dereference iterator and encode the value it holds into CodeT
template <class CodeT, class IterT, class StateT>
  requires std::integral<CodeT> && utils::is_iterator<IterT, StateT> &&
           std::copy_constructible<StateT>
CodeT EncodingIterImpl<CodeT, IterT, StateT>::EncodeCurrent() const {
  return state_coder_.Encode(*iter_);
}

//! Advance iterator
template <class CodeT, class IterT, class StateT>
  requires std::integral<CodeT> && utils::is_iterator<IterT, StateT> &&
           std::copy_constructible<StateT>
void EncodingIterImpl<CodeT, IterT, StateT>::Advance() {
  ++iter_;
}

//! Binary comparison, it dynamic_cast's EncodingIterIface to this class
template <class CodeT, class IterT, class StateT>
  requires std::integral<CodeT> && utils::is_iterator<IterT, StateT> &&
           std::copy_constructible<StateT>
bool EncodingIterImpl<CodeT, IterT, StateT>::operator==(
    const EncodingIterIface<CodeT> &other) const {
  return iter_ ==
         dynamic_cast<const EncodingIterImpl<CodeT, IterT, StateT> &>(other)
             .iter_;
}


//! Create EncodingIterImpl and implicitly case it to EncodingIterIface
template <class CodeT>
  requires std::integral<CodeT>
template <class IterT, class StateT>
  requires utils::is_iterator<IterT, StateT> && std::copy_constructible<StateT>
EncodingIter<CodeT>::EncodingIter(
    IterT iter, std::shared_ptr<StateCoder<StateT, CodeT>> state_coder) {
  impl_ = std::make_unique<EncodingIterImpl<CodeT, IterT, StateT>>(iter,
                                                                   state_coder);
}

//! Dereference iterator and encode the value it holds into CodeT
template <class CodeT>
  requires std::integral<CodeT>
CodeT EncodingIter<CodeT>::operator*() const {
  return *impl_;
}

//! Advance iterator
template <class CodeT>
  requires std::integral<CodeT>
EncodingIter<CodeT> &EncodingIter<CodeT>::operator++() {
  impl_.Advance();
  return *this;
}

//! Binary comparison that always boiles down to comparison between
//! two EncodingIterImpl's.
template <class CodeT>
  requires std::integral<CodeT>
bool EncodingIter<CodeT>::operator==(const EncodingIter<CodeT> &other) const {
  return impl_ == other.impl_;
}

//! Binary comparison that boiles dows to inverting operator==
template <class CodeT>
  requires std::integral<CodeT>
bool EncodingIter<CodeT>::operator!=(const EncodingIter<CodeT> &other) const {
  return !(this == other);
}

}  // namespace evolv::internal
