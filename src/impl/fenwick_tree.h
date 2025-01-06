#pragma once

#include <concepts>
#include <cstdint>
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
  explicit FenwickTree(SizeT size)
      : tree_(static_cast<std::size_t>(size) + 1, 0) {
  }

  //! Return number of elements in Fenwick tree
  SizeT Size() const {
    return static_cast<SizeT>(tree_.size()) - 1;
  }


  //! Resize tree: shrink or expand
  void Resize(SizeT new_size) {
    tree_.resize(static_cast<std::size_t>(new_size) + 1, 0);
  }

  //! Count the sum over prefix [0, rb]
  DataT Sum(SizeT rb) const {
    rb++;
    DataT res = 0;
    for (; rb > 0; rb -= (rb & -rb)) {
      res += tree_[rb];
    }
    return res;
  }

  //! Count the sum over segment [lb, rb]
  DataT Sum(SizeT lb, SizeT rb) const {
    rb++;
    lb++;
    return sum(rb) - sum(lb - 1);
  }

  //! Add x to element at given index
  void Add(SizeT idx, DataT x) {
    if (idx >= Size()) {
      Resize(idx + 1);
    }
    idx++;
    for (; idx < static_cast<SizeT>(tree_.size()); idx += (idx & -idx)) {
      tree_[idx] += x;
    }
  }

  //! Upper bound on prefix sums
  SizeT UpperBound(DataT x) const {
    SizeT idx = 0;
    for (SizeT lmb = std::__bit_floor(tree_.size()); lmb >= 1; lmb >>= 1) {
      if (idx + lmb < static_cast<SizeT>(tree_.size()) and
          tree_[idx + lmb] <= x) {
        idx += lmb;
        x -= tree_[idx];
      }
    }
    return idx;
  }

 private:
  std::vector<DataT> tree_;
};

}  // namespace evolv::internal
