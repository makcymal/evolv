#pragma once

template <class Seq> class SeqTraits;

template <class Seq, class Traits = SeqTraits<Seq>>
class MarkovChain {
 public:
  MarkovChain(unsigned memory = 0);

  ~MarkovChain();

  void fit(const Seq &seq);

  Seq predict(unsigned n_states = 1);

 private:
  struct Impl;
  Impl *impl;
};
