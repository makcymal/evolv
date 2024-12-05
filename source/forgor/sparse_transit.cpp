#include "sparse_transit.hpp"

#include <boost/bimap.hpp>
#include <chrono>
#include <random>

#include "fenwick.hpp"

struct SparseTransit::Impl {
  boost::bimap<int, int>  states;
  Fenwick<int>            transitions;
  int                     total_transitions;
  mutable std::mt19937_64 rng;
};


SparseTransit::SparseTransit(int approx_state_num) : impl(new Impl) {
  impl->transitions = Fenwick<int>(approx_state_num);
  impl->total_transitions = 0;
  impl->rng = std::mt19937_64(
    std::chrono::steady_clock::now().time_since_epoch().count());
}

SparseTransit::~SparseTransit() {
  delete impl;
}

void SparseTransit::clear() {
  impl->states.clear();
  impl->transitions.clear();
  impl->total_transitions = 0;
}

void SparseTransit::add_transition(int state, int count) {
  if (impl->states.left.count(state) == 0) {
    impl->states.insert({state, static_cast<int>(impl->transitions.size())});
    impl->transitions.append(0);
  }
  impl->transitions.add(impl->states.left.at(state), count);
  impl->total_transitions += count;
}

int SparseTransit::gen_transition() const {
  int index =
    impl->transitions.upper_bound(impl->rng() % impl->total_transitions);
  return impl->states.right.at(index);
}
