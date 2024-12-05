#include <string>

#include "../header/source.hpp"


template <class Seq>
class SeqTraits {
  struct Dummy {};
  using Elem = Dummy;

  static std::pair<int, int> tokenize(const Seq &seq);

  static Seq build_sequence(std::vector<Elem> elements);
};


template <>
class SeqTraits<std::vector<int>> {
  using Elem = int;
  
  static 
};


template <class Seq, class Traits>
MarkovChain<Seq, Traits>::MarkovChain(unsigned memory) : impl(new Impl) {
  if (memory == 0) {
    impl->chain = ForgorChain();
  } else {
    impl->chain = RemberChain(memory);
  }
}

template <class Seq, class Traits>
int MarkovChain<Seq, Traits>::Impl::encode(const typename Traits::Elem &elem) {
  if (encoder.count(elem) == 0) {
    encoder[elem] = static_cast<int>(decoder.size());
    decoder.push_back(elem);
  }
  return encoder[elem];
}

template <class Seq, class Traits>
typename Traits::Elem MarkovChain<Seq, Traits>::Impl::decode(int code) {
  return decoder[code];
}

template <class Seq, class Traits>
MarkovChain<Seq, Traits>::~MarkovChain() {
  delete impl;
}

template <class Seq, class Traits>
void MarkovChain<Seq, Traits>::fit(const Seq &seq) {
  std::vector<int> encoded_seq;
  for (auto [begin, end] = Traits::tokenize(seq); begin != end; ++begin) {
    encoded_seq.push_back(impl->encode(*begin));
  }
  impl->chain.fit(encoded_seq);
}

template <class Seq, class Traits>
Seq MarkovChain<Seq, Traits>::predict(unsigned n_states) {
  std::vector<typename Traits::Elem> future_states;
  if (n_states == 0) {
    return Traits::build_sequence(future_states);
  }

  std::vector<int> encoded_seq = impl->chain.predict(n_states);
  future_states.reserve(n_states);
  for (int encoded_elem : encoded_seq) {
    future_states.push_back(impl->decode(encoded_elem));
  }

  return Traits::build_sequence(std::move(future_states));
}
