#pragma once

#include <concepts>
#include <cstdint>
#include <iostream>
#include <vector>


namespace evolv::internal {

//! Fenwick tree implementation
template <class DataT, class SizeT = int64_t>
  requires std::integral<DataT> && std::signed_integral<SizeT>
class FenwickTree {
 public:
  //! Constructs empty Fenwick tree
  FenwickTree() : tree_(1, 0) {
  }

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
    new_size += 1;
    auto old_size = Size() + 1;
    tree_.resize(new_size, 0);
    for (SizeT idx = old_size; idx < new_size; ++idx) {
      auto sum = Sum(idx - (idx & -idx), old_size - 2);
      // std::cout << "adding to " << idx - 1 << " sum " << sum << std::endl;
      tree_[idx] += sum;
    }
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
    if (rb < lb) {
      return 0;
    }
    // std::cout << "sum on [" << lb << ", " << rb << "] = ";
    int sum_rb = Sum(rb), sum_lb = Sum(lb - 1);
    // std::cout << sum_rb << " - " << sum_lb << std::endl;
    return sum_rb - sum_lb;
  }
  
  DataT Total() const {
    return total_;
  }

  //! Add x to element at given index
  void Add(SizeT idx, DataT x) {
    if (idx >= Size()) {
      Resize(idx + 1);
    }
    idx++;
    for (; idx < Size() + 1; idx += (idx & -idx)) {
      tree_[idx] += x;
    }
    total_ += x;
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

  void dbg() {
    for (int i = 1; i < (int)(tree_.size()); ++i) {
      std::cout << tree_[i] << ' ';
    }
    std::cout << std::endl;
  }

 private:
  std::vector<DataT> tree_;
  DataT total_ = 0;
};

}  // namespace evolv::internal
