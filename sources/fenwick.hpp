#pragma once

#include <vector>

template <typename T>
class Fenwick {
 public:
  Fenwick() = default;

  Fenwick(std::size_t n);

  Fenwick(const std::vector<T> &arr);
  
  ~Fenwick();

  std::size_t size() const;

  void resize(std::size_t n);

  void reset();

  void clear();

  T sum(int rb) const;

  T sum(int lb, int rb) const;

  T get(int idx) const;

  void add(int idx, T x);

  void set(int idx, T x);

  void append(T x);

  int upper_bound(int x) const;
  
 private:
  struct Impl;
  Impl *impl;
};
