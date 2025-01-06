#pragma once

#include <concepts>
#include <memory>

#include "src/evolv.h"
#include "state_coder.h"


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
                   std::shared_ptr<StateCoder<StateT, CodeT>> state_coder);

  //! Dereference iterator and encode the value it holds into CodeT
  CodeT EncodeCurrent() const;

  //! Advance iterator
  void Advance();

  //! Binary comparison, it dynamic_cast's EncodingIterIface to this class
  bool operator==(const EncodingIterIface<CodeT> &other) const;

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
               std::shared_ptr<StateCoder<StateT, CodeT>> state_coder);

  //! Dereference iterator and encode the value it holds into CodeT
  CodeT operator*() const;

  //! Advance iterator
  EncodingIter &operator++();

  //! Binary comparison that always boiles down to comparison between
  //! two EncodingIterImpl's.
  bool operator==(const EncodingIter<CodeT> &other) const;

  //! Binary comparison that boiles dows to inverting operator==
  bool operator!=(const EncodingIter<CodeT> &other) const;

 private:
  // EncodingIterImpl, implicitly casted to EncodingIterIface behind ptr
  std::unique_ptr<EncodingIterIface<CodeT>> impl_;
};

}  // namespace evolv::internal
