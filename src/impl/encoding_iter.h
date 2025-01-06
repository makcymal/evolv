#pragma once

#include <concepts>
#include <memory>

#include "state_coder.h"
#include "utils.h"


namespace evolv::internal {

/*!
  \brief Interface that hides StateT and IterT

  The purpose is to make EncodingIter independent from template class
  parameters StateT and IterT so EncodingIter stores object of this interface.
  Instead it's descendant EncodingIterImpl depend on StateT and IterT.
*/
template <class CodeT>
  requires std::integral<CodeT>
class EncodingIterIface {
 public:
  virtual ~EncodingIterIface() = default;

  //! Dereference iterator and encode the value it holds into CodeT
  virtual CodeT EncodeCurrent() const = 0;

  //! Advance iterator
  virtual void Advance() = 0;
};


/*!
  \brief The only implementation of EncodingIterIface

  While EncodingIterIface is meant to hide IterT and StateT, this
  explicitly depends on them along with CodeT. To hide IterT and StateT
  this is stored as EncodingIterIface and when needed dynamic_cast'ed back.
*/
template <class CodeT, class IterT, class StateT>
  requires std::integral<CodeT> && evolv::utils::is_iterator<IterT, StateT> &&
           std::copy_constructible<StateT>
class EncodingIterImpl : public EncodingIterIface<CodeT> {
 public:
  //! Inititalize underlying iterator and state coder
  EncodingIterImpl(IterT iter,
                   std::shared_ptr<StateCoder<StateT, CodeT>> state_coder)
      : iter_(iter), state_coder_(state_coder) {
  }

  //! Dereference iterator and encode the value it holds into CodeT
  CodeT EncodeCurrent() const {
    return state_coder_.Encode(*iter_);
  }

  //! Advance iterator
  void Advance() {
    ++iter_;
  }

  //! Binary comparison, it dynamic_cast's EncodingIterIface to this class
  bool operator==(const EncodingIterIface<CodeT> &other) const {
    return iter_ ==
           dynamic_cast<const EncodingIterImpl<CodeT, IterT, StateT> &>(other)
               .iter_;
  }

 private:
  //! Underlying iterator
  IterT iter_;
  //! State coder
  std::shared_ptr<StateCoder<StateT, CodeT>> state_coder_;
};


/*!
  \brief Iterator on StateT sequence that encodes it into CodeT

  Only constructor depends on template class parametes IterT and StateT.
  Although it's implementation EncodingIterImpl also depends on it, it's
  implicitly casted to EncodingIterIface behind unique_ptr.
  When iterating over sequence, it applies encoding on it's elements.
*/
template <class CodeT>
  requires std::integral<CodeT>
class EncodingIter {
 public:
  //! Create EncodingIterImpl and implicitly case it to EncodingIterIface
  template <class IterT, class StateT>
    requires evolv::utils::is_iterator<IterT, StateT> &&
             std::copy_constructible<StateT>
  EncodingIter(IterT iter,
               std::shared_ptr<StateCoder<StateT, CodeT>> state_coder) {
    impl_ = std::make_unique<EncodingIterImpl<CodeT, IterT, StateT>>(
        iter, state_coder);
  }

  //! Dereference iterator and encode the value it holds into CodeT
  CodeT operator*() const {
    return impl_->EncodeCurrent();
  }

  //! Advance iterator
  EncodingIter &operator++() {
    impl_->Advance();
    return *this;
  }

  //! Binary comparison that always boiles down to comparison between
  //! two EncodingIterImpl's.
  bool operator==(const EncodingIter<CodeT> &other) const {
    return impl_ == other.impl_;
  }

  //! Binary comparison that boiles dows to inverting operator==
  bool operator!=(const EncodingIter<CodeT> &other) const {
    return !(*this == other);
  }

 private:
  // EncodingIterImpl, implicitly casted to EncodingIterIface behind ptr
  std::unique_ptr<EncodingIterIface<CodeT>> impl_;
};

}  // namespace evolv::internal
