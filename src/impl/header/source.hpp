#pragma once

#include <unordered_map>
#include <vector>

#include "evolv.hpp"


template <class Seq, class Traits>
class MarkovChain<Seq, Traits>::Impl {
 public:
   chain;

  int encode(const typename Traits::Elem &elem);
  typename Traits::Elem decode(int code);

 private:
  std::vector<typename Traits::Elem> decoder;
  std::unordered_map<typename Traits::Elem, int> encoder;
};
