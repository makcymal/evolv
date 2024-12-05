#pragma once

#include <vector>
#include <unordered_map>

#include "evolv.hpp"


class Chain {
 public:
  virtual void fit(const std::vector<int> &);

  virtual std::vector<int> predict(int);
};

class ForgorChain : Chain {
 public:
  ForgorChain();
};

class RemberChain : Chain {
 public:
  RemberChain(unsigned memory = 1);
};

template <class Seq, class Traits>
class MarkovChain<Seq, Traits>::Impl {
  std::unordered_map<typename Traits::Elem, int> encoder;
  std::vector<typename Traits::Elem> decoder;
  
  public:
  Chain chain;
  
  int encode(const typename Traits::Elem &elem);
  typename Traits::Elem decode(int code);
};
