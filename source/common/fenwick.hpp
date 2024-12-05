#pragma once

#include <vector>

//! Fenwick tree implementation
template <typename T>
class Fenwick {
 public:
  //! Constructs empty Fenwick tree
  Fenwick() = default;

  //! Constructs Fenwick tree of given size
  Fenwick(std::size_t n);

  //! Constructs Fenwick tree with given data
  Fenwick(const std::vector<T> &data);
  
  //! Destroys Fenwick tree
  ~Fenwick();

  //! Returns numbers of elements in Fenwick tree
  std::size_t size() const;

  //! Resizes 
  void resize(std::size_t n);

  void reset();

  void clear();

  T sum(int rb) const;

  T sum(int lb, int rb) const;

  T get(int idx) const;

  void add(int idx, T x);

  void set(int idx, T x);

  void append(T x);

  int upper_bound(T x) const;
  
 private:
  struct Impl;
  Impl *impl;
};
