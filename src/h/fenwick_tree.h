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

  FenwickTree(SizeT size);

  //! Returns number of elements in Fenwick tree
  SizeT Size() const;

  //! Resizes
  void Resize(SizeT n);

  DataT Sum(SizeT rb) const;

  DataT Sum(SizeT lb, SizeT rb) const;

  void Add(SizeT idx, DataT x);

  SizeT UpperBound(DataT x) const;

 private:
  std::vector<DataT> tree_;
};

}  // namespace evolv::internal
