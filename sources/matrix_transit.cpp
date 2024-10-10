#include "matrix_transit.hpp"

#include <chrono>
#include <random>

#include "fenwick.hpp"

struct MatrixTransit::Impl {
  Fenwick<int>            transitions;
  int                     total_transitions;
  mutable std::mt19937_64 rng;
};

MatrixTransit::MatrixTransit(int states_num) : impl(new Impl) {
  impl->transitions.resize(states_num);
  impl->total_transitions = 0;
  impl->rng = std::mt19937_64(
    std::chrono::steady_clock::now().time_since_epoch().count());
}

MatrixTransit::~MatrixTransit() {
  delete impl;
}

void MatrixTransit::clear() {
  impl->transitions.reset();
  impl->total_transitions = 0;
}

void MatrixTransit::add_transition(int state, int count) {
  impl->transitions.add(state, count);
  impl->total_transitions += count;
}

int MatrixTransit::gen_transition() const {
  return impl->transitions.upper_bound(impl->rng() % impl->total_transitions);
}


int main() {
  return 0;
}
