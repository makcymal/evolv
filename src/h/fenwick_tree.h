#pragma once

#include <concepts>
#include <cstdint>
#include <iterator>
#include <type_traits>
#include <vector>


namespace evolv::internal {

//! Fenwick tree implementation
template <class DataT, class SizeT = int64_t>
  requires std::integral<DataT> && std::signed_integral<SizeT>
class FenwickTree {
 public:
  //! Constructs empty Fenwick tree
  FenwickTree() = default;
  
  //! Costruct Fenwick tree of given size filled with zeros
  explicit FenwickTree(SizeT size);

  //! Return number of elements in Fenwick tree
  SizeT Size() const;

  //! Resize tree: shrink or expand
  void Resize(SizeT n);

  //! Count the sum over prefix [0, rb]
  DataT Sum(SizeT rb) const;

  //! Count the sum over segment [lb, rb]
  DataT Sum(SizeT lb, SizeT rb) const;

  //! Add x to element at given index
  void Add(SizeT idx, DataT x);

  //! Upper bound on prefix sums
  SizeT UpperBound(DataT x) const;

 private:
  std::vector<DataT> tree_;
};

}  // namespace evolv::internal
