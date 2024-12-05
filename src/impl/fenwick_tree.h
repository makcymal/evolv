#pragma once

#include <concepts>
#include <cstdint>
#include <iterator>
#include <type_traits>
#include <vector>

//! Fenwick tree implementation
template <class DataT, class SizeT = int64_t>
  requires std::integral<DataT> && std::signed_integral<SizeT>
class FenwickTree {
 public:
  //! Constructs empty Fenwick tree
  FenwickTree(SizeT size = 0);

  //! Returns number of elements in Fenwick tree
  SizeT size() const;

  //! Resizes
  void resize(SizeT n);

  DataT sum(SizeT lb, SizeT rb) const;

  void add(SizeT idx, DataT x);

  void append(DataT x);

  SizeT upper_bound(DataT x) const;

 private:
  std::vector<DataT> tree_;

  DataT sum(SizeT rb) const;
};
